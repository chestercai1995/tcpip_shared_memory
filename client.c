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

#define BUF_SIZE 1
#define CLADDR_LEN 100
#define PORT 1234

char buffer[BUF_SIZE]; 
int n;

void error(char *msg) {  
    perror(msg); 
    exit(1);
} 

void *receive(void * socket){
    int sockfd, ret;
    sockfd =*((int*)socket);
    bzero(buffer,BUF_SIZE); 
    FILE* output = fopen("output_client.jpg", "wb");
    if(output == NULL){
        error("ERROR: cannot open the output file\n");
    }

    printf("starting to receive img and saving it to output.img\n");
    while(1){
        n = read(sockfd,buffer,BUF_SIZE); 
        if (n < 0){ 
            error("ERROR reading from socket"); 
            break;
        }
        if (n == 0) {
            printf("closing connection\n");
            break;
        }
        if(buffer[0]=='e' & buffer[1]=='x' & buffer[2]=='i' & buffer[3]=='t'){
            printf("the other user is off line");
            pthread_exit(NULL);
        }
       // char check = 1;//make sure user name provided on both side are the same
       // for(int i = 1; i < strlen(user) + 1; i++){
       //     if(buffer[i] == user[i - 1])
       //     check = 0;
       // }
       // if(check==1)
       fwrite(buffer, BUF_SIZE, 1, output);
       //     printf("%s\n",buffer); 
    }
    fclose(output);
    close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
         printf("usage %s hostname port\n", argv[0]);
         exit(0);
    }

    int sockfd, portno; 
    struct sockaddr_in serv_addr; 
    struct hostent *server; 
    pthread_t rThread;
    int ret;
    /*printf("provide user name:");
    bzero(user, BUF_SIZE); 
    fgets(user, BUF_SIZE - 1, stdin); 
    for(int i = 0; i < BUF_SIZE; i++){
        if (user[i] == 10){
            user[i] = 0;
        }
    }*/

    printf("Waiting for connection..\n");
    if (argc == 2) {
        printf("not port number provided, using default port %d", PORT);
        portno = 1234;
    }
    else{
        portno = atoi(argv[2]); 
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket"); 
    server = gethostbyname(argv[1]); 
    if (server == NULL) { 
        fprintf(stderr,"ERROR, no such host\n"); 
        exit(0); 
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
               (char *)&serv_addr.sin_addr.s_addr, 
               server->h_length); 
    serv_addr.sin_port = htons(portno); 
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting"); 
    else{
        printf("connection established with %s \n", argv[1]);
    }
    if (ret = pthread_create(&rThread, NULL, receive, (void *)(&sockfd))) {
        printf("ERROR: Return Code from pthread_create() is %d\n", ret);
        error("ERROR creating thread");
    }
    FILE* img = fopen("test.txt\0", "rb");
    if(img == NULL){
        error("ERROR: Cannot open image");
    }
    printf("calculating the size of the image\n");
    fseek(img, 0L, SEEK_END);
    int32_t size = ftell(img);
    rewind(img);
    printf("the size of the image is %d bytes\n", size);
    n = write(sockfd, &size, 4);
    if (n < 0){
        error("error writing to socket\n");
    }
    printf("staring to send img \n");
    while(fread(buffer, BUF_SIZE, 1, img)){
        n = write(sockfd, buffer, BUF_SIZE); 
        if (n < 0){ 
            error("ERROR writing to socket\n"); 
        }
        bzero(buffer, BUF_SIZE);
    }
    /*buffer[0] = 'e';
    buffer[1] = 'x';
    buffer[2] = 'i';
    buffer[3] = 't';
    buffer[4] = '\0';
    n = write(sockfd, buffer, BUF_SIZE); 
    if (n < 0) 
        error("ERROR writing to socket"); 
    */
    printf("finished sending img\n");
    fclose(img);
//    pthread_join(rThread, NULL);
    exit(0);
}
