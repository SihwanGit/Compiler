/***************************************************************
*      scanner routine for Mini C language                    *
*                                   2003. 3. 10               *
***************************************************************/

#pragma once //헤더파일이 중복되지 않도록 방지

#define NO_KEYWORD 16 //미니C의 키워드 개수. 7(기존) + 9(과제 추가본) = 16
#define ID_LENGTH 999 //식별자의 최대길이

typedef struct tokenType {	//스캐너가 반환할 토큰의 구조
	int number;		// 토큰 타입을 나타내는 번호 (enum tsymbol 값 중 하나)
	union {
		char id[ID_LENGTH];		// 식별자(예: 변수 이름)
		int num;				// 정수 리터럴 값
	} value;		// 토큰의 실제 값 (식별자 이름 또는 숫자값)
} tokenType;


//토큰의 종류를 열거형으로 정의
//각 토큰의 번호와 이름, 해당하는 연산자, 기호, 키워드, EOF 등 설정
enum tsymbol {
	tnull = -1,
	tnot, tnotequ, tremainder, tremAssign, tident, tnumber,
	/* 0          1            2         3            4          5     */
	tand, tlparen, trparen, tmul, tmulAssign, tplus,
	/* 6          7            8         9           10         11     */
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
	/* 12         13          14        15           16         17     */
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
	/* 18         19          20        21           22         23     */
	tequal, tgreat, tgreate, tlbracket, trbracket, teof,
	/* 24         25          26        27           28         29     */

	//   ...........    word symbols ................................. //
	/* 30         31          32        33           34         35     */
	tconst, telse, tif, tint, treturn, tvoid,
	/* 36         37          38        39                             */
	twhile, tlbrace, tor, trbrace,

	//--------------    과제 추가본   ---------------------
	/* 40		  41		  42		43			 44			45		*/
	tchar, tdouble, tfor, tdo, tgoto, tswitch,
	/* 46		  47		  48		49			 50			51		*/
	tcase, tbreak, tdefault, tdoublelit, tcharlit, tstringlit,
	/*52	*/	
	tcolon
};
//tdoublelelit는 double형 리터럴 토큰
//tcharlit 는 char형 리터럴 토큰
//tstringlit는 string 형 리터럴 토큰


// 스캐너 함수 선언
// 소스 코드에서 다음 토큰을 읽어와 tokenType 구조체로 반환
struct tokenType scanner();

// 디버깅 및 출력용 함수
// 토큰의 내용을 콘솔에 출력
void printToken(struct tokenType token);
