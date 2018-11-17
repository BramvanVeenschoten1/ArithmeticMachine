#include <stdio.h>
#include <stdlib.h>
#include "header.h"

int main()
{
    u8 code[1024];
    for(int i = 0; i < 1024; i++){
        code[i] = 2;
    }
    {
        char source[4096];

        FILE* f = fopen("main.yaspl", "r");
        if(!f){
            printf("Couldn't open file\n");
            return 0xbabeface;
        }

        int length = 0;
        for(int c = fgetc(f); c != -1 && length < 4096; c = fgetc(f)){
            source[length] = c;
            length++;
        }
        fclose(f);

        char heap[4096];

        void* tree = parse(source, source + length, heap, heap + 4096);
        assemble(code, code + 1024, tree);
    }
    execute(code);
    return 0;
}
