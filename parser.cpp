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
            if (match(Token::TYPE)) {
                type = previous->text;
            } else if (match(Token::ARRAY_OF)) {
                if (!match(Token::LT)) {
                    cout << "Error: se esperaba '<' después de 'arrayOf' en tipo de variable." << endl;
                    exit(1);
                }
                if (match(Token::TYPE)) {
                    type = "arrayOf<" + previous->text + ">";
                } else {
                    cout << "Error: se esperaba un tipo después de '<' en tipo de variable." << endl;
                    exit(1);
                }
                if (!match(Token::GT)) {
                    cout << "Error: se esperaba '>' después del tipo en tipo de variable." << endl;
                    exit(1);
                }
            } else if (check(Token::ID) && current->text == "arrayOf") {
                advance();
                if (!match(Token::LT)) {
                    cout << "Error: se esperaba '<' después de 'arrayOf' en tipo de variable." << endl;
                    exit(1);
                }
                if (match(Token::TYPE)) {
                    type = "arrayOf<" + previous->text + ">";
                } else {
                    cout << "Error: se esperaba un tipo después de '<' en tipo de variable." << endl;
                    exit(1);
                }
                if (!match(Token::GT)) {
                    cout << "Error: se esperaba '>' después del tipo en tipo de variable." << endl;
                    exit(1);
                }
            } else if (check(Token::ARRAY)) {
                advance();
                if (!match(Token::LT)) {
                    cout << "Error: se esperaba '<' después de 'Array' en tipo de variable." << endl;
                    exit(1);
                }
                if (match(Token::TYPE)) {
                    type = "Array<" + previous->text + ">";
                } else {
                    cout << "Error: se esperaba un tipo después de '<' en tipo de variable." << endl;
                    exit(1);
                }
                if (!match(Token::GT)) {
                    cout << "Error: se esperaba '>' después del tipo en tipo de variable." << endl;
                    exit(1);
                }
            } else {
                cout << "Error: tipo de variable desconocido." << endl;
                exit(1);
            }
        }

        Exp* val = nullptr;
        if (match(Token::ASSIGN)) {
            val = parseAExp(); // Una asignación puede tener cualquier tipo de expresión, incluyendo lógicas
        }
        // Si el tipo está vacío y el valor es un ArrayExp, deducir el tipo
        if (type.empty() && val != nullptr) {
            ArrayExp* arr = dynamic_cast<ArrayExp*>(val);
            if (arr) {
                if (arr->type == "Int") type = "Array<Int>";
                else if (arr->type == "Float") type = "Array<Float>";
                else if (arr->type == "Boolean") type = "Array<Boolean>";
            }
        }

        if (type.empty() && val == nullptr) {
            cout << "Error: se debe especificar el tipo de la variable o inicializarla." << endl;
            exit(1);
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

    while (!isAtEnd() && (check(Token::PC) || check(Token::ID) || check(Token::PRINT) || check(Token::PRINTLN) || check(Token::IF) || check(Token::WHILE) || check(Token::FOR) || check(Token::RETURN))) {
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
            if (check(Token::PC)) advance(); // Avanza si hay punto y coma, pero no lo requiere
        }        // Si el siguiente token es inicio de sentencia
        else if (check(Token::ID) || check(Token::PRINT) || check(Token::PRINTLN) || check(Token::IF) || check(Token::WHILE) || check(Token::FOR) || check(Token::RETURN)) {
            Stm* stm = parseStatement();
            if (stm != nullptr) sl->add(stm);
            if (check(Token::PC)) advance(); // Avanza si hay punto y coma, pero no lo requiere
        } else if (check(Token::END) || check(Token::LLD)) {
            break;
        } else if (check(Token::PC)) {
            advance();
        } else {
            cout << "Error: token inesperado en el cuerpo del programa: " << *current << endl; exit(1);
        }
    }

    return new Body(vdl, sl);
}

Program* Parser::parseProgram() {
    VarDecList* vardecs = new VarDecList();
    FunDecList* fundecs = new FunDecList();

    while (!isAtEnd()) {
        if (check(Token::VAR)) {
            VarDec* varDec = parseVarDec();
            if (varDec != nullptr) vardecs->add(varDec);
            match(Token::PC); 
        } else if (check(Token::FUN)) {
            FunDec* funDec = parseFunDec();
            if (funDec != nullptr) fundecs->add(funDec);
            // Avanza si el token actual es un cierre de bloque (LLD) después de la función
            if (check(Token::LLD)) advance();
        } else if (check(Token::END)) {
            break;
        } else {
            cout << "Error: se esperaba 'var', 'fun' o final del archivo, pero se encontró: " << *current << endl;
            exit(1);
        }
    }

    return new Program(vardecs, fundecs);
}


Stm* Parser::parseStatement() {
    Stm* s = NULL;
    Exp* e = NULL;

    // Check for return statement first
    if (check(Token::RETURN)) {
        ReturnStatement* returnStmt = parseReturnStatement();
        return returnStmt;
    }

    else if (check(Token::ID)) {
        string lex = current->text;
        advance();
        Exp* lhs = new IdentifierExp(lex);
        while (check(Token::CI)) {
            lhs = parseArrayAccess(lhs);
        }
        if (match(Token::ASSIGN)) {
            e = parseAExp();
            IdentifierExp* idLhs = dynamic_cast<IdentifierExp*>(lhs);
            if (idLhs) {
                s = new AssignStatement(lex, e);
            } else {
                s = new ArrayAssignStatement(lhs, e);
            }
        } else if (match(Token::PLUS_ASSIGN)) {
            e = parseAExp();
            s = new PlusAssignStatement(lex, e);
        } else if (match(Token::MINUS_ASSIGN)) {
            e = parseAExp();
            s = new MinusAssignStatement(lex, e);
        } else if (match(Token::PI)) {
            list<Exp*>* args = new list<Exp*>();
            if (!check(Token::PD)) {
                args->push_back(parseAExp());
                while (match(Token::COMA)) {
                    args->push_back(parseAExp());
                }
            }
            if (!match(Token::PD)) {
                cout << "Error: se esperaba ')' después de los argumentos de la función." << endl;
                exit(1);
            }
            s = new FCallStm(lex, *args);
        } else {
            cout << "Error: se esperaba '=', '+=', '-=', o '(' después del identificador o acceso a array." << endl;
            exit(1);
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

        IfStatement* ifStmt = new IfStatement(condition, thenBody);
        
        while (match(Token::ELSE)) {
            if (check(Token::IF)) {
                advance();
                if (!match(Token::PI)) {
                    cout << "Error: se esperaba '(' después de 'if'" << endl;
                    exit(1);
                }
                Exp* elseIfCondition = parseAExp();
                if (!match(Token::PD)) {
                    cout << "Error: se esperaba ')' después de la condición del else if" << endl;
                    exit(1);
                }
                if (!match(Token::LLI)) {
                    cout << "Error: se esperaba '{' después de la condición del else if" << endl;
                    exit(1);
                }
                Body* elseIfBody = parseBody();
                if (!match(Token::LLD)) {
                    cout << "Error: se esperaba '}' después del cuerpo del else if" << endl;
                    exit(1);
                }
                ifStmt->addElseIf(elseIfCondition, elseIfBody);
            } else {
                if (!match(Token::LLI)) {
                    cout << "Error: se esperaba '{' después de 'else'" << endl;
                    exit(1);
                }
                Body* elseBody = parseBody();
                if (!match(Token::LLD)) {
                    cout << "Error: se esperaba '}' después del cuerpo del else" << endl;
                    exit(1);
                }
                ifStmt->setElse(elseBody);
                break;
            }
        }
        
        s = ifStmt;
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

        if (match(Token::TYPE)) {
            varType = previous->text;
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
        cout << "Error: Se esperaba un identificador, 'print', 'if', 'while', 'for', o 'return', pero se encontró: " << *current << endl;
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

// Operadores < <= > >= ==
Exp* Parser::parseCExp(){
    Exp* left = parseExpression(); // Los operandos de comparación son expresiones aritméticas
    if (match(Token::LT) || match(Token::LE) || match(Token::GT) || match(Token::GE) || match(Token::EQ) || match(Token::NOT_EQ)) {
        BinaryOp op;
        if (previous->type == Token::LT){
            op = LT_OP;
        }
        else if (previous->type == Token::LE){
            op = LE_OP;
        }
        else if (previous->type == Token::GT){
            op = GT_OP;
        }
        else if (previous->type == Token::GE){
            op = GE_OP;
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

Exp* Parser::parseArrayExpression() {
    // Esperamos arrayOf<Type>(elementos...)
    if (!match(Token::ARRAY_OF)) exit(1);
    
    // Parsear el tipo genérico
    string type;
    if (!match(Token::LT)) exit(1);
    if (match(Token::TYPE)) {
        type = previous->text;
    } else {
        cout << "Error: se esperaba un tipo después de '<'" << endl;
        exit(1);
    }
    if (!match(Token::GT)) exit(1);

    // Parsear los elementos
    if (!match(Token::PI)) exit(1);
    vector<Exp*> elements;
    
    // Parsear la lista de elementos
    while (true) {
        Exp* element = parseExpression();
        if (!element) break;
        elements.push_back(element);
        if (!match(Token::COMA)) break;
    }
    
    if (!match(Token::PD))
    {
        cout << "Error: se esperaba ')'" << endl;
        exit(1);
    }
    return new ArrayExp(type, elements);
}

Exp* Parser::parseArrayAccess(Exp* array) {
    if (!match(Token::CI)) return array;
    Exp* index = parseExpression();
    if (!index || !match(Token::CD))
    {
        cout << "Error: se esperaba un índice entre corchetes '[]' después del nombre del array." << endl;
        exit(1);
    }
    return new ArrayAccessExp(array, index);
}

Exp* Parser::parseFactor() {
    // Array literal
    if (match(Token::ARRAY_OF)) {
        // Verificar si tiene tipos genéricos
        if (check(Token::LT)) {
            if (!match(Token::LT)) { 
                cout << "Error: Se esperaba '<' después de 'arrayOf'." << endl;
                exit(1);
            }
            if (!match(Token::TYPE)) {
                cout << "Error: Se esperaba un tipo (Int, Float, Boolean) dentro de '<>'." << endl;
                exit(1);
            }
            string array_type = previous->text;

            if (!match(Token::GT)) { 
                cout << "Error: Se esperaba '>' después del tipo." << endl;
                exit(1);
            }
            if (!match(Token::PI)) {
                cout << "Error: Se esperaba '(' después de 'arrayOf<Type>'." << endl;
                exit(1);
            }

            vector<Exp*> elements;
            if (!check(Token::PD)) { 
                do {
                    elements.push_back(parseAExp()); 
                } while (match(Token::COMA));
            }

            if (!match(Token::PD)) {
                cout << "Error: Se esperaba ')' para cerrar la lista de elementos del array." << endl;
                exit(1);
            }
            
            return new ArrayExp(array_type, elements);
        } else {
            // Si no hay tipo explícito, NO debe procesarse como arrayOf
            cout << "Error: Se esperaba '<Tipo>' después de 'arrayOf'." << endl;
            exit(1);
        }
    }

    // Paréntesis
    if (match(Token::PI)) {
        Exp* e = parseAExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')'" << endl;
            exit(1);
        }
        return e;
    }

    // Literales numéricos
    if (match(Token::NUM)) {
        string numText = previous->text;
        int value = stoi(numText);
        NumberExp* numExp = new NumberExp(value);
        // Verificar si el token tiene 'f' al final
        if (previous->has_f || (numText.length() > 0 && numText.back() == 'f')) {
            numExp->has_f = true;
        }
        return numExp;
    }
    if (match(Token::DECIMAL)) {
        string decText = previous->text;
        float value = stof(decText);
        DecimalExp* decExp = new DecimalExp(value);
        // Los decimales siempre se consideran float
        if (previous->has_f || (decText.length() > 0 && decText.back() == 'f')) {
            decExp->has_f = true;
        }
        return decExp;
    }

    // Booleanos
    if (match(Token::TRUE)) return new BoolExp(true);
    if (match(Token::FALSE)) return new BoolExp(false);

    // Identificadores o llamada a función
    if (match(Token::ID)) {
        string name = previous->text;
        
        if (check(Token::PI)) {
             advance();
             list<Exp*> args;
             if (!check(Token::PD)) {
                 args.push_back(parseAExp());
                 while (match(Token::COMA)) {
                     args.push_back(parseAExp());
                 }
             }
             if (!match(Token::PD)) {
                 cout << "Error: se esperaba ')' en llamada a función." << endl;
                 exit(1);
             }
             return new FCallExp(name, args);
        }

        // Verificar si es arrayOf sin < >
        if (name == "arrayOf" && check(Token::PI)) {
            cout << "Error: Se esperaba '<Tipo>' después de 'arrayOf'." << endl;
            exit(1);
        }
        
        // Si no es una llamada a función, es un identificador
        Exp* factor = new IdentifierExp(name);
        
        // Métodos de array (size, indices)
        if (match(Token::DOT)) {
            if (match(Token::INDICES)) {
                return new ArrayMethodExp(factor, ArrayMethodType::INDICES);
            } else if (match(Token::SIZE)) {
                return new ArrayMethodExp(factor, ArrayMethodType::SIZE);
            }
        }
        
        // Acceso a array con corchetes
        if (check(Token::CI)) {
            return parseArrayAccess(factor);
        }

        return factor;
    }

    cout << "Error: factor inesperado: " << *current << endl;
    exit(1);
    return nullptr;
}


Exp* Parser::parseRangeExpression() {
    Exp* start = parseAExp(); // Un rango puede empezar con cualquier expresión
    
    RangeType rangeType = RANGE_DOTDOT;
    Exp* end = nullptr;
    Exp* step = nullptr;
    
    if (match(Token::DOTDOT)) {
        rangeType = RANGE_DOTDOT;
        end = parseAExp();
    } else if (match(Token::UNTIL)) {
        rangeType = RANGE_UNTIL;
        end = parseAExp();
    } else if (match(Token::DOWNTO)) {
        rangeType = RANGE_DOWNTO;
        end = parseAExp();
    } else {
        // Si no hay operador de rango, devuelvo la expresión inicial
        return start;
    }
    
    // verifico si hay un paso (step)
    if (match(Token::STEP)) {
        step = parseAExp();
    }
    
    return new RangeExp(start, end, rangeType, step);
}
FunDec* Parser::parseFunDec() {
    if (!match(Token::FUN)) {
        // Si no se encuentra 'fun', no es una declaración de función
        cout << "Error: se esperaba 'fun' al inicio de la declaración de función." << endl;
        exit(1);
    }

    string name;
    if (!match(Token::ID)) {
        cout << "Error: se esperaba nombre de función después de 'fun'." << endl;
        exit(1);
    }
    name = previous->text;

    if (!match(Token::PI)) {
        cout << "Error: se esperaba '(' después del nombre de función." << endl;
        exit(1);
    }

    list<string> paramNames;
    list<string> paramTypes;
    if (!check(Token::PD)) {
        if (!match(Token::ID)) {
            cout << "Error: se esperaba nombre de parámetro." << endl;
            exit(1);
        }
        string paramName = previous->text;

        if (!match(Token::TWO_POINTS)) {
            cout << "Error: se esperaba ':' después del nombre del parámetro." << endl;
            exit(1);
        }

        string paramType;
        if (match(Token::TYPE)) {
            paramType = previous->text;
        } else {
            cout << "Error: tipo de parámetro desconocido." << endl;
            exit(1);
        }

        paramNames.push_back(paramName);
        paramTypes.push_back(paramType);

        while (match(Token::COMA)) {
            if (!match(Token::ID)) {
                cout << "Error: se esperaba nombre de parámetro después de ','." << endl;
                exit(1);
            }
            paramName = previous->text;

            if (!match(Token::TWO_POINTS)) {
                cout << "Error: se esperaba ':' después del nombre del parámetro." << endl;
                exit(1);
            }

            if (match(Token::TYPE)) {
                paramType = previous->text;
            } else {
                cout << "Error: tipo de parámetro desconocido." << endl;
                exit(1);
            }

            paramNames.push_back(paramName);
            paramTypes.push_back(paramType);
        }
    }

    if (!match(Token::PD)) {
        cout << "Error: se esperaba ')' después de los parámetros." << endl;
        exit(1);
    }

    string returnType = "Unit";
    if (match(Token::TWO_POINTS)) {
        if (match(Token::TYPE)) {
            returnType = previous->text;
        } else {
            cout << "Error: tipo de retorno desconocido." << endl;
            exit(1);
        }
    }

    if (!match(Token::LLI)) {
        cout << "Error: se esperaba '{' después de la declaración de función." << endl;
        exit(1);
    }

    Body* body = parseBody();

    if (!match(Token::LLD)) {
        cout << "Error: se esperaba '}' después del cuerpo de la función." << endl;
        exit(1);
    }

    return new FunDec(name, paramNames, paramTypes, returnType, body);
}

FunDecList* Parser::parseFunDecList() {
    FunDecList* fdl = new FunDecList();
    FunDec* aux = parseFunDec();
    while (aux != nullptr) {
        fdl->add(aux);
        aux = parseFunDec();
    }
    return fdl;
}

Exp* Parser::parseFCallExp() {
    if (!check(Token::ID)) {
        cout << "Error: se esperaba un identificador para la llamada a función." << endl;
        exit(1);
    }
    string name = current->text;
    advance();

    if (!match(Token::PI)) {
        return new IdentifierExp(name);
    }

    list<Exp*>* args = new list<Exp*>();
    
    if (!check(Token::PD)) {
        args->push_back(parseAExp());
        
        while (match(Token::COMA)) {
            args->push_back(parseAExp());
        }
    }

    if (!match(Token::PD)) {
        cout << "Error: se esperaba ')' después de los argumentos de la función." << endl;
        exit(1);
    }

    return new FCallExp(name, *args);
}

ReturnStatement* Parser::parseReturnStatement() {
    if (!match(Token::RETURN)) {
        cout << "Error: se esperaba 'return' al inicio de la declaración de retorno." << endl;
        exit(1);
    }

    Exp* value = nullptr;
    if (!check(Token::PC) && !check(Token::LLD) && !check(Token::END)) {
        value = parseAExp();
    }

    return new ReturnStatement(value);
}