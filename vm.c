#include <stdio.h>
#include <stdlib.h>
#include "header.h"

void execute(u8* ops){
    i32 stack[1024];
    i32* ptr = stack;
    while(*ops){
        switch(*ops){
            case LOAD: {
                Word w;
                ops++; w.bytes[0] = *ops;
                ops++; w.bytes[1] = *ops;
                ops++; w.bytes[2] = *ops;
                ops++; w.bytes[3] = *ops;
                *ptr = w.integer;
                break;
            }
            case PUSH: if(ptr >= stack + 1024)
                { printf("Stack overflow!\n"); exit(0xdeadbeef);}
                      ptr++;                  break;
            case POP: if(ptr <= stack)
                {printf("Stack underflow!\n"); exit(0xdeadbeef);}
                      ptr--;                  break;
            case GET: *ptr = ptr[- *ptr];     break;
            case PUT: ptr[- ptr[-1]] = *ptr;  break;
            case JMP: ops += *ptr;            break;
            case OUT: printf("%d\n", *ptr);   break;
            case ADD: ptr[-1] += *ptr;        break;
            case SUB: ptr[-1] -= *ptr;        break;
            case MUL: ptr[-1] *= *ptr;        break;
            case DIV: ptr[-1] /= *ptr;        break;
            case MOD: ptr[-1] %= *ptr;        break;
        }
        ops++;
    }
}
