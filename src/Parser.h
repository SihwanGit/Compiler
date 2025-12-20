#ifndef PARSER_H
#define PARSER_H

void semantic(int);
void printToken(struct tokenType token); // -> MiniC Scanner
void dumpStack();
void errorRecovery();

void parser();


#endif // PARSER_H