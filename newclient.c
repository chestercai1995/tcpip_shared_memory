#include <stdio.h>
#include <stdlib.h>
#include "tcpiplib.h"

int main(int argc, char** argv){
    int sockfd = init_client("192.168.1.151", 1234);
    receive_file("output.txt", sockfd);
    close_connection(sockfd);
}
