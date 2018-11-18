#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#define HALT 0
#define LOAD 1
#define PUSH 2
#define POP 3
#define JMP 4
#define JMPIF 5
#define GET 6
#define PUT 7

#define ADD 8
#define SUB 9
#define MUL 10
#define DIV 11
#define MOD 12
#define OUT 13

#define EQ 14
#define LESS 15
#define LOR 16
#define LAND 17
#define NOT 18
#define TERN 19

typedef unsigned char u8;
typedef char i8;
typedef unsigned int u32;
typedef int i32;

typedef struct Binary {
    i8 type;
    void* lhs;
    void* rhs;
} Binary;
typedef struct Identifier {
    i8 type;
    u32 length;
    i8* data;
} Identifier;
typedef struct Number {
    i8 type;
    i32 value;
} Number;
typedef struct Unary {
    i8 type;
    void* expr;
} Unary;

typedef union Word {
    i32 integer;
    u8 bytes[sizeof(i32)];
} Word;

void* parse(i8*, i8*, void*, void*);
void assemble(u8* code, u8* codeEnd, void* tree);
void printCode(u8*);
void execute(u8*);

#endif // PARSER_H_INCLUDED
