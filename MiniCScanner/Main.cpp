#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Scanner.h"

void icg_error(int n);


FILE *astFile;                          // AST file
FILE *sourceFile;                       // miniC source program
FILE *ucodeFile;                        // ucode file

#define FILE_LEN 999

void main(int argc, char *argv[])
{
	char fileName[FILE_LEN];
	int err;

	printf(" *** start of Mini C Compiler\n");
	if (argc != 2) { //입력인자가 부족하면 에러
		icg_error(1);
		exit(1);
	}

	// 입력된 파일 이름을 fileName 배열에 복사
	strcpy_s(fileName, argv[1]);
	printf("   * source file name: %s\n", fileName);

	// sourceFile 포인터를 사용해 파일 열기 (읽기 전용)
	err = fopen_s(&sourceFile, fileName, "r");
	if (err != 0) {
		icg_error(2); //읽기 실패시 에러
		exit(1);
	}
	
	
	struct tokenType token; // scanner 함수가 반환할 토큰 구조체 변수 선언
	
	printf(" === start of Scanner\n");
	
	token = scanner(); // 첫 번째 토큰을 읽어옴
	
	// 파일 끝(EOF)까지 반복하며 모든 토큰을 처리
	while (token.number != teof) { 

		printf("Current Token --> ");
		printToken(token); // 현재 토큰 정보 출력
		token = scanner(); // 다음 토큰 읽기
		
	} /* while (1) */


	// 구문 분석기 (Parser) 시작 메시지 - 현재는 미구현
	printf(" === start of Parser\n");
	printf(" > Not yet implemented...\n");
	//root = parser();
	//printTree(root, 0);
	

	// 중간 코드 생성기 (ICG) 시작 메시지 - 현재는 미구현
	printf(" === start of ICG\n");
	printf(" > Not yet implemented...\n");

	//codeGen(root);
	printf(" *** end   of Mini C Compiler\n");
} // end of main


void icg_error(int n) //디버깅 용 에러 함수. 숫자를 통해 어디서 에러가 발생했는지 알 수 있다.
{
	printf("icg_error: %d\n", n);
	//3:printf("A Mini C Source file must be specified.!!!\n");
	//"error in DCL_SPEC"
	//"error in DCL_ITEM"
}
