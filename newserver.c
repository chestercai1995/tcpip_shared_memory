#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"
#include <string.h>
#include <unistd.h>
int main(int argc, char** argv){
    int sockfd = init_server(1234);
    //transmit_file(in, sockfd);
    char* str = "did i do it?\n\0";    
    printf("transmitting string: \n********************\n%s********************\n", str);
    transmit_buffer((void*)str, 14);
    char* str1 = "I did it!\n\0";
    printf("transmitting string: \n********************\n%s********************\n", str1);
    transmit_buffer((void*)str1, 11);
    int int_array[5];
    int_array[0] = 5;
    int_array[1] = 4;
    int_array[2] = 3;
    int_array[3] = 2;
    int_array[4] = 1;
    transmit_buffer((void*)int_array, 5*sizeof(int));
    close_connection(sockfd);
}
