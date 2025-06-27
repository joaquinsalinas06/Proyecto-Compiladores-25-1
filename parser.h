#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "exp.h"

class Parser {
private:
    Scanner* scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd(); 
    Exp* parseCExp();

    Exp* parseAExp();
    Exp* parseUnary();

    Exp* parseExpression();
    Exp* parseTerm();
    Exp* parseFactor();
    Exp* parseRangeExpression();
    Exp* parseArrayExpression();
    Exp* parseArrayAccess(Exp* array);
public:
    Parser(Scanner* scanner);
    Program* parseProgram();
    Stm* parseStatement();
    StatementList* parseStatementList();
    VarDec* parseVarDec();
    VarDecList* parseVarDecList();
    Body* parseBody();
    
    FunDec* parseFunDec();
    FunDecList* parseFunDecList();
    Exp* parseFCallExp();
    ReturnStatement* parseReturnStatement();
    
};

#endif // PARSER_H