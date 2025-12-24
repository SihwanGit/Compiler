// Parser.c
#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"     // 변경된 부분. Node 구조체, enum, nodeName 외부 선언 포함
#include "MiniC.tbl"
#include "Scanner.h"

#define PS_SIZE 100     // parsing stack size

// nodeName 정의
char* nodeName[] = {
    "ERROR_NODE",
    "ACTUAL_PARAM", "ADD", "ADD_ASSIGN", "ARRAY_VAR", "ASSIGN_OP",
    "CALL", "COMPOUND_ST", "CONST_NODE", "DCL", "DCL_ITEM",
    "DCL_LIST", "DCL_SPEC", "DIV", "DIV_ASSIGN", "EQ",
    "EXP_ST", "FORMAL_PARA", "FUNC_DEF", "FUNC_HEAD",
    "GE", "GT", "IDENT", "IF_ELSE_ST", "IF_ST",
    "INDEX", "INT_NODE", "LE", "LOGICAL_AND", "LOGICAL_NOT",
    "LOGICAL_OR", "LT", "MOD", "MOD_ASSIGN", "MUL",
    "MUL_ASSIGN", "NE", "NUMBER", "PARAM_DCL", "POST_DEC",
    "POST_INC", "PRE_DEC", "PRE_INC", "PROGRAM", "RETURN_ST",
    "SIMPLE_VAR", "STAT_LIST", "SUB", "SUB_ASSIGN", "UNARY_MINUS",
    "VOID_NODE", "WHILE_ST"
};

// ruleName 정의
int ruleName[] = {
    0, PROGRAM, 0, 0, 0,
    0, FUNC_DEF, FUNC_HEAD, DCL_SPEC, 0,
    0, 0, 0, CONST_NODE, INT_NODE,
    VOID_NODE, 0, FORMAL_PARA, 0, 0,
    0, 0, PARAM_DCL, COMPOUND_ST, DCL_LIST,
    DCL_LIST, 0, 0, DCL, 0,
    0, DCL_ITEM, DCL_ITEM, SIMPLE_VAR, ARRAY_VAR,
    0, 0, STAT_LIST, 0, 0,
    0, 0, 0, 0, 0,
    0, EXP_ST, 0, 0, IF_ST,
    IF_ELSE_ST, WHILE_ST, RETURN_ST, 0, 0,
    ASSIGN_OP, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN,
    MOD_ASSIGN, 0, LOGICAL_OR, 0, LOGICAL_AND,
    0, EQ, NE, 0, GT,
    LT, GE, LE, 0, ADD,
    SUB, 0, MUL, DIV, MOD,
    0, UNARY_MINUS, LOGICAL_NOT, PRE_INC, PRE_DEC,
    0, INDEX, CALL, POST_INC, POST_DEC,
    0, 0, ACTUAL_PARAM, 0, 0,
    0, 0, 0
};

int sp;
int stateStack[PS_SIZE];
int symbolStack[PS_SIZE];
Node* valueStack[PS_SIZE];

Node* parser()
{
    extern int parsingTable[NO_STATES][NO_SYMBOLS + 1];
    extern int leftSymbol[NO_RULES + 1];
    extern int rightLength[NO_RULES + 1];

    int entry, ruleNumber, lhs;
    int currentState;
    struct tokenType token;
    Node* ptr;

    sp = 0;
    stateStack[sp] = 0;

    token = scanner();

    while (1) {
        currentState = stateStack[sp];
        entry = parsingTable[currentState][token.number];

        if (entry > 0) {
            sp++;
            if (sp > PS_SIZE) {
                printf("critical compiler error: parsing stack overflow");
                exit(1);
            }

            symbolStack[sp] = token.number;
            stateStack[sp] = entry;
            valueStack[sp] = meaningfulToken(token) ? buildNode(token) : NULL;

            token = scanner();
        }
        else if (entry < 0) {
            ruleNumber = -entry;

            if (ruleNumber == GOAL_RULE) {
                return valueStack[sp - 1];
            }

            ptr = buildTree(ruleName[ruleNumber], rightLength[ruleNumber]);

            sp = sp - rightLength[ruleNumber];
            lhs = leftSymbol[ruleNumber];
            currentState = parsingTable[stateStack[sp]][lhs];

            sp++;
            symbolStack[sp] = lhs;
            stateStack[sp] = currentState;
            valueStack[sp] = ptr;
        }
        else {
            printf(" === error in source ===\n");
            printf("Current Token : ");
            printToken(token);
            dumpStack();
            errorRecovery();
            token = scanner();
        }
    }
}

void semantic(int n)
{
    printf("reduced rule number = %d\n", n);
}

void dumpStack()
{
    int i, start;

    if (sp > 10) start = sp - 10;
    else start = 0;

    printf("\n *** dump state stack :");
    for (i = start; i <= sp; ++i)
        printf(" %d", stateStack[i]);

    printf("\n *** dump symbol stack :");
    for (i = start; i <= sp; ++i)
        printf(" %d", symbolStack[i]);
    printf("\n");
}

void printToken(struct tokenType token)
{
    if (token.number == tident)
        printf("%s", token.value.id);
    else if (token.number == tnumber)
        printf("%d", token.value.num);
    else
        printf("%s", tokenName[token.number]);
}

void errorRecovery()
{
    struct tokenType tok;
    int parenthesisCount = 0, braceCount = 0;
    int i;

    while (1) {
        tok = scanner();
        if (tok.number == teof) exit(1);

        if (tok.number == tlparen) parenthesisCount++;
        else if (tok.number == trparen) parenthesisCount--;

        if (tok.number == tlbrace) braceCount++;
        else if (tok.number == trbrace) braceCount--;

        if ((tok.number == tsemicolon) &&
            (parenthesisCount <= 0) && (braceCount <= 0))
            break;
    }

    for (i = sp; i >= 0; i--) {
        if (stateStack[i] == 36) break;
        if (stateStack[i] == 24) break;
        if (stateStack[i] == 25) break;
        if (stateStack[i] == 17) break;
        if (stateStack[i] == 2) break;
        if (stateStack[i] == 0) break;
    }
    sp = i;
}

int meaningfulToken(struct tokenType token)
{
    if (token.number == tident || token.number == tnumber)
        return 1;
    return 0;
}

Node* buildNode(struct tokenType token)
{
    Node* ptr = malloc(sizeof(Node));
    if (!ptr) {
        printf("malloc error in buildNode\n");
        exit(1);
    }
    ptr->token = token;
    ptr->noderep = terminal;
    ptr->son = NULL;
    ptr->brother = NULL;
    ptr->father = NULL;
    return ptr;
}

Node* buildTree(int nodeNumber, int rhsLength)
{
    int i, j, start;
    Node* first;
    Node* ptr;

    i = sp - rhsLength + 1;

    while (i <= sp && valueStack[i] == NULL) i++;
    if (!nodeNumber && i > sp) return NULL;

    start = i;

    while (i <= sp - 1) {
        j = i + 1;
        while (j <= sp && valueStack[j] == NULL) j++;
        if (j <= sp) {
            ptr = valueStack[i];
            while (ptr->brother) ptr = ptr->brother;
            ptr->brother = valueStack[j];
        }
        i = j;
    }

    first = (start > sp) ? NULL : valueStack[start];

    if (nodeNumber) {
        ptr = malloc(sizeof(Node));
        if (!ptr) {
            printf("malloc error in buildTree\n");
            exit(1);
        }
        ptr->token.number = nodeNumber;
        ptr->noderep = nonterm;
        ptr->son = first;
        ptr->brother = NULL;
        ptr->father = NULL;
        return ptr;
    }
    return first;
}
