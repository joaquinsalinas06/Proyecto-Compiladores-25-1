#ifndef CODEGEN_H
#define CODEGEN_H

#include "visitor.h"
#include <fstream>
#include <unordered_map>

class Exp;
class Stm;
class Program;
class Body;

class GenCodeVisitor : public Visitor {
public:
    std::ofstream& out;
    std::unordered_map<std::string, bool> memoriaGlobal;
    std::unordered_map<std::string, int> memoria;
    std::unordered_map<std::string, std::string> varTypes;  // Segun la variable guarda su tipo
    std::unordered_map<std::string, double> floatConstants; // Para almacenar constantes flotantes
    std::vector<VarDec*> globalVarDecs; // Variables globales
    int offset;
    int labelCounter;
    int constantCounter; // Contador de constantes
    bool entornoFuncion;
    std::string currentFunction;
    bool isIntegerResult; // Evaluar si el resultado es un entero o float
    
    GenCodeVisitor(std::ofstream& out_stream) : out(out_stream), offset(-8), labelCounter(0), constantCounter(0), entornoFuncion(false), isIntegerResult(true) {}

    void generar(Program* program);
    void collectConstants(Program* program);
    void collectConstantsFromExp(Exp* exp);
    void collectConstantsFromBody(Body* body);
    void collectConstantsFromStmt(Stm* stmt);
    
    bool needsFloatOperation(BinaryExp* exp);
    std::string getFloatConstant(double value);

    // Métodos visit para expresiones
    int visit(BinaryExp* exp) override;
    int visit(UnaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(DecimalExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(RangeExp* exp) override;
    int visit(FCallExp* fcall) override;

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

#endif // CODEGEN_H
