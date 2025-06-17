#ifndef VISITOR_H
#define VISITOR_H
#include "exp.h"
#include "environment.h"
#include <list>

class BinaryExp;
class NumberExp;
class DecimalExp;
class BoolExp;
class IdentifierExp;
class RangeExp;
class AssignStatement;
class PrintStatement;

class IfStatement;
class WhileStatement;
class ForStatement;

class VarDec;
class VarDecList;
class StatementList;
class Body;
class Program;

class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(DecimalExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(IdentifierExp* exp) = 0;
    virtual int visit(RangeExp* exp) = 0;
    virtual void visit(AssignStatement* stm) = 0;
    virtual void visit(PrintStatement* stm) = 0;

    virtual void visit(IfStatement* stm) = 0; // IfStatement
    virtual void visit(WhileStatement* stm) = 0; // WhileStatement
    virtual void visit(ForStatement* stm) = 0; // ForStatement
    virtual void visit(VarDec* stm) = 0;
    virtual void visit(VarDecList* stm) = 0;
    virtual void visit(StatementList* stm) = 0;
    virtual void visit(Body* b) = 0;
};

class PrintVisitor : public Visitor {
private:
    int indent = 0;  // Nivel actual de indentación
    void imprimirIndentacion();  // Método auxiliar para imprimir la indentación
public:
    void imprimir(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;

    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    void visit(AssignStatement* stm) override;
    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override; // IfStatement
    void visit(WhileStatement* stm) override; // WhileStatement
    void visit(ForStatement* stm) override; // ForStatement
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
};

class EVALVisitor : public Visitor {
    Environment env;
    int lastType;    // 1=int, 2=float, 3=bool
    int lastInt;     // último valor entero evaluado
    float lastFloat; // último valor float evaluado
public:
    void ejecutar(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    void visit(AssignStatement* stm) override;
    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override; // IfStatement
    void visit(WhileStatement* stm) override; // WhileStatement
    void visit(ForStatement* stm) override; // ForStatement
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
};

class TypeVisitor : public Visitor {
    Environment env;
public:
    void check(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
  
    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    void visit(AssignStatement* stm) override;
    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override;
    void visit(WhileStatement* stm) override;
    void visit(ForStatement* stm) override;
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
};

class CodeGenVisitor : public Visitor {
public:
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    void visit(AssignStatement* stm) override;
    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override; // IfStatement
    void visit(WhileStatement* stm) override; // WhileStatement
    void visit(ForStatement* stm) override; // ForStatement
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
};

#endif // VISITOR_H