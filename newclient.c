#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"

int main(int argc, char** argv){
    int sockfd = init_client("192.168.1.151", 1234);
    char buffer[16];
    receive_buffer((void*)buffer, sockfd);
    printf("%s", buffer);
    close_connection(sockfd);
}
