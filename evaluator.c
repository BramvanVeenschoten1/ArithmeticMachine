#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define TABLE_SIZE 256

typedef struct Entry{
    char* data;
    u32 length;
    u32 depth;
} Entry;
static u8 toHash(char* data, u32 length){
    u8 result;
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
static u8 set(Entry* table, Entry item){
    u8 hash = toHash(item.data, item.length);
    for(u32 i = 0; i < TABLE_SIZE; i++){
        if(table[hash].data == NULL){
            table[hash] = item;
            return 0;
        }
        if(item.length == table[hash].length && equals(item.data, table[hash].data, item.length)){
            table[hash] = item;
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
