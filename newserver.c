#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"
#include <string.h>
int main(int argc, char** argv){
    int sockfd = init_server(1234);
    //transmit_file(in, sockfd);
    char* str = "did i do it?\n\0";    
    transmit_buffer((void*)str, strlen(str), sockfd);
    close_connection(sockfd);
}
