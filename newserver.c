#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"
#include <string.h>
int main(int argc, char** argv){
    int sockfd = init_server(1234);
    //transmit_file(in, sockfd);
    char* str = "did i do it?\n\0";    
    transmit_buffer((void*)str, strlen(str), sockfd);
    transmit_buffer((void*)str, strlen(str), sockfd);
    int int_array[5];
    int_array[0] = 5;
    int_array[1] = 4;
    int_array[2] = 3;
    int_array[3] = 2;
    int_array[4] = 1;
    transmit_buffer((void*)int_array, 5*sizeof(int), sockfd);
    close_connection(sockfd);
}
