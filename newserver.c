#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"
#include <string.h>
#include <unistd.h>
int main(int argc, char** argv){
    int sockfd = init_server(1234);
//    char* str = "did i do it?\n\0";    
//    printf("transmitting string: \n********************\n%s********************\n", str);
//    transmit_buffer((void*)str, 14);
//    char* str1 = "I did it!\n\0";
//    printf("transmitting string: \n********************\n%s********************\n", str1);
//    transmit_buffer((void*)str1, 11);
    int int_array[5];
    int_array[0] = 5;
    int_array[1] = 4;
    int_array[2] = 3;
    int_array[3] = 2;
    int_array[4] = 1;
//    transmit_buffer((void*)int_array, 5*sizeof(int));
    printf("***************************\n");
    printf("shared mem stuff\n");
    printf("***************************\n");
    init_sm(int_array, 5*sizeof(int));
    int* ic = (int*)malloc(5*sizeof(int));
    read_sm(ic, 0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ic[0], ic[1], ic[2], ic[3], ic[4]);
    int* id = malloc(4*sizeof(int));
    id[0]=101;
    id[1]=195;
    id[2]=995865;
    id[3]=23132;
    printf("im here\n");
    write_sm(id, 0, 4*sizeof(int));
    printf("im here\n");
    memset(ic, 0, 5*sizeof(int));
    write_sm(ic, 0*sizeof(int), 5*sizeof(int));
    memset(ic, 0, 5*sizeof(int));
    write_sm(ic, 2*sizeof(int), 3*sizeof(int));
    sys_sync();
    write_sm(id, 1*sizeof(int), 4*sizeof(int));
    free(id);
    sys_sync();
    int* ib = (int*)malloc(5*sizeof(int));
    read_sm(ib, 0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ib[0], ib[1], ib[2], ib[3], ib[4]);
    resize_sm(100*sizeof(int)); 
    int ie[100];
    memset(ie, 0, 100*sizeof(int));
    read_sm(ie, 0, 4*sizeof(int));
    int i;
    for(i = 0; i < 4;i++){
        printf("ie at index %d reads %d\n", i, ie[i]);
    }
    write_sm(ie, 0, 100*sizeof(int));
    sys_sync();
    printf("here\n");
    destroy_sm();
    close_connection(sockfd);
}
