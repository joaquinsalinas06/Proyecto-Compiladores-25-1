#include <iostream>
#include "exp.h"
#include "visitor.h"
using namespace std;

string typeToString(Type type) {
    switch(type) {
        case UNDEFINED_TYPE: return "Undefined";
        case INT_TYPE: return "Int";
        case FLOAT_TYPE: return "Float";
        case BOOLEAN_TYPE: return "Boolean";
        case UNIT_TYPE: return "Unit";
        case RANGE_TYPE: return "Range";
        default: return "Unknown";
    }
}

Type stringToType(const string& typeStr) {
    if (typeStr == "Int") return INT_TYPE;
    else if (typeStr == "Float") return FLOAT_TYPE;
    else if (typeStr == "Boolean") return BOOLEAN_TYPE;
    else if (typeStr == "Unit") return UNIT_TYPE;
    else if (typeStr == "Range") return RANGE_TYPE;
    else return UNDEFINED_TYPE;
}

Exp::~Exp() {}
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {
    // Si es suma, resta, multiplicación o división, puede ser Int o Float
    if (op == PLUS_OP || op == MINUS_OP || op == MUL_OP || op == DIV_OP) {
        if (left->has_f || right->has_f) {
            type = "Float";
            has_f = true; // Marcamos que tiene un float
        } else {
            type = "Int";
        }
    } else {
        // Para el resto, lo trato como booleano
        type = "Boolean";
    }
}

BinaryExp::~BinaryExp() { 
    delete left;
    delete right;
}

// El resto de los constructores y destructores son bastante directos
UnaryExp::UnaryExp(Exp* e, UnaryOp op): e(e), op(op) {}
UnaryExp::~UnaryExp() { 
    delete e; 
}

NumberExp::NumberExp(int v):value(v) {}
NumberExp::~NumberExp() { }


DecimalExp::DecimalExp(float v):value(v) {}
DecimalExp::~DecimalExp() { }
    

IdentifierExp::IdentifierExp(const string& n): name(n) {}
IdentifierExp::~IdentifierExp() { }

BoolExp::BoolExp(bool v):value(v) {}
BoolExp::~BoolExp() { }

AssignStatement::AssignStatement(string id, Exp* e): id(id), rhs(e) {}
AssignStatement::~AssignStatement() {
    delete rhs;
}

// += y -=, igual que el assign normal pero con su propio operador
PlusAssignStatement::PlusAssignStatement(string id, Exp* e) : id(id), rhs(e) {}
PlusAssignStatement::~PlusAssignStatement() {
    delete rhs;
}
MinusAssignStatement::MinusAssignStatement(string id, Exp* e) : id(id), rhs(e) {}
MinusAssignStatement::~MinusAssignStatement() {
    delete rhs;
}

int ArrayAssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

ArrayAssignStatement::~ArrayAssignStatement() {
    delete lhs;
    delete rhs;
}

PrintStatement::PrintStatement(Exp* e, bool newline) : e(e), newline(newline) {}
PrintStatement::~PrintStatement() {
    delete e;
}

IfStatement::IfStatement(Exp* condition, Body* then): condition(condition), then(then), els(nullptr) {}

void IfStatement::addElseIf(Exp* condition, Body* body) {
    elseifs.push_back(std::make_pair(condition, body));
}

void IfStatement::setElse(Body* body) {
    els = body;
}

IfStatement::~IfStatement() { 
    delete condition;
    delete then;
    for (auto& elseif : elseifs) {
        delete elseif.first;
        delete elseif.second;
    }
    delete els;
}

WhileStatement::WhileStatement(Exp* condition, Body* body) : condition(condition), b(body) {}
WhileStatement::~WhileStatement() {}

ForStatement::ForStatement(std::string id, std::string type, Exp* range, Body* body) : id(id), type(type), range(range), body(body) {}
    
ForStatement::~ForStatement() { delete range; delete body; }

RangeExp::RangeExp(Exp* start, Exp* end, RangeType type, Exp* step): start(start), end(end), type(type), step(step) {}
RangeExp::~RangeExp() { delete start; delete end; if (step) delete step; }

VarDec::VarDec(string id, string t, Exp* val): id(id), type(t), value(val) {}
VarDec::~VarDec() { delete value; }

VarDecList::VarDecList(): decls() {}
void VarDecList::add(VarDec* v) {
    decls.push_back(v);
}

VarDecList::~VarDecList() {
    for (VarDec* decl : decls) {
        delete decl;
    }
}


StatementList::StatementList() {}
void StatementList::add(Stm* s) {
    stms.push_back(s);
}
StatementList::~StatementList() {
    for (auto s: stms) {
        delete s;
    }
}

Body::Body(VarDecList* v, StatementList* s): vardecs(v), slist(s) {}
Body::~Body() {
    delete vardecs;
    delete slist;
}

Program::Program(VarDecList* vardecs, FunDecList* fundecs): vardecs(vardecs), fundecs(fundecs) {}
Program::~Program() {
    delete vardecs;
    delete fundecs;
}

Stm::~Stm() {}

// Esta función la uso para convertir el enum de operadores a string, para imprimir o debuggear
string Exp::binopToChar(BinaryOp op) {
    string c = "";
    switch(op) {
        case PLUS_OP: c = "+"; break;
        case MINUS_OP: c = "-"; break;
        case PLUSPLUS_OP: c = "++"; break;
        case MINUSMINUS_OP: c = "--"; break;
        case AND_OP: c = "and"; break;
        case OR_OP: c = "or"; break;
        case MUL_OP: c = "*"; break;
        case DIV_OP: c = "/"; break;
        case LT_OP: c = "<"; break;
        case LE_OP: c = "<="; break;
        case GT_OP: c = ">"; break;
        case GE_OP: c = ">="; break;
        case EQ_OP: c = "=="; break;
        case NOT_EQ_OP: c ="!="; break;
        default: c = "$";
    }
    return c;
}

// FunDec y FunDecList, lo típico para manejar funciones
FunDec::FunDec(string nombre, list<string> parametros, list<string> tipos_parametros, 
               string tipo_retorno, Body* cuerpo) 
    : nombre(nombre), parametros(parametros), tipos_parametros(tipos_parametros), 
      tipo_retorno(tipo_retorno), cuerpo(cuerpo) {}

FunDec::~FunDec() {
    delete cuerpo;
}

FunDecList::FunDecList() {}

void FunDecList::add(FunDec* fundec) {
    fundecs.push_back(fundec);
}

FunDecList::~FunDecList() {
    for (auto fundec : fundecs) {
        delete fundec;
    }
}

// Llamadas a función, igual, nada raro
FCallExp::FCallExp(string nombre, list<Exp*> argumentos) 
    : nombre(nombre), argumentos(argumentos) {}

FCallExp::~FCallExp() {
    for (auto arg : argumentos) {
        delete arg;
    }
}

FCallStm::FCallStm(string nombre, list<Exp*> argumentos) 
    : nombre(nombre), argumentos(argumentos) {}

FCallStm::~FCallStm() {
    for (auto arg : argumentos) {
        delete arg;
    }
}

// ReturnStatement, igual de directo
ReturnStatement::ReturnStatement(Exp* e) : e(e) {}

ReturnStatement::~ReturnStatement() {
    if (e) delete e;
}

// Arrays: los constructores y destructores solo limpian los elementos
ArrayExp::ArrayExp(const string& type, vector<Exp*>& elements) : type(type), elements(elements) {}
ArrayExp::~ArrayExp() {
    for (auto element : elements) {
        delete element;
    }
}

ArrayAccessExp::ArrayAccessExp(Exp* array, Exp* index) : array(array), index(index) {}
ArrayAccessExp::~ArrayAccessExp() {
    delete array;
    delete index;
}

ArrayMethodExp::ArrayMethodExp(Exp* array, ArrayMethodType method) : array(array), method(method) {}
ArrayMethodExp::~ArrayMethodExp() {
    delete array;
}