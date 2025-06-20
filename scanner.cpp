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
            // si es flotante pero no tiene 'f', generamos un error
            token = new Token(Token::ERR, input, first, current - first);
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

    else if (isalpha(c)) {
        current++;
        while (current < input.length() && isalnum(input[current]))
            current++;
        string word = input.substr(first, current - first);
        
        if (word == "Int") {
            token = new Token(Token::INT, word, 0, word.length());
        } else if (word == "Float") {
            token = new Token(Token::FLOAT, word, 0, word.length());
        } 
        
        else if (word == "Boolean") {
            token = new Token(Token::BOOLEAN, word, 0, word.length());
        } 
        
        else if (word == "print") {
            token = new Token(Token::PRINT, word, 0, word.length());
        } else if (word == "println") {
            token = new Token(Token::PRINTLN, word, 0, word.length());
        } else if (word == "if") {
            token = new Token(Token::IF, word, 0, word.length());
        } else if (word == "else") {
            token = new Token(Token::ELSE, word, 0, word.length());
        } else if (word == "else if") {
            token = new Token(Token::ELSE_IF, word, 0, word.length());
        } else if (word == "while") {
            token = new Token(Token::WHILE, word, 0, word.length());
        } else if (word == "do") {
            token = new Token(Token::DO, word, 0, word.length());
        } else if (word == "for") {
            token = new Token(Token::FOR, word, 0, word.length());
        } else if (word == "in") {
            token = new Token(Token::IN, word, 0, word.length());
        } else if (word == "var") {
            token = new Token(Token::VAR, word, 0, word.length());
        } else if (word == "true") {
            token = new Token(Token::TRUE, word, 0, word.length());
        } else if (word == "false") {
            token = new Token(Token::FALSE, word, 0, word.length());
        } else if (word == "and") {
            token = new Token(Token::AND, word, 0, word.length());
        } else if (word == "or") {
            token = new Token(Token::OR, word, 0, word.length());
        } else {
            token = new Token(Token::ID, word, 0, word.length());
        }
    }

    else if (strchr(":+-*/()=;,<{}.", c)) {
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
            case '=':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::EQ, "==", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::ASSIGN, c);
                }
                break;
            case '<':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::LE, "<=", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::LT, c);
                } break;
                case '.':
                if (current + 1 < input.length() && input[current + 1] == '.') {
                
                    size_t start = current;
                    current += 2;
                    return new Token(Token::DOTDOT, input, start, 2);
                } else {
                    token = new Token(Token::ERR, c);
                }           
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