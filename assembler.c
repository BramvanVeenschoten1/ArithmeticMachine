#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define TABLE_SIZE 256

// symbol table
typedef struct Entry{
    char* data;
    u32 length;
    u32 depth;
} Entry;
static u8 toHash(char* data, u32 length){
    u8 result = 0;
    for(int i = 0; i < length; i++)
        result += data[i];
    return result;
}
static i8 equals(char* a, char* b, u32 length){
    for(int i = 0; i < length; i++)
        if(a[i] != b[i])
            return 0;
    return 1;
}
static void clearTable(Entry* table){
    for(int i = 0; i < TABLE_SIZE; i++)
        table[i].data = NULL;
}
static u8 set(Entry* table, i8* data, u32 length, u32 depth){
    u8 hash = toHash(data, length);
    for(u32 i = 0; i < TABLE_SIZE; i++){
        if(table[hash].data == NULL){
            table[hash] = (Entry){data, length, depth};
            return 0;
        }
        if(length == table[hash].length && equals(data, table[hash].data, length)){
            table[hash] = (Entry){data, length, depth};
            return 1;
        }
        hash++;
    }
    printf("Symbol table full!\n");
    exit(0xf00dcafe);
}
static u32* get(Entry* table, char* data, u32 length){
    u8 hash = toHash(data, length);
    for(u32 i = 0; i < TABLE_SIZE; i++){
        if(table[hash].data == NULL)
            return NULL;
        if(table[hash].length == length && equals(data, table[hash].data, length))
            return &table[hash].depth;
        hash++;
    }
    return NULL;
}
static u8 del(Entry* table, char* data, u32 length){
    u8 hash = toHash(data, length);
    for(u32 i = 0; i < TABLE_SIZE; i++){
        if(table[hash].data == NULL)
            return 0;
        if(table[hash].length == length && equals(data, table[hash].data, length)){
            table[hash].data = NULL;
            return 1;
        }
        hash++;
    }
    return 0;
}

// assembler
typedef struct Assembler {
    u8* code;
    u8* codeEnd;
    u32 stackCounter;
    Entry table[TABLE_SIZE];
} Assembler;
static void assembleNode(Assembler*,void*);
static void assembleBinary(Assembler* a, Binary* b){
    assembleNode(a, b->lhs);
    *a->code = PUSH; a->code++;
    a->stackCounter++;
    assembleNode(a, b->rhs);
    switch(b->type){
        case '+': *a->code = ADD;  a->code++; break;
        case '-': *a->code = SUB;  a->code++; break;
        case '*': *a->code = MUL;  a->code++; break;
        case '/': *a->code = DIV;  a->code++; break;
        case '%': *a->code = MOD;  a->code++; break;
        case '<': *a->code = LESS; a->code++; break;
        case 'E': *a->code = EQ;   a->code++; break;
        case 'O': *a->code = LOR;  a->code++; break;
        case 'A': *a->code = LAND; a->code++; break;
    }
    *a->code = POP; a->code++;
    a->stackCounter--;
}
static void assembleAssign(Assembler* a, Binary* b){
    Identifier* id = b->lhs;
    u32* depth = get(a->table, id->data, id->length);
    if(!depth){
        printf("Unknown Identifier: ");
        for(int i = 0; i < id->length; i++)
            putchar(id->data[i]);
        putchar('\n');
    }
    a->stackCounter++;
    Word w;
    w.integer = a->stackCounter - *depth;
    *a->code = LOAD;       a->code++;
    *a->code = w.bytes[0]; a->code++;
    *a->code = w.bytes[1]; a->code++;
    *a->code = w.bytes[2]; a->code++;
    *a->code = w.bytes[3]; a->code++;
    *a->code = PUSH;       a->code++;

    assembleNode(a, b->rhs);

    *a->code = PUT; a->code++;

    *a->code = POP; a->code++;
    a->stackCounter--;
}
static void assembleNumber(Assembler* a, Number* n){
    Word w;
    w.integer = n->value;
    *a->code = LOAD;       a->code++;
    *a->code = w.bytes[0]; a->code++;
    *a->code = w.bytes[1]; a->code++;
    *a->code = w.bytes[2]; a->code++;
    *a->code = w.bytes[3]; a->code++;
}
static void assembleDecl(Assembler* a, Binary* d){
    Identifier* id = d->lhs;
    set(a->table, id->data, id->length, a->stackCounter);

    assembleNode(a, d->rhs);
    *a->code = PUSH; a->code++;
    a->stackCounter++;
}
static void assembleIdentifier(Assembler* a, Identifier* id){
    u32* depth = get(a->table, id->data, id->length);
    if(!depth){
        printf("Unknown Identifier: ");
        for(int i = 0; i < id->length; i++)
            putchar(id->data[i]);
        putchar('\n');
    }
    Word w;
    w.integer = a->stackCounter - *depth;
    *a->code = LOAD;         a->code++;
    *a->code = w.bytes[0];   a->code++;
    *a->code = w.bytes[1];   a->code++;
    *a->code = w.bytes[2];   a->code++;
    *a->code = w.bytes[3];   a->code++;
    *a->code = GET;          a->code++;
}
static void assembleUnary(Assembler* a, Unary* q){
    assembleNode(a, q->expr);
    if(q->type == 'q'){
        *a->code = OUT; a->code++;
    }
    else {
        *a->code = NOT; a->code++;
    }

}
static void assembleNode(Assembler* a, void* node){
    if(!node)
        return;
    i8* type = (i8*)node;
    switch(*type){
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '<':
        case 'E':
        case 'O':
        case 'A': return assembleBinary(a, node);
        case '=': return assembleAssign(a, node);
        case 'i': return assembleIdentifier(a, node);
        case 'd': return assembleDecl(a, node);
        case 'n': return assembleNumber(a, node);
        case '!':
        case 'q': return assembleUnary(a, node);
        case 's': return assembleNode(a, ((Binary*)node)->lhs), assembleNode(a, ((Binary*)node)->rhs);
    }
}

void assemble(u8* code, u8* codeEnd, void* tree){
    Assembler a;
    a.code = code;
    a.codeEnd = codeEnd;
    a.stackCounter = 0;
    clearTable(a.table);
    assembleNode(&a, tree);
}




