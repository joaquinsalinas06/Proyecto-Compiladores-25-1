#ifndef EXP_H
#define EXP_H

#include <string>
#include <unordered_map>
#include <list>
#include <string>
#include "visitor.h"
using namespace std;
enum BinaryOp { PLUS_OP, MINUS_OP, MUL_OP, DIV_OP, LT_OP, LE_OP, EQ_OP }; // falta el NOT_EQUAL

class Body;

// // Clase abstracta para todas la expresiones
class Exp {
public:
    bool has_f = false;  // Indicador para saber si tiene 'f'
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};

// Operaciones binarias con su operador * - + /
class BinaryExp : public Exp {
public:
    Exp *left, *right;
    string type;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    int accept(Visitor* visitor);
    ~BinaryExp();
};

// Valor numérico entero dentro de la expresión
class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v);
    int accept(Visitor* visitor);
    ~NumberExp();
};

// Valor numérico decimal (flotante) dentro de la expresión
class DecimalExp : public Exp {
public:
    float value;  // Usamos float para almacenar el valor decimal
    DecimalExp(float v);
    int accept(Visitor* visitor);
    ~DecimalExp();
};


class BoolExp : public Exp {
public:
    int value;
    BoolExp(bool v);
    int accept(Visitor* visitor);
    ~BoolExp();
};

class IdentifierExp : public Exp {
public:
    string name;
    IdentifierExp(const std::string& n);
    int accept(Visitor* visitor);
    ~IdentifierExp();
};

class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class AssignStatement : public Stm {
public:
    string id;
    Exp* rhs;
    AssignStatement(string id, Exp* e);
    int accept(Visitor* visitor);
    ~AssignStatement();
};

class PrintStatement : public Stm {
public:
    Exp* e;
    bool newline;
    PrintStatement(Exp* e, bool newline);
    int accept(Visitor* visitor);
    ~PrintStatement();
};

class VarDec {
public:
    string id;
    string type; // Int o Float
    Exp* value; // nullptr si no se inicializa
    VarDec(std::string id, std::string type, Exp* value = nullptr);
    int accept(Visitor* visitor);
    ~VarDec();
};

class VarDecList {
public:
    std::list<VarDec*> decls;
    VarDecList();
    void add(VarDec* decl);
    int accept(Visitor* visitor);
    ~VarDecList();
};

class StatementList {
public:
    list<Stm*> stms;
    StatementList();
    void add(Stm* stm);
    int accept(Visitor* visitor);
    ~StatementList();
};


class Body{
public:
    VarDecList* vardecs;
    StatementList* slist;
    Body(VarDecList* vardecs, StatementList* stms);
    int accept(Visitor* visitor);
    ~Body();
};

class Program {
public:
    Body* body;
    Program(Body* body);
    ~Program();
};

// // If - While - For 
// // class IfStatement : public Stm {
// // public:
// //     Exp* condition;
// //     Body* then;
// //     Body* els;
// //     IfStatement(Exp* condition, Body* then, Body* els);
// //     int accept(Visitor* visitor);
// //     ~IfStatement();
// // };


// // class WhileStatement : public Stm {
// // public:
// //     Exp* condition;
// //     Body* b;
// //     WhileStatement(Exp* condition, Body* b);
// //     int accept(Visitor* visitor);
// //     ~WhileStatement();
// // };

// //
// // class ForStatement : public Stm {
// // public:
// //     std::string id;
// //     Exp *start, *limit, *step;
// //     Body *body;
// //     ForStatement(const std::string& id, Exp* start, Exp* limit, Exp* step, Body* body);
// //     int accept(Visitor* visitor) override;
// //     ~ForStatement();
// // };

#endif // EXP_H