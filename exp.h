#ifndef EXP_H
#define EXP_H

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <string>

class Visitor;
class FunDecList;

using namespace std;
enum BinaryOp { AND_OP, OR_OP, PLUS_OP, MINUS_OP, MUL_OP, DIV_OP, LT_OP, LE_OP, GT_OP, GE_OP, EQ_OP, PLUSPLUS_OP, MINUSMINUS_OP, NOT_EQ_OP}; // falta el NOT_EQUAL
enum UnaryOp { NOT_OP, UMINUS_OP };
enum RangeType { RANGE_DOTDOT, RANGE_UNTIL, RANGE_DOWNTO };

enum Type { 
    UNDEFINED_TYPE = 0,
    INT_TYPE = 1,
    FLOAT_TYPE = 2, 
    BOOLEAN_TYPE = 3,
    UNIT_TYPE = 4,
    RANGE_TYPE = 5
};

string typeToString(Type type);
Type stringToType(const string& typeStr);

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

// Operaciones unarias con su operador ! 
class UnaryExp : public Exp {
public:
    Exp* e;
    UnaryOp op;
    UnaryExp(Exp* e, UnaryOp op);
    int accept(Visitor* visitor);
    ~UnaryExp();
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

class RangeExp : public Exp {
public:
    Exp* start;
    Exp* end;
    RangeType type;
    Exp* step;
    RangeExp(Exp* start, Exp* end, RangeType type = RANGE_DOTDOT, Exp* step = nullptr);
    int accept(Visitor* visitor);
    ~RangeExp();
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

class PlusAssignStatement : public Stm {
public:
    string id;
    Exp* rhs;
    int accept(Visitor* visitor);
    PlusAssignStatement(string id, Exp* e);
    ~PlusAssignStatement();
};

class MinusAssignStatement : public Stm {
public:
    string id;
    Exp* rhs;
    int accept(Visitor* visitor);
    MinusAssignStatement(string id, Exp* e);
    ~MinusAssignStatement();
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
    string type; // Int, Float, Bool
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
    VarDecList* vardecs;
    FunDecList* fundecs;
    Program(VarDecList* vardecs, FunDecList* fundecs);
    int accept(Visitor* visitor);
    ~Program();
};

// Función
class FunDec {
public:
    std::string nombre;
    std::list<std::string> parametros;
    std::list<std::string> tipos_parametros;
    std::string tipo_retorno;
    Body* cuerpo;
    FunDec(std::string nombre, std::list<std::string> parametros, 
           std::list<std::string> tipos_parametros, std::string tipo_retorno, Body* cuerpo);
    int accept(Visitor* visitor);
    ~FunDec();
};

class FunDecList {
public:
    std::list<FunDec*> fundecs;
    FunDecList();
    void add(FunDec* funcdec);
    int accept(Visitor* visitor);
    ~FunDecList();
};

// Llamada a función como expresión
class FCallExp : public Exp {
public:
    std::string nombre;
    std::list<Exp*> argumentos;
    FCallExp(std::string nombre, std::list<Exp*> argumentos);
    int accept(Visitor* visitor);
    ~FCallExp();
};

// Llamada a función como statement
class FCallStm : public Stm {
public:
    std::string nombre;
    std::list<Exp*> argumentos;
    FCallStm(std::string nombre, std::list<Exp*> argumentos);
    int accept(Visitor* visitor);
    ~FCallStm();
};

// Return statement
class ReturnStatement : public Stm {
public:
    Exp* e;
    ReturnStatement(Exp* e);
    int accept(Visitor* visitor);
    ~ReturnStatement();
};
// If - While - For 
class IfStatement : public Stm {
public:
    Exp* condition;
    Body* then;
    std::vector<std::pair<Exp*, Body*>> elseifs; //Condicion + Que hacer si se cumple dicha condicion
    Body* els;
    IfStatement(Exp* condition, Body* then);
    void addElseIf(Exp* condition, Body* body);
    void setElse(Body* body);
    int accept(Visitor* visitor);
    ~IfStatement();
};

class WhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStatement(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~WhileStatement();
};

class ForStatement : public Stm {
public:
    std::string id;
    std::string type;
    Exp* range;       
    Body* body;
    ForStatement(std::string id, std::string type, Exp* range, Body* body);
    int accept(Visitor* visitor) override;
    ~ForStatement();
};

// Expresión para declaración de arrays
class ArrayExp : public Exp {
public:
    string type;  // Int o Float
    vector<Exp*> elements;
    ArrayExp(const string& type, vector<Exp*>& elements);
    int accept(Visitor* visitor);
    ~ArrayExp();
};

// Expresión para acceso a elementos del array
class ArrayAccessExp : public Exp {
public:
    Exp* array;
    Exp* index;
    ArrayAccessExp(Exp* array, Exp* index);
    int accept(Visitor* visitor);
    ~ArrayAccessExp();
};

// Expresión para métodos de array como .size, .indices, etc.
enum class ArrayMethodType { SIZE, INDICES };
class ArrayMethodExp : public Exp {
public:
    Exp* array;
    ArrayMethodType method;
    ArrayMethodExp(Exp* array, ArrayMethodType method);
    int accept(Visitor* visitor);
    ~ArrayMethodExp();
};

#endif // EXP_H