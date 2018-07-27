#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"
#include <string.h>
#include <unistd.h>
int main(int argc, char** argv){
    int sockfd = init_client("192.168.1.151", 1234);
    char* buffer = (char*)receive_buffer();
    printf("***************************\n");
    printf("%s", buffer);
    printf("***************************\n");
    buffer = (char*)receive_buffer();
    printf("***************************\n");
    printf("%s", buffer);
    printf("***************************\n");
    int* ia = (int*)receive_buffer();
    printf("received: %d, %d, %d, %d, %d\n", ia[0], ia[1], ia[2], ia[3], ia[4]);
    free(ia);
    printf("***************************\n");
    printf("shared mem stuff\n");
    printf("***************************\n");
    accept_sm();
    int* ib = (int*)read_sm(0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ib[0], ib[1], ib[2], ib[3], ib[4]);
    int* ic = malloc(3*sizeof(int));
    ic[0] = 11;
    ic[1] = 12;
    ic[2] = 13;
    write_sm(ic, 1*sizeof(int), 3*sizeof(int));
    memset(ic, 0, 5*sizeof(int));
    write_sm(ic, 0*sizeof(int), 5*sizeof(int));
    memset(ic, 0, 5*sizeof(int));
    write_sm(ic, 2*sizeof(int), 3*sizeof(int));

    sys_sync();
    ib = (int*)read_sm(0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ib[0], ib[1], ib[2], ib[3], ib[4]);
    sys_sync();
    ib = (int*)read_sm(0, sizeof(int) * 5);
    printf("read: %d, %d, %d, %d, %d\n", ib[0], ib[1], ib[2], ib[3], ib[4]);
    sys_sync();
    int i;
    int* ir=read_sm(0, 100*sizeof(int));
    printf("im reading: \n");
    for(i = 0; i< 100; i++){
        printf("at index %d I have %d\n", i, ir[i]);
    }
    destroy_sm();
    close_connection(sockfd);
}
