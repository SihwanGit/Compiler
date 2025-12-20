#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "src/Parser.h"

#define MAX_FILE_NAME_LEN	999

FILE* sourceFile;                       // miniC source program

void parser_error(int n);


void main(int argc, char* argv[])
{
	char fileName[MAX_FILE_NAME_LEN];

	printf(" *** start of Mini C Compiler\n");
	if (argc != 2) { // check for input source file name only.
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

	printf(" === start of Parser\n");
	parser();
	
	printf(" *** end   of Mini C Compiler\n");
} // end of main

void parser_error(int n)
{
	printf("parser_error: %d\n", n);
}
