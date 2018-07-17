#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"
#include <string.h>
int main(int argc, char** argv){
    int sockfd = init_client("192.168.1.151", 1234);
    char* buffer = (char*)receive_buffer(sockfd);
    printf("***************************\n");
    printf("%s", buffer);
    printf("***************************\n");
    buffer = (char*)receive_buffer(sockfd);
    printf("***************************\n");
    printf("%s", buffer);
    printf("***************************\n");
    int* ia = (int*)receive_buffer(sockfd);
    printf("received: %d, %d, %d, %d, %d", ia[0], ia[1], ia[2], ia[3], ia[4]);
    close_connection(sockfd);
}
