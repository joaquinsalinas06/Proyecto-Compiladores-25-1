#include <iostream>
#include "exp.h"
using namespace std;

Exp::~Exp() {}
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {
    if (op == PLUS_OP || op == MINUS_OP || op == MUL_OP || op == DIV_OP) {
        type = "int";
    } else {
        type = "bool";
    }
    // Falta agregar el float (Verificar si el resultado es decimal)
}

BinaryExp::~BinaryExp() { 
    delete left;
    delete right;
}

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

// +=
PlusAssignStatement::PlusAssignStatement(string id, Exp* e) : id(id), rhs(e) {}
PlusAssignStatement::~PlusAssignStatement() {
    delete rhs;
}
// -=
MinusAssignStatement::MinusAssignStatement(string id, Exp* e) : id(id), rhs(e) {}
MinusAssignStatement::~MinusAssignStatement() {
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

RangeExp::RangeExp(Exp* start, Exp* end): start(start), end(end) {}
RangeExp::~RangeExp() { delete start; delete end; }

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

// FunDec
FunDec::FunDec(string nombre, list<string> parametros, list<string> tipos_parametros, 
               string tipo_retorno, Body* cuerpo) 
    : nombre(nombre), parametros(parametros), tipos_parametros(tipos_parametros), 
      tipo_retorno(tipo_retorno), cuerpo(cuerpo) {}

FunDec::~FunDec() {
    delete cuerpo;
}

// FunDecList
FunDecList::FunDecList() {}

void FunDecList::add(FunDec* fundec) {
    fundecs.push_back(fundec);
}

FunDecList::~FunDecList() {
    for (auto fundec : fundecs) {
        delete fundec;
    }
}

// FCallExp  
FCallExp::FCallExp(string nombre, list<Exp*> argumentos) 
    : nombre(nombre), argumentos(argumentos) {}

FCallExp::~FCallExp() {
    for (auto arg : argumentos) {
        delete arg;
    }
}

// FCallStm
FCallStm::FCallStm(string nombre, list<Exp*> argumentos) 
    : nombre(nombre), argumentos(argumentos) {}

FCallStm::~FCallStm() {
    for (auto arg : argumentos) {
        delete arg;
    }
}

// ReturnStatement
ReturnStatement::ReturnStatement(Exp* e) : e(e) {}

ReturnStatement::~ReturnStatement() {
    if (e) delete e;
}