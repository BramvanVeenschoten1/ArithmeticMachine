#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#define HALT 0
#define LOAD 1
#define PUSH 2
#define POP 3
#define JMP 4
#define GET 5
#define PUT 6
#define ADD 7
#define SUB 8
#define MUL 9
#define DIV 10
#define MOD 11
#define OUT 12

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
typedef struct Quote {
    i8 type;
    void* expr;
} Quote;

typedef union Word {
    i32 integer;
    u8 bytes[sizeof(i32)];
} Word;

void* parse(i8*, i8*, i8*, i8*);
void printTree(void*, int);
void assemble(u8* code, u8* codeEnd, void* tree);
void execute(u8*);

#endif // PARSER_H_INCLUDED
