/***************************************************************
*      scanner routine for Mini C language                    *
***************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "Scanner.h"

extern FILE *sourceFile;                       // miniC source program
//스캐너는 이 파일을 한 문자씩 읽으며 토큰을 인식함.

int superLetter(char ch);
int superLetterOrDigit(char ch);
int getNumber(char firstCharacter);
int hexValue(char ch);
void lexicalError(int n);


char *tokenName[] = { //헤더파일에서 정의된 번호와 이름에 해당하는 각 토큰들.
	"!",        "!=",      "%",       "%=",     "%ident",   "%number",
	/* 0          1           2         3          4          5        */
	"&&",       "(",       ")",       "*",      "*=",       "+",
	/* 6          7           8         9         10         11        */
	"++",       "+=",      ",",       "-",      "--",	    "-=",
	/* 12         13         14        15         16         17        */
	"/",        "/=",      ";",       "<",      "<=",       "=",
	/* 18         19         20        21         22         23        */
	"==",       ">",       ">=",      "[",      "]",        "eof",
	/* 24         25         26        27         28         29        */
	//   ...........    word symbols ................................. //
	/* 30         31         32        33         34         35        */
	"const",    "else",     "if",      "int",     "return",  "void",
	/* 36         37         38        39                              */
	"while",    "{",        "||",       "}",

	
	//--------------    과제 추가본     ----------------//
	/* 40		  41		  42		43			 44			45		*/
	"char", "double", "for", "do", "goto", "switch",
	/* 46		  47		  48		49			 50			51		*/
	"case", "break", "default", "doublelit", "charlit", "stringlit",
	/*52		*/
	":"
};
//double_lit, char_lit, string_lit은 리터럴 구현을 위한 토큰

char *keyword[NO_KEYWORD] = { //각 키워드들. NO_KEYWORD는 헤더파일에서 정의됨.
	"const",  "else",    "if",    "int",    "return",  "void",    "while",
	"char", "double", "for", "do", "goto", "switch", "case", "break", "default"
};
//여기에 추가 키워드 : char, double, for, do, goto, switch, case, break, default를 추가.

enum tsymbol tnum[NO_KEYWORD] = {//각 키워드들의 토큰
	tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
	tchar, tdouble, tfor, tdo, tgoto, tswitch, tcase, tbreak, tdefault
}; //추가 키워드의 토큰도 추가



/*
Mini C 소스 파일에서 하나의 토큰을 인식해서 tokenType 구조체로 반환.
공백을 무시하고, 식별자/키워드, 숫자, 연산자/구분자 등을 인식
인식 실패 시 lexicalError() 호출.
*/
struct tokenType scanner()
{
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH];

	token.number = tnull;

	do {
		while (isspace(ch = fgetc(sourceFile))); //파일에 있는 공백들은 무시

		if (superLetter(ch)) { //식별자 또는 키워드 처리
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;//한문자씩 읽기
				ch = fgetc(sourceFile);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1); //제한 길이를 넘으면 에러
			id[i] = '\0'; //마지막 문자는 널문자

			ungetc(ch, sourceFile);  //  retract
						 
			for (index = 0; index < NO_KEYWORD; index++) // 키워드 테이블에서 해당 식별자 찾기
				if (!strcmp(id, keyword[index])) break; //일치하지 않으면 탐색 종료 후 다음 키워드 가져오기
			if (index < NO_KEYWORD)    // 만약 index가 키워드의 수보다 작으면 찾기 성공
				token.number = tnum[index];
			else {                     // index가 키워드의 수보다 크거나 같으면 찾기 실패
				token.number = tident;
				strcpy(token.value.id, id);
			}
		}  // end of identifier or keyword
		//추가되는 키워드들은 여기에서 다룬다.

	/*	else if (isdigit(ch)) {  // 숫자
			token.number = tnumber;
			token.value.num = getNumber(ch);
		}
		456. 처럼 숏폼을 인식하기 위해 지운 것. 얘는 정수만 입력 가능
	*/

		else if (isdigit(ch) || ch == '.') { // 정수 또는 실수 리터럴 시작.
			//숏폼 인식을 위해 소수점으로 시작하는 경우도 추가함.
			int isFloat = 0;		// 소수인지 여부를 나타내는 플래그
			double num = 0.0;		// 최종 숫자값 저장 (정수 또는 실수)
			double frac = 0.1;		// 소수점 이하 자리수 계산용 (0.1, 0.01, 0.001 등)
			char buf[ID_LENGTH];	// 토큰의 원본 문자열 저장용
			int bi = 0;				// buf의 인덱스

			if (ch == '.') { //.으로 시작하면 앞에 0을 붙인다.
				isFloat = 1;			// 소수로 간주
				buf[bi++] = '0';		// Ex : .123 -> 0.123
				buf[bi++] = '.';
				ch = fgetc(sourceFile);	// 다음 문자 읽기 (숫자여야 함)
				if (!isdigit(ch)) {		// .다음에 숫자가 없으면 에러
					lexicalError(4);
					break;
				}
				while (isdigit(ch)) {			// 소수점 이하 숫자 읽기
					buf[bi++] = ch;
					num += (ch - '0') * frac;	// 자리수 곱해 누적
					frac /= 10;
					ch = fgetc(sourceFile);
				}
				ungetc(ch, sourceFile);
			}

			// 정수 혹은 456. 같이 소수점 뒤 생략된 실수 처리
			else {
				while (isdigit(ch)) {
					buf[bi++] = ch;
					num = num * 10 + (ch - '0');
					ch = fgetc(sourceFile);
				}
				if (ch == '.') {	// 소수점이 뒤따라오면 소수로 전환
					isFloat = 1;
					buf[bi++] = '.';
					ch = fgetc(sourceFile);
					while (isdigit(ch)) {
						buf[bi++] = ch;
						num += (ch - '0') * frac;
						frac /= 10;
						ch = fgetc(sourceFile);
					}
					ungetc(ch, sourceFile); // 다음 토큰을 위해 문자 돌려놓기.
					//이게 없으면 숏폼 뒤에 붙은 세미콜론을 인식할 수 없음.
				}
				else {
					ungetc(ch, sourceFile); // '.'이 없었으면 문자 복구
				}
			}

			buf[bi] = '\0';

			if (isFloat) {
				token.number = tdoublelit; // 실수 리터럴로 인식
				strcpy(token.value.id, buf);
			}
			else {
				token.number = tnumber;    // 정수 리터럴로 인식
				token.value.num = (int)num;
			}
			break;
		}


		else switch (ch) {  // 연산자나 기호같은 특수기호
		case '/': //만약 /가 나왔는데, 그 뒤에 다음과 같은 기호들이 나오면
			ch = fgetc(sourceFile);
			if (ch == '*')			// /*가 나오면 text comment.
				do {
					while (ch != '*') ch = fgetc(sourceFile);
					ch = fgetc(sourceFile);
				} while (ch != '/'); //또다시 */이 나오기 전까진 주석처리

			else if (ch == '/')		// line comment. //으로 나왔다면 \n 전까지만 주석처리
				while (fgetc(sourceFile) != '\n' && ch != EOF);

			else if (ch == '=')  token.number = tdivAssign; // /=라는 복합대입연산자. a = a / b의 줄임말.
			else {
				token.number = tdiv;
				ungetc(ch, sourceFile); // retract
			}
			break;

		case '!': //!가 나오면 != 인지, 그냥 !인지 체크
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tnotequ;
			else {
				token.number = tnot;
				ungetc(ch, sourceFile); // retract
			}
			break;

		case '%': //%가 나오면 그냥 %인지 %=인지 체크. %=는 복합대입연산자
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tremAssign;
			}
			else {
				token.number = tremainder;
				ungetc(ch, sourceFile);
			}
			break;

		case '&': //&가 나오면 &&인지 체크. 그렇지 않다면 에러처리
			ch = fgetc(sourceFile);
			if (ch == '&')  token.number = tand;
			else {
				lexicalError(2);
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '*': //*는 *=인지 그냥 *인지 체크
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tmulAssign;
			else {
				token.number = tmul;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '+': //+는 ++인지 +=인지 그냥 +인지 체크. ++은 증감, +=는 복합대입연산자
			ch = fgetc(sourceFile);
			if (ch == '+')  token.number = tinc;
			else if (ch == '=') token.number = taddAssign;
			else {
				token.number = tplus;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '-': //-는 --와 -와 -= 중 체크. --는 증감연산자
			ch = fgetc(sourceFile);
			if (ch == '-')  token.number = tdec;
			else if (ch == '=') token.number = tsubAssign;
			else {
				token.number = tminus;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '<': //<는 <=인지 그냥 <인지 체크
			ch = fgetc(sourceFile);
			if (ch == '=') token.number = tlesse;
			else {
				token.number = tless;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '=': // =도 그냥 =인지 ==인지 체크
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tequal;
			else {
				token.number = tassign;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '>': // >와 >= 중 체크
			ch = fgetc(sourceFile);
			if (ch == '=') token.number = tgreate;
			else {
				token.number = tgreat;
				ungetc(ch, sourceFile);  // retract
			}
			break;

		case '|': // ||가 아니라면 에러
			ch = fgetc(sourceFile);
			if (ch == '|')  token.number = tor;
			else {
				lexicalError(3);
				ungetc(ch, sourceFile);  // retract
			}
			break;
			//이 밑에는 기타 특수문자 처리
		case '(': token.number = tlparen;         break;
		case ')': token.number = trparen;         break;
		case ',': token.number = tcomma;          break;
		case ';': token.number = tsemicolon;      break;
		case '[': token.number = tlbracket;       break;
		case ']': token.number = trbracket;       break;
		case '{': token.number = tlbrace;         break;
		case '}': token.number = trbrace;         break;
		case EOF: token.number = teof;            break;


		// 문자 리터럴 처리 블록 수정:
		case '\'': {
			char charValue;		// 실제 문자 값을 저장할 변수
			//switch문 내부에서 생성된 변수기 떄문에 범위를 혼동하지 않게끔
			//case 문에 { } 사용함.
			ch = fgetc(sourceFile);
			if (ch == '\\') {	// 만약 이스케이프 문자라면 다음 문자 확인
				ch = fgetc(sourceFile);
				switch (ch) {
				case 'n': charValue = '\n'; break;
				case 't': charValue = '\t'; break;
				case '\\': charValue = '\\'; break;
				case '\'': charValue = '\''; break;
				case '0': charValue = '\0'; break;
				default:
					lexicalError(4);
					break;
				}
			}
			else {
				charValue = ch;	// 일반 문자라면 그대로 저장
			}

			ch = fgetc(sourceFile);	// 닫는 작은 따옴표 읽기
			if (ch != '\'') {
				lexicalError(4); // 닫는 따옴표 없으면 에러
			}
			else {
				token.number = tcharlit; // 문자 리터럴 토큰
				token.value.num = (int)charValue;
			}
			break;
		}

		case '"': // 문자열 리터럴 처리
			i = 0;
			while ((ch = fgetc(sourceFile)) != '"' && ch != EOF && i < ID_LENGTH - 1) {
				if (ch == '\\') { // 이스케이프 문자 처리
					char esc = fgetc(sourceFile);
					switch (esc) {
					case 'n': id[i++] = '\n'; break;
					case 't': id[i++] = '\t'; break;
					case '\\': id[i++] = '\\'; break;
					case '"': id[i++] = '"'; break;
					default: id[i++] = esc; break;
					}
				}
				else {
					id[i++] = ch;
				}
			}
			id[i] = '\0'; //널문자를 읽으면 문자열 종료
			if (ch != '"') {
				lexicalError(4); // 닫는 큰따옴표 누락되면 에러
			}
			else {
				token.number = tstringlit;
				strcpy(token.value.id, id); // 문자열은 id에 저장
			}
			break;

		case ':': token.number = tcolon; break; // case 문에 사용될 콜론':'을 인식하기 위한 스위치문.

		default: {
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}

		} // switch end
	} while (token.number == tnull);
	return token;
} // end of scanner



//오류 코드 n에 따른 메세지 출력
void lexicalError(int n)
{
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n"); //id의 길이가 너무 길때 발생하는 에러. 헤더파일에서 999로 조정함.
		break;
	case 2: printf("next character must be &\n"); // &뒤에 또 &가 오지 않아 발생하는 에러
		break;
	case 3: printf("next character must be |\n"); // |뒤에 또 |가 오지 않아 발생하는 에러
		break;
	case 4: printf("invalid character\n"); // 정의되지 않았거나 인식할 수 없는 문자가 왔을 떄 발생하는 에러
		break;
	}
}


//C언어 기준 식별자의 유효한 첫문자 검사
int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

//그 이후 검사
int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	else return 0;
}


//숫자 인식 함수
int getNumber(char firstCharacter)
{
	int num = 0;
	int value;
	char ch;

	if (firstCharacter == '0') {
		ch = fgetc(sourceFile);
		if ((ch == 'X') || (ch == 'x')) {		// hexa decimal. 0x 또는 0X는 16진수
			while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
				num = 16 * num + value;
		}
		else if ((ch >= '0') && (ch <= '7'))	// octal. 8진수
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(sourceFile);
			} while ((ch >= '0') && (ch <= '7'));
		else num = 0;						// zero. 아무것도 안오면 그냥 0
	}
	else {									// decimal
		ch = firstCharacter;
		do {
			num = 10 * num + (int)(ch - '0');
			ch = fgetc(sourceFile);
		} while (isdigit(ch));
	}
	ungetc(ch, sourceFile);  /*  retract  */
	return num;
}
//현재는 16,8,10진수만 인식하지만 실수형을 추가할 예정


//16진수. 문자를 숫자로 변환
int hexValue(char ch)
{
	switch (ch) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (ch - '0');
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10);
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10);
	default: return -1;
	}
}


//인식된 토큰을 콘솔에 출력
void printToken(struct tokenType token)
{
	if (token.number == tident)
		printf("number: %d, value: %s\n", token.number, token.value.id);
	else if (token.number == tnumber)
		printf("number: %d, value: %d\n", token.number, token.value.num);
	else
		printf("number: %d(%s)\n", token.number, tokenName[token.number]);

}