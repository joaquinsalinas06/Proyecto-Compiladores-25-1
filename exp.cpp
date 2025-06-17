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

BinaryExp::~BinaryExp() { delete left;
    delete right;
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

PrintStatement::PrintStatement(Exp* e, bool newline) : e(e), newline(newline) {}
PrintStatement::~PrintStatement() {
    delete e;
}

IfStatement::IfStatement(Exp* condition, Body* then, Body* els): condition(condition), then(then), els(els) {}
IfStatement::~IfStatement() { }

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

Program::Program(Body* b): body(b) {}
Program::~Program() {
    delete body;
}

Stm::~Stm() {}
string Exp::binopToChar(BinaryOp op) {
    string  c;
    switch(op) {
        case PLUS_OP: c = "+"; break;
        case MINUS_OP: c = "-"; break;
        case MUL_OP: c = "*"; break;
        case DIV_OP: c = "/"; break;
        case LT_OP: c = "<"; break;
        case LE_OP: c = "<="; break;
        case EQ_OP: c = "=="; break;
        default: c = "$";
    }
    return c;
}