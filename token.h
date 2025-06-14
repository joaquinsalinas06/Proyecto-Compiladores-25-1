#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
    enum Type {
        VAR, TWO_POINTS, END, // VAR, : y fin de entrada
        PLUS, MINUS, MUL, DIV, NUM, DECIMAL, ERR, // Operadores, NUM, DECIMAL y errores
        PLUS_ASSIGN, MINUS_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, // Operadores de asignación
        PLUS_PLUS, MINUS_MINUS, // Operadores de incremento y decremento
        PI, PD, ID, ASSIGN, PC, COMA, LT, LE, EQ,  // (, ), id, =, ;, , , <, <=, ==
        PRINT, PRINTLN, // println: print con un salto de líneas | print: print sin salto de línea
        IF, LLI, LLD, ELSE_IF, ELSE, // IfStatement
        WHILE, DO, // WhileStatement
        FOR, IN, // in: para rangos en kotlin
        FALSE, TRUE, // booleanos
        INT, FLOAT // enteros y floats
    };

    Type type;
    std::string text;
    bool has_f = false;
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const std::string& source, int first, int last);

    friend std::ostream& operator<<(std::ostream& outs, const Token& tok);
    friend std::ostream& operator<<(std::ostream& outs, const Token* tok);
};

#endif // TOKEN_H