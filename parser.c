#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

typedef struct Parser {
    i8* begin;
    i8* end;
    void* heap;
    void* heapEnd;
    i32 lineNo;
} Parser;

static void* parsePrimary(Parser* p);

static void* alloc(Parser* p, u32 size){
    if(p->heap + size >= p->heapEnd){
        printf("Heap too small!\n");
        exit(0xcafef00d);
    }
    void* tmp = p->heap;
    p->heap += size;
    return tmp;
}

static void skipWhite(Parser* p){
    while(1){
        if(p->begin >= p->end)
            return;
        i8* c = p->begin;
        if(*c == ' ' || *c == '\t' || *c == '\v' || *c == '\r')
            p->begin++;
        else if (*c == '\n'){
            p->lineNo++;
            p->begin++;
        }
        else return;
    }
}
static i8 parseChar(Parser* p, i8 c){
    if(p->begin >= p->end)
        return 0;
    if(*(p->begin) == c){
        if(c == '\n')
            p->lineNo++;
        p->begin++;
        return 1;
    }
    return 0;
}
static i8 parseRange(Parser* p, i8 l, i8 u){
    if(p->begin >= p->end)
        return 0;
    if(*(p->begin) >= l && *(p->begin) <= u){
        if(*(p->begin) == '\n')
            p->lineNo++;
        p->begin++;
        return 1;
    }
    return 0;
}
static i8 parseLiteral(Parser* p, i8* c){
    u32 length = strlen(c);
    if(p->begin + length >= p->end)
        return 0;
    i32 lines = 0;
    for(int i = 0; i < length; i++){
        if(p->begin[i] != c[i])
            return 0;
        if(p->begin[i] == '\n')
            lines++;
    }
    p->begin += length;
    p->lineNo += lines;
    return 1;
}

static void* parseNumber(Parser* p){
    i32 value = (i32)strtol(p->begin, &(p->begin), 10);
    Number* result = alloc(p, sizeof(Number));
    result->type = 'n';
    result->value = value;
    return result;
}
static void* parseIdentifier(Parser* p){
    char* begin = p->begin;
    u32 length = 0;
    while(parseRange(p, 'a', 'z') ||
          parseRange(p, 'A', 'Z') ||
          parseRange(p, '0', '9') ||
          parseChar(p, '_'))
        length++;
    Identifier* result = alloc(p, sizeof(Identifier));
    result->type = 'i';
    result->length = length;
    result->data = begin;
    return result;
}
static void* parseMul(Parser* p){
    Binary* result = parsePrimary(p);
    if(!result)
        return result;

    while(1){
        skipWhite(p);
        if(parseChar(p, '*')){
            void* rhs = parsePrimary(p);
            if(!rhs)
                return rhs;
            void* lhs = result;
            result = alloc(p, sizeof(Binary));
            result->type = '*';
            result->lhs = lhs;
            result->rhs = rhs;
        }
        else if(parseChar(p, '/')){
            void* rhs = parsePrimary(p);
            if(!rhs)
                return rhs;
            void* lhs = result;
            result = alloc(p, sizeof(Binary));
            result->type = '/';
            result->lhs = lhs;
            result->rhs = rhs;
        }
        else if(parseChar(p, '%')){
            void* rhs = parsePrimary(p);
            if(!rhs)
                return rhs;
            void* lhs = result;
            result = alloc(p, sizeof(Binary));
            result->type = '%';
            result->lhs = lhs;
            result->rhs = rhs;
        }
        else return result;
    }
}
static void* parseAdd(Parser* p){
    Binary* result = parseMul(p);
    if(!result)
        return result;

    while(1){
        skipWhite(p);
        if(parseChar(p, '+')){
            void* rhs = parseMul(p);
            if(!rhs)
                return rhs;
            void* lhs = result;
            result = alloc(p, sizeof(Binary));
            result->type = '+';
            result->lhs = lhs;
            result->rhs = rhs;
        }
        else if(parseChar(p, '-')){
            void* rhs = parseMul(p);
            if(!rhs)
                return rhs;
            void* lhs = result;
            result = alloc(p, sizeof(Binary));
            result->type = '-';
            result->lhs = lhs;
            result->rhs = rhs;
        }
        else return result;
    }
}
static void* parsePrimary(Parser* p){
    skipWhite(p);
    if(parseChar(p, '(')){
        void* result = parseAdd(p);
        if(!result)
            return result;
        skipWhite(p);
        if(parseChar(p, ')'))
            return result;
        printf("(line %d) Expected \')\' after expression.\n", p->lineNo);
        return NULL;
    }
    if(parseRange(p, '0', '9')){
        p->begin--;
        return parseNumber(p);
    }
    if(parseRange(p, 'a', 'z') || parseRange(p, 'A', 'Z') || parseChar(p, '_')){
        p->begin--;
        return parseIdentifier(p);
    }

    printf("(line %d) Unexpected character: \'%c\'.\n", p->lineNo, p->begin[0]);
    return NULL;
}

static void* parseAssign(Parser* p){
    void* id = parseIdentifier(p);
    if(!id)
        return id;
    skipWhite(p);
    if(!parseChar(p, '=')){
        printf("(line %d) Expected \'=\' after identifier.\n", p->lineNo);
        return NULL;
    }
    void* expr = parseAdd(p);
    if(!expr)
        return expr;
    Binary* assign = alloc(p, sizeof(Binary));
    assign->type = '=';
    assign->lhs = id;
    assign->rhs = expr;

    skipWhite(p);
    if(!parseChar(p, ';')){
        printf("(line %d) Expected \';\' after assignment\n", p->lineNo);
        return NULL;
    }
    return assign;
}
static void* parseDecl(Parser* p){
    skipWhite(p);
    void* id = parseIdentifier(p);
    if(!id)
        return id;

    Binary* decl = alloc(p, sizeof(Binary));
    decl->type = 'd';
    decl->lhs = id;

    skipWhite(p);
    if(parseChar(p, '=')){
        void* expr = parseAdd(p);
        if(!expr)
            return expr;
        decl->rhs = expr;
        skipWhite(p);
    }
    else
        decl->rhs = NULL;

    if(!parseChar(p, ';')){
        printf("(line %d) Expected \';\' after declaration\n", p->lineNo);
        return NULL;
    }
    return decl;
}
static void* parseStatement(Parser* p){
    skipWhite(p);
    if(parseLiteral(p, "var"))
        return parseDecl(p);
    if(parseLiteral(p, ">>>")){
        void* arg = parseAdd(p);
        if(!arg)
            return arg;
        skipWhite(p);
        if(!parseChar(p, ';')){
            printf("(line %d) Expected \';\' after print\n", p->lineNo);
            return NULL;
        }
        Quote* result = alloc(p, sizeof(Quote));
        result->type = 'q';
        result->expr = arg;
        return result;
    }
    if(parseRange(p, 'a', 'z') || parseRange(p, 'A', 'Z') || parseChar(p, '_')){
        p->begin--;
        return parseAssign(p);
    }

    printf("(line %d) Unexpected character: \'%c\'.\n", p->lineNo, p->begin[0]);
    return NULL;
}
static void* parseStatements(Parser* p){
    Binary* result = parseStatement(p);
    if(!result)
        return result;
    skipWhite(p);
    if(p->begin >= p->end)
        return result;
    Binary* lhs = result;
    Binary* rhs = parseStatements(p);
    if(!rhs)
        return rhs;
    result = alloc(p, sizeof(Binary));
    result->type = 's';
    result->lhs = lhs;
    result->rhs = rhs;
    return result;
}

static void printIndent(int indent){
    for(int i = 0; i < indent; i++){
        printf("    ");
    }
}
static void printBinary(Binary* b, char type, int indent){
    printIndent(indent);
    printf("{\n");
    printTree(b->lhs, indent + 1);
    printIndent(indent);
    putchar(type);
    putchar('\n');
    printTree(b->rhs, indent + 1);
    printIndent(indent);
    printf("}\n");
}
static void printDecl(Binary* b, int indent){
    printIndent(indent);
    printf("var\n");
    printTree(b->lhs, indent + 1);
    if(b->rhs)
        printTree(b->rhs, indent + 1);
}
static void printIdentifier(Identifier* id, int indent){
    printIndent(indent);
    for(int i = 0; i < id->length; i++){
        putchar(id->data[i]);
    }
    putchar('\n');
}

void printTree(void* tree, int indent){
    if(!tree) return;
    char* type = tree;
    switch(*type){
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=': return printBinary(tree, *type, indent);
        case 'i': return printIdentifier(tree, indent);
        case 'd': return printDecl(tree, indent);
        case 'n': printIndent(indent); printf("%d\n", ((Number*)tree)->value); return;
        case 'q': printIndent(indent); printf(">>>\n"); printTree(((Quote*)tree)->expr, indent + 1); return;
        case 's': printTree(((Binary*)tree)->lhs, indent); printTree(((Binary*)tree)->rhs, indent); return;
    }
}

void* parse(i8* source, i8* sourceEnd, i8* heap, i8* heapEnd){
    Parser p = {source, sourceEnd, heap,  heapEnd, 1};
    return parseStatements(&p);
}
