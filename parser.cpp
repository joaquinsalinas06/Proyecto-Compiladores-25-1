#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h" // Asegúrate de que UnaryExp y NOT_OP estén definidos aquí
#include "parser.h"

using namespace std;

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous; // Libera la memoria del token anterior
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            cout << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

Parser::Parser(Scanner* sc):scanner(sc) {
    previous = NULL;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        cout << "Error en el primer token: " << current->text << endl;
        exit(1);
    }
}

// Listo
VarDec* Parser::parseVarDec() {
    VarDec* vd = NULL;
    if (match(Token::VAR)) {
        string id;
        if (!match(Token::ID)) {
            cout << "Error: se esperaba un identificador después de 'var'." << endl;
            exit(1);
        }
        id = previous->text;

        string type;
        if (match(Token::TWO_POINTS)) {
            if (match(Token::INT)){
                type = "Int";
            } else if (match(Token::BOOLEAN)) {
                type = "Boolean";
            } else if (match(Token::FLOAT)) {
                type = "Float";
            } else {
                cout << "Error: tipo de variable desconocido." << endl;
                exit(1);
            }
        }

        Exp* val = nullptr;
        if (match(Token::ASSIGN)) {
            val = parseAExp(); // Una asignación puede tener cualquier tipo de expresión, incluyendo lógicas
        }
        vd = new VarDec(id, type, val);
    }
    return vd;
}

VarDecList* Parser::parseVarDecList() {
    VarDecList* vdl = new VarDecList();
    VarDec* aux = parseVarDec(); // Se analiza la primera declaración de variable
    while (aux != NULL) {
        vdl->add(aux);
        if (match(Token::PC)) { // Después de una declaración, puede venir un punto y coma.
            aux = parseVarDec(); // Se analiza la siguiente declaración de variable
        } else {
            break;  // Si no hay más declaraciones, terminamos.
        }
    }
    return vdl;
}

StatementList* Parser::parseStatementList() {
    StatementList* sl = new StatementList();
    Stm* stmt = parseStatement();
    sl->add(stmt);

    while (!isAtEnd() && (check(Token::PC) || check(Token::ID) || check(Token::PRINT) || check(Token::PRINTLN) || check(Token::IF) || check(Token::WHILE) || check(Token::FOR))) {
        if (match(Token::PC)) {
            // Si se encuentra un punto y coma, avanza y continua
            continue;
        }
        stmt = parseStatement();
        sl->add(stmt);
    }
    return sl;
}

Body* Parser::parseBody() {
    VarDecList* vdl = new VarDecList();
    StatementList* sl = new StatementList();

    // Esto me permite tener intercalación de declaraciones de variables (VarDec) y sentencias (Stm)
    while (!isAtEnd()) {
        // Si el siguiente token es una declaración de variable
        if (check(Token::VAR)) {
            VarDec* varDec = parseVarDec();
            if (varDec != nullptr) vdl->add(varDec);
            // Si después de la declaración hay un ;, avanzo
            match(Token::PC);
        }        // Si el siguiente token es inicio de sentencia
        else if (check(Token::ID) || check(Token::PRINT) || check(Token::PRINTLN) || check(Token::IF) || check(Token::WHILE) || check(Token::FOR)) {
            Stm* stm = parseStatement();
            if (stm != nullptr) sl->add(stm);
            match(Token::PC);
        }        // termino
        else if (check(Token::END) || check(Token::LLD)) {
            break;
        }
        // Si el token es ;, avanzo
        else if (check(Token::PC)) {
            advance();
        }
        else { cout << "Error: token inesperado en el cuerpo del programa: " << *current << endl; exit(1); }
    }

    return new Body(vdl, sl);
}

Program* Parser::parseProgram() {
    Body* b = parseBody();
    return new Program(b);
}


Stm* Parser::parseStatement() {
    Stm* s = NULL;
    Exp* e = NULL;

    if (match(Token::ID)) {
        string lex = previous->text;
        if (match(Token::ASSIGN)) {
            e = parseAExp();
            s = new AssignStatement(lex, e);
        } else if (match(Token::PLUS_ASSIGN)) {
            e = parseAExp(); // Asignación con suma también puede ser cualquier expresión
            s = new PlusAssignStatement(lex, e);
        } else if (match(Token::MINUS_ASSIGN)) {
            e = parseAExp(); // Asignación con resta también puede ser cualquier expresión
            s = new MinusAssignStatement(lex, e);
        }
    } else if (check(Token::PRINT) || check(Token::PRINTLN)) {
        bool isPrintln = match(Token::PRINTLN);
        if (!isPrintln) match(Token::PRINT); // Si no era PRINTLN, debe ser PRINT

        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después de 'print/println'." << endl;
            exit(1);
        }
        e = parseAExp(); // print puede ser cualquier expresión, incluyendo lógicas
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        s = new PrintStatement(e, isPrintln); // true si PRINTLN, false si PRINT
    }
    // IFSTATEMENT - Kotlin
    else if (match(Token::IF)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de 'if'" << endl;
            exit(1);
        }
        Exp* condition = parseAExp(); // La condición de un IF puede ser una expresión lógica
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la condición del if" << endl;
            exit(1);
        }
        if (!match(Token::LLI)) {
            cout << "Error: se esperaba '{' después de la condición del if" << endl;
            exit(1);
        }
        Body* thenBody = parseBody();
        if (!match(Token::LLD)) {
            cout << "Error: se esperaba '}' después del cuerpo del if" << endl;
            exit(1);
        }

        Body* elseBody = nullptr;
        if (match(Token::ELSE)) {
            if (!match(Token::LLI)) {
                cout << "Error: se esperaba '{' después de 'else'" << endl;
                exit(1);
            }
            elseBody = parseBody();
            if (!match(Token::LLD)) {
                cout << "Error: se esperaba '}' después del cuerpo del else" << endl;
                exit(1);
            }
        }
        s = new IfStatement(condition, thenBody, elseBody);
    }
    // WHILESTATEMENT - Kotlin
    else if (match(Token::WHILE)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de 'while'" << endl;
            exit(1);
        }
        Exp* condition = parseAExp(); // La condición de un WHILE puede ser una expresión lógica
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la condición del while" << endl;
            exit(1);
        }
        if (!match(Token::LLI)) {
            cout << "Error: se esperaba '{' después de la condición del while" << endl;
            exit(1);
        }
        Body* whileBody = parseBody();
        if (!match(Token::LLD)) {
            cout << "Error: se esperaba '}' después del cuerpo del while" << endl;
            exit(1);
        }
        s = new WhileStatement(condition, whileBody);
    }    // FORSTATEMENT - Kotlin
    else if (match(Token::FOR)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de 'for'" << endl;
            exit(1);
        }

        string varType = "";
        string varId = "";

        // Los tipos INT y FLOAT son opcionales en la declaración de la variable en el for (en Kotlin)
        if (match(Token::INT)) {
            varType = "Int";
        } else if (match(Token::FLOAT)) {
            varType = "Float";
        }

        if (!match(Token::ID)) {
            cout << "Error: se esperaba identificador en el for" << endl;
            exit(1);
        }
        varId = previous->text;

        if (!match(Token::IN)) {
            cout << "Error: se esperaba 'in' después del identificador" << endl;
            exit(1);
        }

        Exp* rangeExp = parseRangeExpression(); // Un rango es una expresión

        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después del rango" << endl;
            exit(1);
        }
        if (!match(Token::LLI)) {
            cout << "Error: se esperaba '{' después del for" << endl;
            exit(1);
        }
        Body* forBody = parseBody();
        if (!match(Token::LLD)) {
            cout << "Error: se esperaba '}' después del cuerpo del for" << endl;
            exit(1);
        }
        s = new ForStatement(varId, varType, rangeExp, forBody);
    }

    else {
        cout << "Error: Se esperaba un identificador, 'print', 'if', 'while', o 'for', pero se encontró: " << *current << endl;
        exit(1);
    }
    return s;
}

// Operadores lógicos AND y OR
Exp* Parser::parseAExp() {
    Exp* left = parseCExp(); // Los operandos de AND/OR son expresiones de comparación
    while (match(Token::AND) || match(Token::OR)) {
        BinaryOp op = (previous->type == Token::AND) ? AND_OP : OR_OP;
        Exp* right = parseCExp(); // El lado derecho también debe ser una CExp
        left = new BinaryExp(left, right, op);
    }
    return left;
}

// Operadores < <= ==
Exp* Parser::parseCExp(){
    Exp* left = parseExpression(); // Los operandos de comparación son expresiones aritméticas
    if (match(Token::LT) || match(Token::LE) || match(Token::EQ) || match(Token::NOT_EQ)) {
        BinaryOp op;
        if (previous->type == Token::LT){
            op = LT_OP;
        }
        else if (previous->type == Token::LE){
            op = LE_OP;
        }
        else if (previous->type == Token::EQ){
            op = EQ_OP;
        }
        else if (previous->type == Token::NOT_EQ){
            op = NOT_EQ_OP;
        }
        Exp* right = parseExpression(); // El lado derecho también es una Expression
        left = new BinaryExp(left, right, op);
    }
    return left;
}

// Operadores + y -
Exp* Parser::parseExpression() {
    Exp* left = parseTerm();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op = (previous->type == Token::PLUS) ? PLUS_OP : MINUS_OP;
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

// Operadores * y /
Exp* Parser::parseTerm() {
    Exp* left = parseUnary(); 
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op = (previous->type == Token::MUL) ? MUL_OP : DIV_OP;
        Exp* right = parseUnary();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

// Operadores unarios: NOT
Exp* Parser::parseUnary() {
    if (match(Token::NOT)) {
        Exp* operand = parseUnary();
        return new UnaryExp(operand, NOT_OP);
    }
    return parseFactor();
}

Exp* Parser::parseFactor() {
    Exp* e;
    // Booleano
    if (match(Token::TRUE)){
        return new BoolExp(1);
    }else if (match(Token::FALSE)){
        return new BoolExp(0);
    } 
    // Numero entero
    else if (match(Token::NUM)) {
        NumberExp* numExp = new NumberExp(stoi(previous->text));
        numExp->has_f = previous->has_f;
        return numExp;
    }
    // Decimal
    else if (match(Token::DECIMAL)) {
        DecimalExp* decExp = new DecimalExp(stof(previous->text));
        return decExp;
    }
    // ID
    else if (match(Token::ID)) {
        return new IdentifierExp(previous->text);
    }
    // (Exp) - Las expresiones entre paréntesis tienen la máxima precedencia
    else if (match(Token::PI)){
        e = parseAExp();
        if (!match(Token::PD)){
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    cout << "Error: se esperaba un número, booleano, identificador o '('." << endl;
    exit(0);
}

Exp* Parser::parseRangeExpression() {
    Exp* start = parseAExp(); // Un rango puede empezar con cualquier expresión
    
    if (match(Token::DOTDOT)) {
        Exp* end = parseAExp(); // El final del rango también
        return new RangeExp(start, end);
    }
    
    return start;
}