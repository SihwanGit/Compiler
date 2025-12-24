#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#define LABEL_SIZE 10

#define MAX_SYMBOLS 1000
#define MAX_ID_LEN  32

#define INT_TYPE    1
#define VOID_TYPE   2

#define VAR_TYPE    1
#define CONST_TYPE  2
#define FUNC_TYPE   3

typedef struct { //symbol의 구조
    char id[MAX_ID_LEN];
    int  typeSpecifier; //타입 지정자
    int  typeQualifier; //타입 한정자
    int  base; //base
    int  offset; //offset
    int  width;
    int  initialValue;
} Symbol;

extern void emitSym(int base, int offset, int size);

Symbol symbolTable[MAX_SYMBOLS]; //테이블 역할을 수행할 심볼들의 배열
int symbolTableTop = 0; //심볼 스택의 탑

int base = 1;
int offset = 1;

void initSymbolTable() //심볼 테이블과 스코프 정보를 초기 상태로 되돌린다
{
    symbolTableTop = 0;
    base = 1;
    offset = 1;
}

static int normalizeBase(int typeQualifier, int inBase) //삽입 시 base 값이 0 등으로 들어올 때 현재 스코프 기준으로 보정한다
{
    if (typeQualifier == FUNC_TYPE) return 1; //함수는 전역(base=1)에 등록
    if (typeQualifier == CONST_TYPE && inBase == 0) return base; //상수 base가 0으로 오면 현재 base로 보정
    if (inBase == 0) return base; //일반 심볼 base가 0이면 현재 base로 보정
    return inBase; //이미 올바른 base면 그대로 사용
}

static int normalizeOffset(int typeQualifier, int inOffset) //삽입 시 offset 값이 의미 없는 경우를 보정한다
{
    if (typeQualifier == FUNC_TYPE) return 0; //함수는 offset 의미 없으므로 0
    if (typeQualifier == CONST_TYPE && inOffset == 0) return 0; //상수는 메모리 슬롯을 쓰지 않으므로 0 유지
    return inOffset; //변수/배열 등은 전달받은 offset 사용
}

int insert(char* id, int typeSpecifier, int typeQualifier,
    int inBase, int inOffset, int width, int initialValue) //심볼을 테이블에 추가하고 인덱스를 반환한다(중복이면 기존 인덱스 반환)
{
    int i;
    int realBase = normalizeBase(typeQualifier, inBase); //base 보정
    int realOffset = normalizeOffset(typeQualifier, inOffset); //offset 보정

    for (i = symbolTableTop - 1; i >= 0; i--) { //현재 스코프(realBase) 안에서만 중복 이름 검사
        if (symbolTable[i].base < realBase) break; //더 바깥 스코프로 넘어가면 중복 검사 종료
        if (!strcmp(symbolTable[i].id, id)) { //같은 스코프에 동일 이름이 있으면 중복
            printf("duplicate identifier: %s\n", id);
            return i;
        }
    }

    if (symbolTableTop >= MAX_SYMBOLS) { //테이블 크기 초과 방지
        printf("symbol table overflow\n");
        return -1;
    }

    strcpy(symbolTable[symbolTableTop].id, id); //이름 저장
    symbolTable[symbolTableTop].typeSpecifier = typeSpecifier; //타입 저장
    symbolTable[symbolTableTop].typeQualifier = typeQualifier; //종류 저장(VAR/CONST/FUNC)
    symbolTable[symbolTableTop].base = realBase; //보정된 base 저장
    symbolTable[symbolTableTop].offset = realOffset; //보정된 offset 저장
    symbolTable[symbolTableTop].width = width; //배열 크기 또는 인자 개수 등 저장
    symbolTable[symbolTableTop].initialValue = initialValue; //상수 초기값 저장

    return symbolTableTop++; //삽입 위치 반환 후 top 증가
}

int lookup(char* id) //가장 안쪽 스코프부터 이름이 같은 심볼을 찾아 인덱스를 반환한다
{
    int i;
    for (i = symbolTableTop - 1; i >= 0; i--) { //스택 top부터 역순 탐색
        if (!strcmp(symbolTable[i].id, id))
            return i;
    }
    return -1; //없으면 -1
}

void set() //함수 진입 등 새 스코프로 들어갈 때 base를 증가시키고 offset을 초기화한다
{
    base++;
    offset = 1;
}

void reset() //현재 스코프의 심볼을 모두 제거하고 base를 감소시켜 이전 스코프로 복귀한다
{
    int i;

    for (i = symbolTableTop - 1; i >= 0; i--) { //현재 base에 속한 심볼들을 pop
        if (symbolTable[i].base == base)
            symbolTableTop--;
        else
            break;
    }

    base--;
    offset = 1; //복귀 후 offset을 기본값으로 초기화
}

void genSym(int currentBase) //현재 스코프의 변수 심볼들에 대해 sym 지시어를 출력한다
{
    int i;

    for (i = 0; i < symbolTableTop; i++) {
        // sym은 실제 저장공간이 필요한 변수(VAR_TYPE)만 생성해야 한다
        if (symbolTable[i].base == currentBase &&
            symbolTable[i].typeQualifier == VAR_TYPE) {
            emitSym(symbolTable[i].base,
                symbolTable[i].offset,
                symbolTable[i].width);
        }
    }
}

void dumpSymbolTable() //디버깅용으로 심볼 테이블의 내용을 출력한다
{
    int i;

    printf("Symbol Table Dump\n");
    for (i = 0; i < symbolTableTop; i++) {
        printf("[%3d] %-10s base=%d offset=%d width=%d\n",
            i,
            symbolTable[i].id,
            symbolTable[i].base,
            symbolTable[i].offset,
            symbolTable[i].width);
    }
}
