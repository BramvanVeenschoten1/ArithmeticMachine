#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define CODE_SIZE 4096
#define SOURCE_SIZE 4096 * 2
#define HEAP_SIZE 1024*1024

int main()
{
    u8 code[CODE_SIZE];
    {
        char source[SOURCE_SIZE];

        FILE* f = fopen("main.yaspl", "r");
        if(!f){
            printf("Couldn't open file\n");
            return 0xbabeface;
        }

        int length = 0;
        for(int c = fgetc(f); c != -1 && length < SOURCE_SIZE; c = fgetc(f)){
            source[length] = c;
            length++;
        }
        fclose(f);

        char heap[HEAP_SIZE];

        void* tree = parse(source, source + length, heap, heap + HEAP_SIZE);

        assemble(code, code + CODE_SIZE, tree);
        printCode(code);
    }
    execute(code);
    printf("%d\n", sizeof(void*));
}
