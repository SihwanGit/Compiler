#include <stdio.h>
#include "Parser.h"

// printTree와 printNode는 AST를 출력하기 위한 SDT 함수
// Parser.h에서 Node 구조체와 nodeName 배열이 선언되어 있다.

// 트리를 들여쓰기 형태로 출력하는 함수
// 현재 노드를 출력한 후, 비단말(nonterm)인 경우 자식(s0n)을 재귀적으로 출력하고 같은 레벨의 형제(brother) 노드도 순서대로 출력한다.
void printTree(Node* pt, int indent)
{
    Node* p = pt;

    // 현재 노드부터 형제 방향으로 순회
    while (p != NULL) {
        // 현재 노드 출력
        printNode(p, indent);

        // 비단말 노드이면 자식 노드를 들여쓰기 +5 하고 재귀 호출
        if (p->noderep == nonterm)
            printTree(p->son, indent + 5);

        // 다음 형제 노드로 이동
        p = p->brother;
    }
}

// 개별 노드를 출력하는 함수
// 단말(terminal)인 경우 토큰 값(id 또는 숫자)을 출력
// 비단말(nonterm)인 경우 nodeName 배열을 이용해 노드 이름을 출력
void printNode(Node* pt, int indent)
{
    extern FILE* astFile; // AST 출력 파일 포인터
    int i;

    // 들여쓰기 출력
    for (i = 1; i <= indent; i++)
        fprintf(astFile, " ");

    // 단말 노드 출력
    if (pt->noderep == terminal) {
        if (pt->token.number == tident)
            // 식별자 토큰 출력
            fprintf(astFile, " Terminal: %s", pt->token.value.id);
        else if (pt->token.number == tnumber)
            // 숫자 토큰 출력
            fprintf(astFile, " Terminal: %d", pt->token.value.num);
    }
    // 비단말 노드 출력
    else {
        i = pt->token.number;  // 노드 번호
        fprintf(astFile, " Nonterminal: %s", nodeName[i]);
    }

    // 줄바꿈
    fprintf(astFile, "\n");
}
