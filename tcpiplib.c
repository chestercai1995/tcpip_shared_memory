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
#include "tcpiplib.h"

int init_server(int portno){
    int sockfd, newsockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    //int n;
    char clientAddr[CLADDR_LEN];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
       printf("ERROR opening socket: %s\n", strerror(errno));
       return -1;
    } 
    printf("Socket created...\n");
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
    printf("Binding done...\n");
    printf("Waiting for a connection...\n");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd<0){
        printf("ERROR on accept: %s", strerror(errno));
        return -1;
    }
    inet_ntop(AF_INET, &(cli_addr.sin_addr), clientAddr, CLADDR_LEN);
    printf("Connection accepted from %s...\n", clientAddr);
    return newsockfd;
}

int init_client(char* serveraddr, int portno){
    int sockfd; 
    struct sockaddr_in serv_addr; 
    struct hostent *server; 
    printf("Waiting for connection..\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
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
    if (connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("ERROR connecting"); 
        return -1;
    } 
    else{
        printf("connection established with %s \n", serveraddr);
    }
    return sockfd;
}

int close_connection(int sockfd){
    return close(sockfd);
}

int transmit_file(FILE* in, int sockfd){
    char buffer[BUF_SIZE];
    printf("calculating the size of the file\n");
    fseek(in, 0L, SEEK_END);
    int32_t size = ftell(in);
    rewind(in);
    printf("the size of the file is %d bytes\n", size);
    int n = write(sockfd, &size, sizeof(int32_t));
    if (n < 0){
        printf("error writing to socket\n");
        return 1;
    }
    printf("staring to send file\n");
    while(fread(buffer, BUF_SIZE, 1, in)){
        n = write(sockfd, buffer, BUF_SIZE); 
        if (n < 0){ 
            printf("ERROR writing to socket\n"); 
            return 1;
        }
        bzero(buffer, BUF_SIZE);
    }
    printf("finished sending file\n");
    return 0;
}

int receive_file(char* out, int sockfd){
    int ret;
    char buffer[BUF_SIZE];
    memset(buffer, 0, BUF_SIZE);
    FILE* output = fopen(out,"wb");
    if(output == NULL){
        printf("ERROR: cannot open the output file\n");
        return 1;
    }
    int32_t size;
    ret = read(sockfd, &size, sizeof(int32_t)); 
    if(ret <= 0){
        printf("error receiving size of the file\n");
        return 1;
    }
    printf("size of the file is %d bytes\n", size);
    printf("starting to receive file\n");
    while ((ret = read(sockfd, buffer, BUF_SIZE)) > 0) {
        if (ret < 0) {
            printf("Error receiving data!\n");
            return 1;
        } 
       fwrite(buffer, BUF_SIZE, 1, output);
       bzero(buffer, BUF_SIZE);
    }
    printf("Closing connection\n");
    fclose(output);
    return 0;
}

int transmit_buffer(void* data, int32_t size, int sockfd) {
    char buffer[BUF_SIZE];
    char* data_p = data;
    int n = write(sockfd, &size, sizeof(int32_t));
    if (n < 0){
        printf("error writing to socket\n");
        return 1;
    }
    printf("staring to send buffer\n");
    while(size != 0){
        buffer[0] = data_p[size-1]; 
        n = write(sockfd, buffer, BUF_SIZE); 
        if (n < 0){ 
            printf("ERROR writing to socket\n"); 
            return 1;
        }
        bzero(buffer, BUF_SIZE);
        size--;
    }
    printf("finished sending buffer\n");
    return 0;
}

void* receive_buffer(int sockfd) {
    int ret;
    char buffer[BUF_SIZE];
    memset(buffer, 0, BUF_SIZE);
    int32_t size;
    ret = read(sockfd, &size, sizeof(int32_t)); 
    if(ret <= 0){
        printf("error receiving size of the buffer\n");
        return NULL;
    }
    printf("size of the buffer is %d bytes\n", size);

    char* data_p = malloc(size);
    memset(data_p, 0, size); 
    printf("starting to receive buffer\n");
    while ((ret = read(sockfd, buffer, BUF_SIZE)) > 0) {
        if (ret < 0) {
            printf("Error receiving data!\n");
            return NULL;
        } 
        data_p[size - 1] = buffer[0]; 
        bzero(buffer, BUF_SIZE);
        size--;
    }
    printf("successfully received %d bytes of data\n", size);
    return (void*)data_p;
}
