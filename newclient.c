#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"
#include <string.h>
#include <unistd.h>
int main(int argc, char** argv){
    int sockfd = init_client("192.168.1.151", 1234);
    //if(init_client("127.0.0.1", 1234) == -1){
    //    printf("something went wrong, check your ip address\n");
    //    return -1;
    //}
    //char* buffer = (char*)receive_buffer();
    //printf("***************************\n");
    //printf("%s", buffer);
    //printf("***************************\n");
    //buffer = (char*)receive_buffer();
    //printf("***************************\n");
    //printf("%s", buffer);
    //printf("***************************\n");
    //int* ia = (int*)receive_buffer();
    //printf("received: %d, %d, %d, %d, %d\n", ia[0], ia[1], ia[2], ia[3], ia[4]);
    //free(ia);
    printf("***************************\n");
    printf("shared mem stuff\n");
    printf("***************************\n");
    accept_sm();
    int* ib = malloc(5*sizeof(int));
    read_sm(ib, 0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ib[0], ib[1], ib[2], ib[3], ib[4]);
    int* ic = malloc(3*sizeof(int));
    ic[0] = 11;
    ic[1] = 12;
    ic[2] = 13;
    printf("im here\n");
    write_sm(ic, 1*sizeof(int), 3*sizeof(int));
    memset(ic, 0, 5*sizeof(int));
    printf("im here\n");
    write_sm(ic, 0*sizeof(int), 5*sizeof(int));
    memset(ic, 0, 5*sizeof(int));
    write_sm(ic, 2*sizeof(int), 3*sizeof(int));

    printf("im here\n");
    sys_sync();
    read_sm(ib, 0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ib[0], ib[1], ib[2], ib[3], ib[4]);
    sys_sync();
    read_sm(ib, 0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ib[0], ib[1], ib[2], ib[3], ib[4]);
    sys_sync();
    free(ib);
    destroy_sm();
    close_connection();
}
