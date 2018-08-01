#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/socket.h>   
#include <netinet/in.h>    
#include <netdb.h>  
#include <pthread.h>    
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ipc.h>
#include "tcpiplib.h"

int sock;
char already_an_shm;
unsigned char rank;
pthread_t tid;
void* shm;
sem_t send_lock;
sem_t shm_lock;
sem_t other_lock;
sem_t sync_lock;

int init_server(int portno){
    int sockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char clientAddr[CLADDR_LEN];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
       printf("ERROR opening socket: %s\n", strerror(errno));
       return -1;
    } 
    int on = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(ret){
        printf("ERROR enabling address reuse: %s", strerror(errno));
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
             printf("ERROR on binding: %s\n", strerror(errno));
             return -1;
    } 
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    sock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(sock<0){
        printf("ERROR on accept: %s", strerror(errno));
        return -1;
    }
    inet_ntop(AF_INET, &(cli_addr.sin_addr), clientAddr, CLADDR_LEN);
//    printf("Connection accepted from %s...\n", clientAddr);


    rank = 0;
    already_an_shm = 0;
    sem_init(&send_lock, 0, 1);
    return sock;
}

int init_client(char* serveraddr, int portno){
    struct sockaddr_in serv_addr; 
    struct hostent *server; 
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        printf("ERROR opening socket: %s\n", strerror(errno)); 
        return -1;
    } 
    server = gethostbyname(serveraddr); 
    if (server == NULL) { 
        printf("ERROR, no such host\n"); 
        return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); 
    serv_addr.sin_port = htons(portno); 
    if (connect(sock,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("ERROR connecting"); 
        return -1;
    } 
    else{
 //       printf("connection established with %s \n", serveraddr);
    }


    rank = 1;
    already_an_shm = 0;
    sem_init(&send_lock, 0, 1);
    return sock;
}


int close_connection(){
    if(already_an_shm){
        destroy_sm();
    }
    sem_destroy(&send_lock);
    return close(sock);
}

int transmit_file(FILE* in){
    //char buffer[BUF_SIZE];
    printf("calculating the size of the file\n");
    fseek(in, 0L, SEEK_END);
    int32_t size = ftell(in);
    rewind(in);
    printf("the size of the file is %d bytes\n", size);
    sem_wait(&send_lock);
    int n = write(sock, &size, sizeof(int32_t));
    if (n < 0){
        printf("error writing to socket\n");
        return 1;
    }
    char* buffer = malloc(size);
    if(buffer == NULL){
        printf("failed to malloc memory\n");
    }
    fread(buffer, size, 1, in);
    n = write(sock, buffer, size); 
    if (n < 0){ 
        printf("ERROR writing to socket: %s\n", strerror(errno)); 
        return 1;
    }
    bzero(buffer, BUF_SIZE);
    free(buffer);
    sem_post(&send_lock);
    return 0;
}

int receive_file(char* out){
    int ret;
    FILE* output = fopen(out,"wb");
    if(output == NULL){
        printf("ERROR: cannot open the output file\n");
        return 1;
    }
    int32_t size;
    ret = read(sock, &size, sizeof(int32_t)); 
    if(ret <= 0){
        printf("error receiving size of the file: %s\n", strerror(errno));
        return 1;
    }
    printf("size of the file is %d bytes\n", size);
    char* buffer = malloc(size);
    if(buffer == NULL){
        printf("Failed to alloc memory\n");
        return 1;
    }
    memset(buffer, 0, size);
    ret = read(sock, buffer, size);
    if (ret < 0) {
        printf("Error receiving data!\n");
        return 1;
    } 
    fwrite(buffer, size, 1, output);
    free(buffer);
    fclose(output);
    return 0;
}

int transmit_buffer_nolock(void* data, int32_t size) {
    char* data_p = data;
    //printf("transmitting size of %d\n", size);
    int n = write(sock, &size, sizeof(int32_t));
    if (n < 0){
        printf("error writing to socket\n");
        return 1;
    }
    //printf("starting to send buffer of size %d\n", size);
    
    n = write(sock, data, size); 
    if (n < 0){ 
        printf("ERROR writing to socket\n"); 
        return 1;
    }
    return 0;
}

int transmit_buffer(void* data, int32_t size){
    sem_wait(&send_lock);
    if(transmit_buffer_nolock(data, size)){
        return 1;
    }
    sem_post(&send_lock);
}

void* receive_buffer() {//try to see if databuffer can be force to be freed
    int ret;
    int32_t size = 0;
    ret = read(sock, &size, sizeof(int32_t)); 
    if(ret < 0){
        printf("error receiving size of the buffer: %s\n", strerror(errno));
        return NULL;
    }
    char* databuffer = malloc(size);
    if(databuffer == NULL){
        printf("Failed to alloc memory\n");
        return NULL;
    }
    memset(databuffer, 0, size); 
    ret = read(sock, databuffer, size);
    if (ret < 0) {
        printf("Error receiving data: %s\n", strerror(errno));
        return NULL;
    } 

    return (void*)databuffer;
}

int transmit_int32(int32_t num){
    int n = write(sock, &num, sizeof(int32_t));
    if (n < 0){
        printf("Error writing to socket: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

int transmit_char(char num){
    int n = write(sock, &num, sizeof(char));
    if (n < 0){
        printf("Error writing to socket: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

void *listener (void *arg){
    int ret;
    char sync;
    while(1){
        ret = read(sock, &sync, sizeof(char)); 
        if(ret <= 0){
            printf("error receiving sync, the other user has probably terminated shared mememory\n");
            break;
        }
        if(sync == 0){//write
            int32_t start;
            ret = read(sock, &start, sizeof(int32_t)); 
            if(ret <= 0){
                printf("error receiving write start position: %s\n", strerror(errno));
                break;
            }
            int32_t size = 0;
            ret = read(sock, &size, sizeof(int32_t)); 
            if(ret < 0){
                printf("error receiving size of the buffer: %s\n", strerror(errno));
                break;
            }
            //printf("size of the buffer is %d bytes\n", size);
            int i;
            char* shm_c = (char*)shm;

            ret = read(sock, shm_c + start, size);
            if (ret < 0) {
                printf("Error receiving data: %s\n", strerror(errno));
                return NULL;
            } 
            sem_post(&shm_lock);
        }
        else if(sync == 1){//sync
            sem_post(&sync_lock);
        }
        else if(sync == 2){//getOtherLock
            if(sem_trywait(&shm_lock)== -1){//didn't get the lock
                sem_wait(&send_lock);
                if(transmit_char((char)6)){
                    printf("failed to transmit: %s\n", strerror(errno));
                    break; 
                }
                sem_post(&send_lock);
            }
            else{
            sem_wait(&send_lock);
            if(transmit_char((char)3)){
                printf("failed to transmit: %s\n", strerror(errno));
                break; 
            }
            sem_post(&send_lock);
            }
        }
        else if(sync == 3){//gotOtherLock
            sem_post(&other_lock);
        } 
        else if(sync == 4){//releaseOtherLock
            sem_post(&shm_lock);
        }
        else if(sync == 5){//resize
            int new_size;
            ret = read(sock, &new_size, sizeof(int32_t)); 
            if(ret < 0){
                printf("error receiving new_size: %s\n", strerror(errno));
                break;
            }
            void* old_shm = shm;
            shm = realloc(old_shm, new_size);
            if(shm == NULL){
                printf("Failed to resize shared memory\n");
                break;
            }
            sem_post(&shm_lock);
        }
        else if(sync == 6){//request other lock again
            sem_wait(&send_lock);
            if(transmit_char((char)2)){
                printf("error sending data \n");
                break;
            }
            sem_post(&send_lock);
        }
        else{
            printf("something went wrong, out of sync\n");
        }
    }
    while(1);
    return NULL;
}

int init_sm(void* data, int32_t size){
    if(already_an_shm){
        printf("ERROR: There is already an shared memory, only one can be created\n");
        return 1;
    }
    already_an_shm = 1;
    if(transmit_buffer(data, size)){
        printf("Failed to send data\n");
        return 1;
    }
    shm = malloc(size);
    if(shm == NULL){
        printf("Failed to alloc memory\n");
        return 1;
    }
    char* shm_c = shm;
    char* data_c = data;
    int i;
    for(i = 0; i < size; i++){
       shm_c[i]=data_c[i];
    }
    if(pthread_create(&tid, NULL, listener, NULL)){
        printf("error creating the listening thread\n");
        return 1;
    }
    sem_init(&shm_lock, 0, 1);
    sem_init(&other_lock, 0, 0);
    sem_init(&sync_lock, 0, 0);
    return 0;
}

int accept_sm(){
    if(already_an_shm){
        printf("ERROR: There is already an shared memory, only one can be created\n");
        return 1;
    }
    shm = receive_buffer();
    if(shm == NULL){
        printf("Error receiving data\n");
        return 1;
    }
    already_an_shm = 1;
    if(pthread_create(&tid, NULL, listener, NULL)){
        printf("error creating the listening thread\n");
        return 1;
    }
    sem_init(&shm_lock, 0, 1);
    sem_init(&other_lock, 0, 0);
    sem_init(&sync_lock, 0, 0);
    return 0;
}

int destroy_sm(){
    if(!already_an_shm){
        printf("ERROR: there are no shared memory in use\n");
        return 1;
    }
    already_an_shm = 0;
    sem_destroy(&shm_lock);
    sem_destroy(&other_lock);
    sem_destroy(&sync_lock);
    pthread_cancel(tid);
    free(shm);
    return 0;
}

void read_sm(void* ptr, int start, int size){
    sem_wait(&shm_lock);
    char* shm_c = (char*) shm;
    int i;
    char* ptr_c = ptr;
    for(i = 0; i < size; i++){
        ptr_c[i] = shm_c[start + i];
    } 
    sem_post(&shm_lock);
}

int getOtherLock(){
    sem_wait(&send_lock);
    if(transmit_char((char)2)){
        printf("Failed to send data: %s\n", strerror(errno));
        return 1;
    }
    sem_post(&send_lock);
    sem_wait(&other_lock);
    return 0;
}

int releaseOtherLock(){
    sem_wait(&send_lock);
    if(transmit_char((char)4)){
        printf("Failed to send data: %s\n", strerror(errno));
        return 1;
    }
    sem_post(&send_lock);
    return 0;
}

int write_sm(void* data, int32_t start, int32_t size){
    if(rank == 0){
        sem_wait(&shm_lock);
        if(getOtherLock()){
            printf("Failed to get other lock\n");
            return 1;
        }
    }
    else{
        if(getOtherLock()){
            printf("Failed to get other lock\n");
            return 1;
        }
        sem_wait(&shm_lock);
    }
    sem_wait(&send_lock);
    if(transmit_char((char)0)){
        printf("Failed to send data\n");
        return 1;
    }
    if(transmit_int32(start)){
        printf("Failed to send data\n");
        return 1;
    }
    if(transmit_buffer_nolock(data, size)){
        printf("Failed to send data\n");
        return 1;
    }
    sem_post(&send_lock);
    int i;
    char* shm_c = (char*)shm;
    char* data_c = (char*)data;
    for(i = 0; i < size; i++){
        shm_c[start + i] = data_c[i];
    }
    if(rank == 0){
    //    releaseOtherLock();
        sem_post(&shm_lock);
    }
    else{
        sem_post(&shm_lock);
     //   if(releaseOtherLock()){
     //       printf("Failed to get other lock\n");
     //       return 1;
     //   }
    }
    return 0;
}
//need to get both locks
int resize_sm(int new_size){
    if(rank == 0){
        sem_wait(&shm_lock);
        if(getOtherLock()){
            printf("Failed to get other lock\n");
            return 1;
        }
    }
    else{
        if(getOtherLock()){
            printf("Failed to get other lock\n");
            return 1;
        }
        sem_wait(&shm_lock);
    }
    void* old_shm = shm;
    shm = realloc(old_shm, new_size);
    if(shm == NULL){
        printf("realloc failed\n");
        return 1;
    }
    sem_wait(&send_lock);
    if(transmit_char((char)5)){
        printf("Failed to send data: %s\n", strerror(errno));
        return 1;
    }
    if(transmit_int32(new_size)){
        printf("Failed to send data: %s\n", strerror(errno));
        return 1;
    }
    sem_post(&send_lock);

    if(rank == 0){
        //if(releaseOtherLock()){
        //    printf("Failed to release other lock\n");
        //    return 1;
        //}
        sem_post(&shm_lock);
    }
    else{
        sem_post(&shm_lock);
        //if(releaseOtherLock()){
        //    printf("Failed to release other lock\n");
        //    return 1;
        //}
    }
    return 0;
}

int sys_sync(){
    sem_wait(&send_lock);
    if(transmit_char((char)1)){
        return 1;
    }
    sem_post(&send_lock);
    sem_wait(&sync_lock);
    return 0;
}
