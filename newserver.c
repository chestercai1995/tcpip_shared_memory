#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"

int main(int argc, char** argv){
    int sockfd = init_server(1234);
    FILE* in =fopen("test.txt", "rb");
    if(in == NULL) {
        printf("cannot open file\n");
        exit(1);
    }
    transmit(in, sockfd);
    
    close_connection(sockfd);
}
