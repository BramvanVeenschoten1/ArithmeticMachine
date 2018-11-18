#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define STACK_SIZE 1024

void printCode(u8* ops){
    int i = 0;
    while(*ops){
        printf("%d:\t", i);
        i++;
        switch(*ops){
            case LOAD: {
                Word w;
                ops++; w.bytes[0] = *ops;
                ops++; w.bytes[1] = *ops;
                ops++; w.bytes[2] = *ops;
                ops++; w.bytes[3] = *ops;
                printf("LOAD %d\n", w.integer);
                break;
            }
            case PUSH: printf("PUSH\n");break;
            case POP:  printf("POP\n"); break;
            case GET:  printf("GET\n"); break;
            case PUT:  printf("PUT\n"); break;
            case JMP:  printf("JMP\n"); break;
            case OUT:  printf("OUT\n"); break;
            case ADD:  printf("ADD\n"); break;
            case SUB:  printf("SUB\n"); break;
            case MUL:  printf("MUL\n"); break;
            case DIV:  printf("DIV\n"); break;
            case MOD:  printf("MOD\n"); break;
            default:   printf("OTHER\n");break;
        }
        ops++;
    }
}

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
            case GET:       *ptr = ptr[- *ptr];     break;
            case PUT:       ptr[- ptr[-1]] = *ptr;  break;
            case JMP:       ops += *ptr;            break;
            case JMPIF:     if(ptr[-1])ops += *ptr; break;
            case OUT:       printf("%d\n", *ptr);   break;
            case ADD:       ptr[-1] += *ptr;        break;
            case SUB:       ptr[-1] -= *ptr;        break;
            case MUL:       ptr[-1] *= *ptr;        break;
            case DIV:       ptr[-1] /= *ptr;        break;
            case MOD:       ptr[-1] %= *ptr;        break;
            case EQ:      ptr[-1] = (ptr[-1] == *ptr); break;
            case LESS:    ptr[-1] = (ptr[-1] <  *ptr); break;
            case LOR:     ptr[-1] = (ptr[-1] || *ptr); break;
            case LAND:    ptr[-1] = (ptr[-1] && *ptr); break;
            case NOT:     *ptr = !*ptr;                break;
        }
        ops++;
    }
}
