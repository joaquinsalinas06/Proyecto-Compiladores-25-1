#include <iostream>
#include "token.h"

using namespace std;

Token::Token(Type type):type(type) { text = ""; }

Token::Token(Type type, char c):type(type) { text = string(1, c); }

Token::Token(Type type, const string& source, int first, int last):type(type) {
    text = source.substr(first, last);
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
    switch (tok.type) {
        case Token::END:outs << "TOKEN(END)"; break;     // Fin de entrada

        case Token::VAR:outs << "TOKEN(VAR)"; break;

        case Token::PLUS: outs << "TOKEN(PLUS)"; break;
        case Token::MINUS: outs << "TOKEN(MINUS)"; break;
        case Token::MUL: outs << "TOKEN(MUL)"; break;
        case Token::DIV: outs << "TOKEN(DIV)"; break;
        case Token::NUM: outs << "TOKEN(NUM)"; break;
        case Token::DECIMAL: outs << "TOKEN(DECIMAL)"; break;

        case Token::ERR: outs << "TOKEN(ERR)"; break;

        case Token::ASSIGN: outs << "TOKEN(ASSIGN)"; break;

        case Token::MINUS_ASSIGN: outs << "TOKEN(MINUS_ASSIGN)"; break;
        case Token::PLUS_ASSIGN: outs << "TOKEN(PLUS_ASSIGN)"; break;

        case Token::AND: outs << "TOKEN(AND)"; break;
        case Token::OR: outs << "TOKEN(OR)"; break;
        case Token::NOT: outs << "TOKEN(NOT)"; break;

        case Token::PI: outs << "TOKEN(PI)"; break;
        case Token::PD: outs << "TOKEN(PD)"; break;
        case Token::LLI: outs << "TOKEN(LLI)"; break;
        case Token::LLD: outs << "TOKEN(LLD)"; break;

        case Token::PRINT: outs << "TOKEN(PRINT)"; break;
        case Token::PRINTLN: outs << "TOKEN(PRINTLN)"; break;

        case Token::PC: outs << "TOKEN(PC)"; break;
        case Token::COMA: outs << "TOKEN(COMA)"; break;
        case Token::TWO_POINTS: outs << "TOKEN(TWO_POINTS)"; break;        
        case Token::ID: outs << "TOKEN(ID)"; break;
        case Token::LT: outs << "TOKEN(LT)"; break;
        case Token::LE: outs << "TOKEN(LE)"; break;
        case Token::GT: outs << "TOKEN(GT)"; break;
        case Token::GE: outs << "TOKEN(GE)"; break;
        case Token::EQ: outs << "TOKEN(EQ)"; break;
        case Token::NOT_EQ: outs << "TOKEN(NOT_EQ)"; break;
        case Token::DOTDOT: outs << "TOKEN(DOTDOT)"; break;

        case Token::IF: outs << "TOKEN(IF)"; break;
        case Token::ELSE_IF: outs << "TOKEN(ELSE_IF)"; break;
        case Token::ELSE: outs << "TOKEN(ELSE)"; break;

        case Token::WHILE: outs << "TOKEN(WHILE)"; break;
        case Token::DO: outs << "TOKEN(DO)"; break;

        case Token::FOR: outs << "TOKEN(FOR)"; break;
        case Token::IN: outs << "TOKEN(IN)"; break;

        case Token::TRUE : outs << "TOKEN(TRUE)"; break;
        case Token::FALSE : outs << "TOKEN(FALSE)"; break;

        case Token::INT: outs << "TOKEN(INT)"; break;
        case Token::FLOAT: outs << "TOKEN(FLOAT)"; break;
        case Token::BOOLEAN: outs << "TOKEN(BOOLEAN)"; break;
        case Token::UNIT: outs << "TOKEN(UNIT)"; break;

        case Token::FUN: outs << "TOKEN(FUN)"; break;
        case Token::RETURN: outs << "TOKEN(RETURN)"; break;

        default: outs << "TOKEN(UNKNOWN)"; break;
    }
    return outs;
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}
