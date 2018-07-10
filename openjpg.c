#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]){
    
    char* filename = "test.jpg\0";
    FILE* img = fopen(filename, "rb");
    FILE* output = fopen("output.jpg", "wb");
    if(img == NULL){
        printf("ERROR: Cannot open file\n");
        return 1;
    }
    if(output == NULL){
        printf("ERROR: cannot open output file");
        return 1;
    }
    unsigned char buffer[512];
    while(fread(buffer, 512, 1, img)){
        /*send*/
        fwrite(buffer, 512, 1, output);
    }
    fclose(img);
    fclose(output);
}
