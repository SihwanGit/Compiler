/*
    Mini C 리터럴 및 키워드 테스트 예제
*/

void main()
{
    char c = 'A';            // 문자 리터럴
    char newline = '\n';     // 이스케이프 문자 포함
    char io = 'a;		//닫는 따옴표를 입력하지 않은 오류 예제

    double x = 3.14;         // 일반적인 double 리터럴
    double y = .123;         // 소수점 앞 생략
    double z = 456.;         // 소수점 뒤 생략

    char* s = "hello world"; // 문자열 리터럴

    for (int i = 0; i < 3; i++) {
        write(s);
        if (i == 1)
            break;
    }

    do {
        x = x - 1;
    } while (x > 0);

    switch (c) {
        case 'A':
            goto labelA;
        default:
            break;
    }

labelA:
    return;
}
