#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
    enum Type {
        VAR, TWO_POINTS, END, // VAR, : y fin de entrada
        AND, OR, PLUS, MINUS, MUL, DIV, NUM, DECIMAL, ERR, // Operadores, NUM, DECIMAL y errores
        PLUS_ASSIGN, MINUS_ASSIGN, // Operadores de asignación
        PI, PD, ID, ASSIGN, PC, COMA, LT, LE, GT, GE, EQ, NOT_EQ, DOTDOT,  // (, ), id, =, ;, , , <, <=, >, >=, ==, ..
        PRINT, PRINTLN, // println: print con un salto de líneas | print: print sin salto de línea
        IF, LLI, LLD, ELSE_IF, ELSE, // IfStatement
        WHILE, DO, // WhileStatement
        FOR, IN, // in: para rangos en kotlin
        UNTIL, DOWNTO, STEP, // operadores de rango en kotlin
        FALSE, TRUE, // booleanos
        INT, FLOAT, BOOLEAN, UNIT, // enteros, floats, booleanos y unit (void)
        NOT,
        FUN, RETURN // Funciones y retorno
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