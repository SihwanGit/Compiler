#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Parser.h"    // parser(), Node 구조체, printTree, printNode 선언 포함

#define MAX_FILE_NAME_LEN	999

FILE* sourceFile;    // miniC source program
FILE* astFile;       // AST 출력 파일 (sdt.c에서 extern으로 참조됨)

void parser_error(int n);

void main(int argc, char* argv[])
{
    char fileName[MAX_FILE_NAME_LEN];
    Node* root;    // parser가 반환하는 AST 루트 노드를 저장

    printf(" *** start of Mini C Compiler\n");

    if (argc != 2) {
        // 입력 파일명 개수 확인
        parser_error(1);
        exit(1);
    }

    strcpy_s(fileName, MAX_FILE_NAME_LEN, argv[1]);
    printf("   * source file name: %s\n", fileName);

    errno_t err = fopen_s(&sourceFile, fileName, "r");
    if (err != 0 || sourceFile == NULL) {
        parser_error(2);
        exit(1);
    }

    // AST 출력 파일 준비
    astFile = fopen("ast.txt", "w");
    if (astFile == NULL) {
        printf("cannot open ast.txt\n");
        exit(1);
    }

    printf(" === start of Parser\n");

    // parser()는 AST의 루트 노드 포인터를 반환함
    root = parser();

    // sdt.c를 이용한 AST 출력
    if (root != NULL) {
        fprintf(astFile, "===== Abstract Syntax Tree =====\n\n");
        printTree(root, 0);
    }

    fclose(astFile);
    fclose(sourceFile);

    printf(" *** end   of Mini C Compiler\n");
}

void parser_error(int n)
{
    printf("parser_error: %d\n", n);
}
