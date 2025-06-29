#include <iostream>
#include <cstring>
#include "token.h"
#include "scanner.h"

using namespace std;

Scanner::Scanner(const char* s):input(s),first(0), current(0) { }

bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

Token* Scanner::nextToken() {
    Token* token;
    while (current < input.length() &&  is_white_space(input[current])) current++;
    if (current >= input.length()) return new Token(Token::END);
    char c = input[current];
    first = current;

    if (isdigit(c)) {
        current++;
        bool is_float = false; // verifico si es un decimal
        bool is_int = false; // para verificar si es un numero natural, entero, etc
        bool has_f = false; // para verificar si hay 'f' al final

        // parte entera
        while (current < input.length() && isdigit(input[current]))
            current++;        
        if (current < input.length() && input[current] == '.') {
            if (current + 1 < input.length() && input[current + 1] == '.') {
                is_int = true;
            } else {
                is_float = true;
                current++;
                while (current < input.length() && isdigit(input[current]))
                current++;
            }
        } else {
            is_int = true;
        }

        if (is_float && current < input.length() && input[current] == 'f') {
            has_f = true;
            current++;
            token = new Token(Token::DECIMAL, input, first, current - first);
            token->has_f = has_f;
        } 
        else if (is_float) {
            token = new Token(Token::DECIMAL, input, first, current - first);
        }
        // número entero pero tiene "f" al final, se trata como float pero el número es un NUM
        else if (is_int && current < input.length() && input[current] == 'f'){
            has_f = true;
            current++;
            token = new Token(Token::NUM, input, first, current - first);
            token->has_f = has_f;
        }
        else {
            // Si no tiene punto ni 'f', es un número entero
            token = new Token(Token::NUM, input, first, current - first);
        }
    }

    else if (isalpha(c) || c == '_') {
        current++;
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_')) 
            current++;
        string lex = input.substr(first, current - first);
        if (lex == "var") return new Token(Token::VAR);
        else if (lex == "Int" || lex == "Float" || lex == "Boolean" || lex == "Unit") {
            token = new Token(Token::TYPE, lex, 0, lex.length());
        } else if (lex == "print") {
            token = new Token(Token::PRINT, lex, 0, lex.length());
        } else if (lex == "println") {
            token = new Token(Token::PRINTLN, lex, 0, lex.length());
        } else if (lex == "if") {
            token = new Token(Token::IF, lex, 0, lex.length());
        } else if (lex == "else") {
            token = new Token(Token::ELSE, lex, 0, lex.length());
        } else if (lex == "else if") {
            token = new Token(Token::ELSE_IF, lex, 0, lex.length());
        } else if (lex == "while") {
            token = new Token(Token::WHILE, lex, 0, lex.length());
        } else if (lex == "do") {
            token = new Token(Token::DO, lex, 0, lex.length());
        } else if (lex == "for") {
            token = new Token(Token::FOR, lex, 0, lex.length());
        } else if (lex == "in") {
            token = new Token(Token::IN, lex, 0, lex.length());
        } else if (lex == "until") {
            token = new Token(Token::UNTIL, lex, 0, lex.length());
        } else if (lex == "downTo") {
            token = new Token(Token::DOWNTO, lex, 0, lex.length());
        } else if (lex == "step") {
            token = new Token(Token::STEP, lex, 0, lex.length());
        } else if (lex == "true") {
            token = new Token(Token::TRUE, lex, 0, lex.length());
        } else if (lex == "false") {
            token = new Token(Token::FALSE, lex, 0, lex.length());
        } else if (lex == "and") {
            token = new Token(Token::AND, lex, 0, lex.length());
        } else if (lex == "or") {
            token = new Token(Token::OR, lex, 0, lex.length());
        } else if (lex == "fun") {
            token = new Token(Token::FUN, lex, 0, lex.length());
        } else if (lex == "return") {
            token = new Token(Token::RETURN, lex, 0, lex.length());
        } else if (lex == "arrayOf") {
            token = new Token(Token::ARRAY_OF, lex, 0, lex.length());
        } else if (lex == "Array") {
            token = new Token(Token::ARRAY, lex, 0, lex.length());
        } else if (lex == "indices") {
            token = new Token(Token::INDICES, lex, 0, lex.length());
        } else if (lex == "size") {
            token = new Token(Token::SIZE, lex, 0, lex.length());
        } else {
            token = new Token(Token::ID, lex, 0, lex.length());
        }
    }

    else if (strchr(":+-*/()=;,<>{}.![]", c)) {
        switch(c) {
            case '+':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::PLUS_ASSIGN, "+=", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::PLUS, c);
                }
                break;
            case '-':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::MINUS_ASSIGN, "-=", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::MINUS, c);
                }
                break;
            case '*': token = new Token(Token::MUL, c);
                break;
            case '/': token = new Token(Token::DIV, c);
                break;
            case ',': token = new Token(Token::COMA, c); break;
            case '(': token = new Token(Token::PI, c); break;
            case ')': token = new Token(Token::PD, c); break;
            case '{': token = new Token(Token::LLI, c); break;
            case '}': token = new Token(Token::LLD, c); break;
            case ':': token = new Token(Token::TWO_POINTS, c); break;
            case '!': 
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::NOT_EQ, "!=", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::NOT, c);
                }
                break;
            case '=':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::EQ, "==", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::ASSIGN, c);
                }
                break;            
            case '<':
                current++;
                if (current < input.length() && input[current] == '=') {
                    current++;
                    return new Token(Token::LE);
                }
                return new Token(Token::LT);
            case '>':
                current++;
                if (current < input.length() && input[current] == '=') {
                    current++;
                    return new Token(Token::GE);
                }
                return new Token(Token::GT);
            case '[':
                current++;
                return new Token(Token::CI);
            case ']':
                current++;
                return new Token(Token::CD);
            case '.':
                if (current + 1 < input.length() && input[current + 1] == '.') {
                
                    size_t start = current;
                    current += 2;
                    return new Token(Token::DOTDOT, input, start, 2);
                } else {
                    token = new Token(Token::DOT, c);
                }
                break;
            case ';': token = new Token(Token::PC, c); break;
            default:
                cout << "No debería llegar acá" << endl;
                token = new Token(Token::ERR, c);
        }
        current++;
    }
    else {
        token = new Token(Token::ERR, c);
        current++;
    }
    return token;
}

void Scanner::reset() {
    first = 0;
    current = 0;
}

Scanner::~Scanner() { }

void test_scanner(Scanner* scanner) {
    Token* current;
    cout << "Iniciando Scanner:" << endl<< endl;
    while ((current = scanner->nextToken())->type != Token::END) {
        if (current->type == Token::ERR) {
            cout << "Error en scanner - carácter inválido: " << current->text << endl;
            break;
        } else {
            cout << *current << endl;
        }
        delete current;
    }
    cout << "TOKEN(END)" << endl;
    delete current;
}