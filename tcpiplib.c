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

void init_server(int portno){
    int sockfd, newsockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    //int n;
    char clientAddr[CLADDR_LEN];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket\n");
    printf("Socket created...\n");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) 
             error("ERROR on binding\n");
    printf("Binding done...\n");
    printf("Waiting for a connection...\n");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd<0){
        error("ERROR on accept");
    }
    inet_ntop(AF_INET, &(cli_addr.sin_addr), clientAddr, CLADDR_LEN);
    printf("Connection accepted from %s...\n", clientAddr);
}

void init_client(char** serv_addr, int portno){
    int sockfd; 
    struct sockaddr_in serv_addr; 
    struct hostent *server; 
    printf("Waiting for connection..\n");
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
}

void send(FILE* in){
    char buffer[BUF_SIZE];
    printf("calculating the size of the file\n");
    fseek(in, 0L, SEEK_END);
    int32_t size = ftell(img);
    rewind(in);
    printf("the size of the file is %d bytes\n", size);
    int n = write(sockfd, &size, 4);
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
    printf("finished sending img\n");
}

void receive(char** out, int sockfd){
    int ret;
    memset(buffer, 0, BUF_SIZE);
    FILE* output = fopen(out,"wb");
    if(output == NULL){
        error("ERROR: cannot open the output file\n");
    }
    int32_t size;
    ret = read(sockfd, &size, 4);
    if(ret <= 0){
        error("error receiving size of the file\n");
    }
    printf("size of the file is %d bytes\n", size);
    printf("starting to receive file\n");
    while ((ret = read(sockfd, buffer, BUF_SIZE)) > 0) {
       fwrite(buffer, BUF_SIZE, 1, output);
       bzero(buffer, BUF_SIZE);
    }
    if (ret < 0) 
        printf("Error receiving data!\n");
    else
        printf("Closing connection\n");
    fclose(output);
    //close(sockfd); TODO: someone needs to close this
}
void close_connection(){
}
