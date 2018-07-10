#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv){
    FILE* in = fopen("test", "rb");
    if(in == NULL){
        printf("unable to open file");
        exit(1);
    }
    fseek(in, 0L, SEEK_END);
    int size = ftell(in);
    printf("this file is %d bytes big", size);
    fclose(in);
}
