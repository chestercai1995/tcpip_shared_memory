#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h> 
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1
#define CLADDR_LEN 100
#define PORT 1234

char buffer[BUF_SIZE];
unsigned char r_buffer[BUF_SIZE];

void error(char *msg){
    perror(msg);
    exit(1);
}

void * receive(void* socket) {
    int sockfd, ret;
    sockfd =  (* ((int*)socket));
    
    memset(buffer, 0, BUF_SIZE);
    FILE* output = fopen("output.jpg", "wb");
    if(output == NULL){
        error("ERROR: cannot open the output file\n");
    }
    int size;
    ret = read(sockfd, buffer, BUF_SIZE);
    if(ret > 0){
        
    }
    printf("starting to receive img and saving it to output.img\n");
    while ((ret = read(sockfd, buffer, BUF_SIZE)) > 0) {
        if(buffer[0]=='e' & buffer[1]=='x' & buffer[2]=='i' & buffer[3]=='t'){
            printf("the client has cut the connection\n");
            pthread_exit(NULL);
        }
       fwrite(buffer, BUF_SIZE, 1, output);
       bzero(buffer, BUF_SIZE);
    }
    if (ret < 0) 
        printf("Error receiving data!\n");
    else
        printf("Closing connection\n");
    fclose(output);
    close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    pthread_t rThread;
    int ret;
    //printf("provide user name:");
    //bzero(user, BUF_SIZE); 
    //fgets(user, BUF_SIZE - 1, stdin); 
    //for(int i = 0; i < BUF_SIZE; i++){
    //    if (user[i]==10){
    //        user[i]=0;
    //    }
    //}
    char clientAddr[CLADDR_LEN];
    if (argc < 2) {
        printf("no port number provided, using default port number %d", PORT);
        portno = PORT;
    }
    else{
        portno = atoi(argv[1]);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    printf("Socket created...\n");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) 
             error("ERROR on binding");
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
    //bzero(buffer,256);
    if (ret = pthread_create(&rThread, NULL, receive, (void *) (&newsockfd))) {
        printf("ERROR: Return Code from pthread_create() is %d\n", ret);
        error("ERROR creating thread");
    }

//    FILE* img = fopen("test1.jpg\0", "rb");
//    if(img == NULL){
//        error("ERROR: Cannot open image");
//    }
//    printf("staring to send img \n");
//    while(fread(buffer, BUF_SIZE, 1, img)){
//        n = write(sockfd, buffer, BUF_SIZE /*strlen(buffer)*/); 
//        //if(buffer1[2+strlen(user)]=='e'&buffer1[2+strlen(user)+1]=='x'&buffer1[2+strlen(user)+2]=='i'&buffer1[2+strlen(user)+3]=='t'){
//        //    printf("Bye");
//        //    break;
//        //    //exit(0);
//        //}
//        if (n < 0) 
//            error("ERROR writing to socket"); 
//               
//        //return 0; 
//        bzero(buffer, BUF_SIZE);
//    }
//    buffer[0] = 'e';
//    buffer[1] = 'x';
//    buffer[2] = 'i';
//    buffer[3] = 't';
//    buffer[4] = '\0';
//    n = write(sockfd, buffer, BUF_SIZE /*strlen(buffer)*/); 
//    if (n < 0) 
//        error("ERROR writing to socket"); 
//    
//    printf("finished sending img\n");
//    fclose(img);
   // while(1){
   //       //printf("<you>");
   //        bzero(buffer, BUF_SIZE); 
   //        fgets(buffer, BUF_SIZE - 1,stdin); 
   //        buffer1[0]=60;
   //        for(int i=0;i<strlen(user);i++){
   //             buffer1[i+1]=user[i];
   //        }
   //        buffer1[strlen(user)+1]=62;
   //        for(int i=0;i<strlen(buffer);i++){
   //             buffer1[i+2+strlen(user)]=buffer[i];
   //        }
   //        n = write(newsockfd,buffer1,strlen(buffer1)); 
   //        if(buffer1[2+strlen(user)]=='e'&buffer1[2+strlen(user)+1]=='x'&buffer1[2+strlen(user)+2]=='i'&buffer1[2+strlen(user)+3]=='t'){
   //             printf("Bye");
   //             break;
   //             //exit(0);
   //        }
   //        if (n < 0) 
   //             error("ERROR writing to socket"); 
   //        bzero(buffer, BUF_SIZE);
   //        bzero(buffer1, BUF_SIZE);
   // }
    pthread_join(rThread, NULL);
    exit(0);
}
