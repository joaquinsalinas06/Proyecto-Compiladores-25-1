#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "visitor.h"
#include "environment.h"
#include <unordered_map>
#include <string>

class FunDec;

class TypeChecker : public Visitor {
    Environment env;
    std::unordered_map<std::string, FunDec*> funciones;
    std::string retorno;
    
public:
    
    TypeChecker();
    
    void check(Program* program);
    
    // Métodos visit para expresiones
    int visit(BinaryExp* exp) override;
    int visit(UnaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    int visit(FCallExp* fcall) override;
    int visit(ArrayExp* exp) override;
    int visit(ArrayAccessExp* exp) override;
    int visit(ArrayMethodExp* exp) override;
    
    // Métodos visit para statements
    void visit(AssignStatement* stm) override;
    void visit(PlusAssignStatement* stm) override;
    void visit(MinusAssignStatement* stm) override;
    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override;
    void visit(WhileStatement* stm) override;
    void visit(ForStatement* stm) override;
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
    void visit(Program* program) override;
    void visit(FunDec* fundec) override;
    void visit(FunDecList* fundecs) override;
    void visit(FCallStm* fcall) override;
    void visit(ReturnStatement* retstm) override;
};

#endif // TYPECHECKER_H
