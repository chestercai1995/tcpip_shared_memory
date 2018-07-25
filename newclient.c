#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"
#include <string.h>
#include <unistd.h>
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
    printf("received: %d, %d, %d, %d, %d\n", ia[0], ia[1], ia[2], ia[3], ia[4]);
    free(ia);
    accept_sm();
    int* ib = (int*)read_sm(0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ib[0], ib[1], ib[2], ib[3], ib[4]);
    sys_sync();
    destroy_sm();
    close_connection(sockfd);
}
