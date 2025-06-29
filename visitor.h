#ifndef VISITOR_H
#define VISITOR_H
#include "environment.h"
#include <list>
#include <unordered_map>
#include <sstream>

// Forward declaration
enum BinaryOp;

class BinaryExp;
class UnaryExp;
class NumberExp;
class DecimalExp;
class BoolExp;
class IdentifierExp;
class RangeExp;
class AssignStatement;

class PlusAssignStatement;
class MinusAssignStatement;
class ArrayAssignStatement;

class PrintStatement;

class IfStatement;
class WhileStatement;
class ForStatement;

class VarDec;
class VarDecList;
class StatementList;
class Body;
class Program;
class FunDec;
class FunDecList;
class FCallExp;
class FCallStm;
class ReturnStatement;

class ArrayExp;
class ArrayAccessExp;
class ArrayMethodExp;

class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(UnaryExp* exp) = 0;

    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(DecimalExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(IdentifierExp* exp) = 0;
    virtual int visit(RangeExp* exp) = 0;
    virtual void visit(AssignStatement* stm) = 0;

    virtual void visit(PlusAssignStatement* stm) = 0; // PlusAssignStatement
    virtual void visit(MinusAssignStatement* stm) = 0; // MinusAssignStatement
    virtual void visit(ArrayAssignStatement* stm) = 0; // ArrayAssignStatement

    virtual void visit(PrintStatement* stm) = 0;

    virtual void visit(IfStatement* stm) = 0; // IfStatement
    virtual void visit(WhileStatement* stm) = 0; // WhileStatement
    virtual void visit(ForStatement* stm) = 0; // ForStatement
    virtual void visit(VarDec* stm) = 0;
    virtual void visit(VarDecList* stm) = 0;
    virtual void visit(StatementList* stm) = 0;
    virtual void visit(Body* b) = 0;
    virtual void visit(Program* program) = 0;
    virtual void visit(FunDec* fundec) = 0;
    virtual void visit(FunDecList* fundecs) = 0;
    virtual int visit(FCallExp* fcall) = 0;
    virtual void visit(FCallStm* fcall) = 0;
    virtual void visit(ReturnStatement* retstm) = 0;
    virtual int visit(ArrayExp* exp) = 0;
    virtual int visit(ArrayAccessExp* exp) = 0;
    virtual int visit(ArrayMethodExp* exp) = 0;
};

class PrintVisitor : public Visitor {
private:
    int indent = 0;  // Nivel actual de indentación
    void imprimirIndentacion();  // Método auxiliar para imprimir la indentación
    int ordenOp(BinaryOp op);  // Obtener precedencia de operador
    bool parentesis(BinaryExp* base, BinaryExp* child, bool isRightChild);  // Verificar si necesita paréntesis
public:
    void imprimir(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(UnaryExp* exp) override;
    int visit(NumberExp* exp) override;

    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    void visit(AssignStatement* stm) override;

    void visit(PlusAssignStatement* stm) override; // PlusAssignStatement
    void visit(MinusAssignStatement* stm) override; // MinusAssignStatement
    void visit(ArrayAssignStatement* stm) override; // ArrayAssignStatement

    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override; // IfStatement
    void visit(WhileStatement* stm) override; // WhileStatement
    void visit(ForStatement* stm) override; // ForStatement
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
    void visit(Program* program) override;
    void visit(FunDec* fundec) override;
    void visit(FunDecList* fundecs) override;
    int visit(FCallExp* fcall) override;
    void visit(FCallStm* fcall) override;
    void visit(ReturnStatement* retstm) override;
    int visit(ArrayExp* exp) override;
    int visit(ArrayAccessExp* exp) override;
    int visit(ArrayMethodExp* exp) override;
};

class EVALVisitor : public Visitor {
    Environment env;
    std::unordered_map<std::string, FunDec*> funciones;
    int lastType;    // 1=int, 2=float, 3=bool
    int lastInt;     // último valor entero evaluado
    float lastFloat; // último valor float evaluado
    bool returnExecuted; // Flag para controlar la ejecución de return
    std::vector<int> lastArrayInt;
    std::vector<float> lastArrayFloat;
    std::vector<bool> lastArrayBool;

public:
    void ejecutar(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(UnaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    void visit(AssignStatement* stm) override;

    void visit(PlusAssignStatement* stm) override; // PlusAssignStatement
    void visit(MinusAssignStatement* stm) override; // MinusAssignStatement
    void visit(ArrayAssignStatement* stm) override; // ArrayAssignStatement

    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override; // IfStatement
    void visit(WhileStatement* stm) override; // WhileStatement
    void visit(ForStatement* stm) override; // ForStatement
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
    void visit(Program* program) override;
    void visit(FunDec* fundec) override;
    void visit(FunDecList* fundecs) override;
    int visit(FCallExp* fcall) override;
    void visit(FCallStm* fcall) override;
    void visit(ReturnStatement* retstm) override;
    int visit(ArrayExp* exp) override;
    int visit(ArrayAccessExp* exp) override;
    int visit(ArrayMethodExp* exp) override;
};

class CodeGenVisitor : public Visitor {
public:
    int visit(BinaryExp* exp) override;
    int visit(UnaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    void visit(AssignStatement* stm) override;

    void visit(PlusAssignStatement* stm) override; // PlusAssignStatement
    void visit(MinusAssignStatement* stm) override; // MinusAssignStatement
    void visit(ArrayAssignStatement* stm) override; // ArrayAssignStatement

    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override; // IfStatement
    void visit(WhileStatement* stm) override; // WhileStatement
    void visit(ForStatement* stm) override; // ForStatement
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
    void visit(Program* program) override;
    void visit(FunDec* fundec) override;
    void visit(FunDecList* fundecs) override;
    int visit(FCallExp* fcall) override;
    void visit(FCallStm* fcall) override;
    void visit(ReturnStatement* retstm) override;

    int visit(ArrayExp* exp) override;
    int visit(ArrayAccessExp* exp) override;
    int visit(ArrayMethodExp* exp) override;
};

#endif // VISITOR_H