#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"

// Node 구조체 정의
typedef struct nodeType {
    struct tokenType token;     // 토큰 저장
    enum { terminal, nonterm } noderep;
    struct nodeType* son;
    struct nodeType* brother;
    struct nodeType* father;
} Node;

// parser() 반환값
Node* parser();

// Parser.c에서 사용되는 함수들
void semantic(int n);
void printToken(struct tokenType token);
void dumpStack();
void errorRecovery();

Node* buildNode(struct tokenType token);
Node* buildTree(int nodeNumber, int rhsLength);
int meaningfulToken(struct tokenType token);

// sdt.c에서 사용하는 출력 함수
void printTree(Node* pt, int indent);
void printNode(Node* pt, int indent);

// nodeName 배열, enum nodeNumber도 외부 선언
extern char* nodeName[];

enum nodeNumber {
	ERROR_NODE,
	ACTUAL_PARAM, ADD, ADD_ASSIGN, ARRAY_VAR, ASSIGN_OP,
	CALL, COMPOUND_ST, CONST_NODE, DCL, DCL_ITEM,
	DCL_LIST, DCL_SPEC, DIV, DIV_ASSIGN, EQ,
	EXP_ST, FORMAL_PARA, FUNC_DEF, FUNC_HEAD,
	GE, GT, IDENT, IF_ELSE_ST, IF_ST,
	INDEX, INT_NODE, LE, LOGICAL_AND, LOGICAL_NOT,
	LOGICAL_OR, LT, MOD, MOD_ASSIGN, MUL,
	MUL_ASSIGN, NE, NUMBER, PARAM_DCL, POST_DEC,
	POST_INC, PRE_DEC, PRE_INC, PROGRAM, RETURN_ST,
	SIMPLE_VAR, STAT_LIST, SUB, SUB_ASSIGN, UNARY_MINUS,
	VOID_NODE, WHILE_ST
};

#endif
