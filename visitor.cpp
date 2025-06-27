#include <iostream>
#include "exp.h"
#include "visitor.h"
#include <iomanip>
#include <unordered_map>
#include <sstream>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////
///// Visitors
/////////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int UnaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int DecimalExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int RangeExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// Para la declaración de suma +=
int PlusAssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
// Para la declaración de resta -=
int MinusAssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int StatementList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int ForStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int Body::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int Program::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FunDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FunDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FCallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FCallStm::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int ReturnStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
// Array
int ArrayExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int ArrayAccessExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int ArrayMethodExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
///// PrintVisitor
/////////////////////////////////////////////////////////////////////////////////////
int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(UnaryExp* exp) {
    cout << "!";
    exp->e->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;  
    if (exp->has_f) {    
        cout << "f";
    }
    return 0; 
}

int PrintVisitor::visit(DecimalExp* exp) {
    cout << fixed << setprecision(1) << exp->value << "f";  
    return 0; 
}

int PrintVisitor::visit(BoolExp* exp) {
    (exp->value)? cout << "true":  cout << "false";
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return 0;
}

int PrintVisitor::visit(RangeExp* exp) {
    exp->start->accept(this);
    
    switch(exp->type) {
        case RANGE_DOTDOT:
            cout << "..";
            break;
        case RANGE_UNTIL:
            cout << " until ";
            break;
        case RANGE_DOWNTO:
            cout << " downTo ";
            break;
    }
    
    exp->end->accept(this);
    
    if (exp->step != nullptr) {
        cout << " step ";
        exp->step->accept(this);
    }
    
    return 0;
}

void PrintVisitor::visit(AssignStatement* stm) {
    imprimirIndentacion();
    cout << stm->id << " = ";
    stm->rhs->accept(this);
}

// Para la declaración de suma +=
void PrintVisitor::visit(PlusAssignStatement* stm) {
    imprimirIndentacion();
    cout << stm->id << " += ";
    stm->rhs->accept(this);
}

// Para la declaración de resta -=
void PrintVisitor::visit(MinusAssignStatement* stm) {
    imprimirIndentacion();
    cout << stm->id << " -= ";
    stm->rhs->accept(this); 
}

void PrintVisitor::visit(PrintStatement* stm) {
    imprimirIndentacion();  
    if(stm->newline==true){
        cout << "println(";
        stm->e->accept(this);
        cout << ")" ;
    } else if(stm->newline==false) {
        cout << "print(";
        stm->e->accept(this);
        cout << ")";
    }
}

void PrintVisitor::imprimir(Program* program){
    indent = 0;
    program->accept(this);
    cout << endl;
};

// función auxiliar para imprimir la indentación
void PrintVisitor::imprimirIndentacion() {
    for (int i = 0; i < indent; i++) 
        cout << "    ";
}

// if
void PrintVisitor::visit(IfStatement* stm) {
    imprimirIndentacion();
    cout << "if (";
    stm->condition->accept(this);
    cout << ") {" << endl;
    indent++;
    stm->then->accept(this);
    indent--;
    imprimirIndentacion();
    cout << "}";
    
    for (const auto& elseif : stm->elseifs) {
        cout << " else if (";
        elseif.first->accept(this);
        cout << ") {" << endl;
        indent++;
        elseif.second->accept(this);
        indent--;
        imprimirIndentacion();
        cout << "}";
    }
    
    if (stm->els != nullptr) {
        cout << " else {" << endl;
        indent++;
        stm->els->accept(this);
        indent--;
        imprimirIndentacion();
        cout << "}";
    }
}
// while
void PrintVisitor::visit(WhileStatement* stm) {
    imprimirIndentacion();
    cout << "while (";
    stm->condition->accept(this);
    cout << ") {" << endl;
    indent++;
    stm->b->accept(this);
    indent--;
    imprimirIndentacion();
    cout << "}";
}
// for
void PrintVisitor::visit(ForStatement* stm) {
    imprimirIndentacion();
    cout << "for (";
    if (!stm->type.empty()) {
        cout << stm->type << " ";
    }
    cout << stm->id << " in ";
    stm->range->accept(this);
    cout << ") {" << endl;
    indent++;
    stm->body->accept(this);
    indent--;
    imprimirIndentacion();
    cout << "}";
}

void PrintVisitor::visit(VarDec* stm){
    imprimirIndentacion();
    cout << "var " << stm->id;
    if (stm->value != nullptr) {
        cout << " = ";
        stm->value->accept(this);
    } else {
        cout << " : " << stm->type;
    }
    cout << endl;
}


void PrintVisitor::visit(VarDecList* stm){
    for(auto i: stm->decls){
        i->accept(this);
    }
}

void PrintVisitor::visit(StatementList* stm) {
    for(auto i: stm->stms){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Body* stm){
    stm->vardecs->accept(this);
    stm->slist->accept(this);
}

void PrintVisitor::visit(Program* program) {
    // se imprime variables globales primero
    if (program->vardecs && program->vardecs->decls.size() > 0) {
        program->vardecs->accept(this);
    }
    // luego imprimo funciones
    if (program->fundecs && program->fundecs->fundecs.size() > 0) {
        program->fundecs->accept(this);
    }
}

void PrintVisitor::visit(FunDec* fundec) {
    imprimirIndentacion();
    cout << "fun " << fundec->nombre << "(";
    
    // imprimo parámetros con sus tipos
    if (!fundec->parametros.empty()) {
        auto it_param = fundec->parametros.begin();
        auto it_tipo = fundec->tipos_parametros.begin();
        
        bool first = true;
        while (it_param != fundec->parametros.end() && it_tipo != fundec->tipos_parametros.end()) {
            if (!first) cout << ", ";
            cout << *it_param << ": " << *it_tipo;
            ++it_param;
            ++it_tipo;
            first = false;
        }
    }
    
    cout << "): " << fundec->tipo_retorno << " {" << endl;
    indent++;
    if (fundec->cuerpo) {
        fundec->cuerpo->accept(this);
    }
    indent--;
    imprimirIndentacion();
    cout << "}" << endl;
}

void PrintVisitor::visit(FunDecList* fundecs) {
    for (auto fundec : fundecs->fundecs) {
        fundec->accept(this);
    }
}

int PrintVisitor::visit(FCallExp* fcall) {
    cout << fcall->nombre << "(";
    if (!fcall->argumentos.empty()) {
        auto it = fcall->argumentos.begin();
        bool first = true;
        while (it != fcall->argumentos.end()) {
            if (!first) cout << ", ";
            (*it)->accept(this);
            ++it;
            first = false;
        }
    }
    cout << ")";
    return 0;
}

void PrintVisitor::visit(FCallStm* fcall) {
    imprimirIndentacion();
    cout << fcall->nombre << "(";
    if (!fcall->argumentos.empty()) {
        auto it = fcall->argumentos.begin();
        bool first = true;
        while (it != fcall->argumentos.end()) {
            if (!first) cout << ", ";
            (*it)->accept(this);
            ++it;
            first = false;
        }
    }
    cout << ")";
}

void PrintVisitor::visit(ReturnStatement* retstm) {
    imprimirIndentacion();
    cout << "return";
    if (retstm->e) {
        cout << " ";
        retstm->e->accept(this);
    }
}

int PrintVisitor::visit(ArrayExp* exp) {
    cout << "arrayOf<" << exp->type << ">(";
    for (size_t i = 0; i < exp->elements.size(); ++i) {
        exp->elements[i]->accept(this);
        if (i + 1 < exp->elements.size()) cout << ", ";
    }
    cout << ")";
    return 0;
}

int PrintVisitor::visit(ArrayAccessExp* exp) {
    exp->array->accept(this);
    cout << "[";
    exp->index->accept(this);
    cout << "]";
    return 0;
}

int PrintVisitor::visit(ArrayMethodExp* exp) {
    exp->array->accept(this);
    switch (exp->method) {
        case ArrayMethodType::SIZE:
            cout << ".size";
            break;
        case ArrayMethodType::INDICES:
            cout << ".indices";
            break;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
///// EVALVisitor
/////////////////////////////////////////////////////////////////////////////////////

static int evalTypeFromEnv(Environment& env, string name) {
    string strtype = env.lookup_type(name);
    if (strtype == "Int") return 1;
    if (strtype == "Float") return 2;
    if (strtype == "Boolean") return 3;
    return -1;
}

int EVALVisitor::visit(NumberExp* exp) {
    lastType = 1;
    lastInt = exp->value;
    return lastType;
}
int EVALVisitor::visit(DecimalExp* exp) {
    lastType = 2;
    lastFloat = exp->value;
    return lastType;
}

int EVALVisitor::visit(BoolExp* exp) {
    lastType = 3;
    lastInt = exp->value ? 1 : 0;
    return lastType;
}

int EVALVisitor::visit(IdentifierExp* exp) {
    int t = evalTypeFromEnv(env, exp->name);  // obtengo el tipo de la variable
    lastType = t;  // Guardamos el tipo de la variable

    if (t == 1) {
        lastInt = env.lookup(exp->name);  // recupero el valor entero
    } else if (t == 2) {
        lastFloat = env.lookup_float(exp->name);  // recupero el valor flotante
    } else if (t == 3) {
        lastInt = env.lookup_bool(exp->name) ? 1 : 0;  // asigno 1 si es true, 0 si es false
    }
    return t;
}

int EVALVisitor::visit(RangeExp* exp) {
    int start_t = exp->start->accept(this);
    int start_val = lastInt;
    int end_t = exp->end->accept(this);
    int end_val = lastInt;
    
    lastType = 4;
    lastInt = start_val;
    lastFloat = (float)end_val;
    return lastType;
}

int EVALVisitor::visit(BinaryExp* exp) {
    int lt = exp->left->accept(this);
    int leftType = lastType;
    int ri = exp->right->accept(this);
    int rightType = lastType;

    // si hay un float, convertimos ambos lados a float y operamos en float
    if (leftType == 2 || rightType == 2) {
        float lv, rv;
        // obtengo valores float de ambas ramas
        exp->left->accept(this); // cast a float si es necesario
        lv = (lastType == 2) ? lastFloat : (float)lastInt;        
        exp->right->accept(this); // cast a float si es necesario
        rv = (lastType == 2) ? lastFloat : (float)lastInt;
        lastType = 2; // El resultado es float

        switch(exp->op) {
            case PLUS_OP:  lastFloat = lv + rv; break;
            case MINUS_OP: lastFloat = lv - rv; break;
            case MUL_OP:   lastFloat = lv * rv; break;
            case DIV_OP:   lastFloat = lv / rv; break;
            case LT_OP:    lastType = 3; lastInt = (lv < rv); break;    
            case LE_OP:    lastType = 3; lastInt = (lv <= rv); break;   
            case GT_OP:    lastType = 3; lastInt = (lv > rv); break;    
            case GE_OP:    lastType = 3; lastInt = (lv >= rv); break;   
            case EQ_OP:    lastType = 3; lastInt = (lv == rv); break;   
            case NOT_EQ_OP: lastType = 3; lastInt = (lv != rv); break;  
            case AND_OP:   // Evaluar AND lógico para flotantes
                lastType = 3; // El resultado es booleano
                lastInt = (lv != 0.0f && rv != 0.0f) ? 1 : 0;
                break;
            case OR_OP:    // Evaluar OR lógico para flotantes
                lastType = 3; // El resultado es booleano
                lastInt = (lv != 0.0f || rv != 0.0f) ? 1 : 0;
                break;
        }
        return lastType;
    }
    exp->left->accept(this);
    int lv = lastInt;
    exp->right->accept(this); 
    int rv = lastInt;

    switch(exp->op) {
        case PLUS_OP:  lastType = 1; lastInt = lv + rv; break;
        case MINUS_OP: lastType = 1; lastInt = lv - rv; break;
        case MUL_OP:   lastType = 1; lastInt = lv * rv; break;
        case DIV_OP:   lastType = 1; lastInt = lv / rv; break;
        case LT_OP:    lastType = 3; lastInt = (lv < rv); break;    
        case LE_OP:    lastType = 3; lastInt = (lv <= rv); break;   
        case GT_OP:    lastType = 3; lastInt = (lv > rv); break;    
        case GE_OP:    lastType = 3; lastInt = (lv >= rv); break;   
        case EQ_OP:    lastType = 3; lastInt = (lv == rv); break;   
        case NOT_EQ_OP: lastType = 3; lastInt = (lv != rv); break;  
        case AND_OP:   // Evaluar AND lógico para enteros/booleanos
            lastType = 3; // El resultado es booleano
            lastInt = (lv != 0 && rv != 0) ? 1 : 0;
            break;
        case OR_OP:    // Evaluar OR lógico para enteros/booleanos
            lastType = 3; // El resultado es booleano
            lastInt = (lv != 0 || rv != 0) ? 1 : 0;
            break;
    }
    return lastType;
}

int EVALVisitor::visit(UnaryExp* exp) {
    exp->e->accept(this);
    lastType = 3;
    lastInt = !lastInt;
    return lastType;
}

void EVALVisitor::visit(AssignStatement* stm) {
    int evaluated_type = stm->rhs->accept(this); // Evalúa el valor de la derecha
    string declared_type = env.lookup_type(stm->id); // Obtiene el tipo declarado de la variable

    if (declared_type == "Int") {
        env.update(stm->id, (evaluated_type == 2) ? static_cast<int>(lastFloat) : lastInt);
    } else if (declared_type == "Float") {
        env.update(stm->id, (evaluated_type == 1) ? static_cast<float>(lastInt) : lastFloat);
    } else if (declared_type == "Boolean") {
        env.update(stm->id, (bool)lastInt);
    } else {
        cout << "Error: Tipo de variable no soportado o variable no declarada: " << stm->id << endl;
        exit(1);
    }
}

void EVALVisitor::visit(PlusAssignStatement* stm) {
    int t = stm->rhs->accept(this);
    if (t == 2) {
        float currentValue = env.lookup_float(stm->id);
        env.update(stm->id, currentValue + lastFloat);
    } else {
        int currentValue = env.lookup(stm->id);
        env.update(stm->id, currentValue + lastInt);
    }
}

void EVALVisitor::visit(MinusAssignStatement* stm) {
    int t = stm->rhs->accept(this);
    if (t == 2) {
        float currentValue = env.lookup_float(stm->id);
        env.update(stm->id, currentValue - lastFloat);
    } else {
        int currentValue = env.lookup(stm->id);
        env.update(stm->id, currentValue - lastInt);
    }
}

void EVALVisitor::visit(PrintStatement* stm) {
    int t = stm->e->accept(this); // Evalúa la expresión
    if (t == 2) { 
        if (lastFloat == static_cast<int>(lastFloat)) {
            cout << static_cast<int>(lastFloat); // Imprime como entero sin decimales
        } else {
            cout << fixed << setprecision(1) << lastFloat; // Imprime con decimales
        }
    } else if (t == 1) { 
        cout << lastInt;
    } else if (t == 3) { 
        cout << (lastInt ? "true" : "false");
    } else if (t == 4) { 
        cout << lastInt << ".." << (int)lastFloat;
    }
    if (stm->newline) cout << endl;
}

void EVALVisitor::ejecutar(Program* program){
    lastType = 1;
    lastInt = 0;
    lastFloat = 0.0f;
    program->accept(this);
}

void EVALVisitor::visit(VarDecList* stm){
    for(auto i: stm->decls){
        i->accept(this);
    }
}

void EVALVisitor::visit(FunDec* fundec) {
    funciones[fundec->nombre] = fundec; // Registrar la función en el conjunto de funciones
}

void EVALVisitor::visit(FunDecList* fundecs) {
    for (auto fundec : fundecs->fundecs) {
        fundec->accept(this);
    }
}

int EVALVisitor::visit(FCallExp* fcall) {
    auto it = funciones.find(fcall->nombre); //Buscamos todas las funciones registradas
    if (it == funciones.end()) { // Si no se encuentra la función, devolver valor por defecto
        lastType = 1;
        lastInt = 0;
        return lastType;
    }
    
    FunDec* funcion = it->second;
    
    env.add_level();
    
    if (fcall->argumentos.size() == funcion->parametros.size()) {
        auto arg_it = fcall->argumentos.begin();
        auto param_it = funcion->parametros.begin();
        auto tipo_it = funcion->tipos_parametros.begin();
        
        while (arg_it != fcall->argumentos.end() && param_it != funcion->parametros.end() && tipo_it != funcion->tipos_parametros.end()) {
            (*arg_it)->accept(this);
            
            string param = *param_it;
            string tipo = *tipo_it;
            
            if (lastType == 1) {
                env.add_var(param, lastInt, tipo);
            } else if (lastType == 2) {
                env.add_var(param, lastFloat, tipo);
            } else if (lastType == 3) {
                env.add_var(param, (bool)lastInt, tipo);
            }
            
            ++arg_it;
            ++param_it;
            ++tipo_it;
        }
    }
    
    bool oldReturnExecuted = returnExecuted;
    returnExecuted = false;
    
    if (funcion->cuerpo) {
        funcion->cuerpo->accept(this);
    }
    
    int resultType = lastType;
    int resultInt = lastInt;
    float resultFloat = lastFloat;
    
    returnExecuted = oldReturnExecuted;
    env.remove_level();
    
    lastType = resultType;
    lastInt = resultInt;
    lastFloat = resultFloat;
    
    return lastType;
}

void EVALVisitor::visit(FCallStm* fcall) {
    FCallExp* fexp = new FCallExp(fcall->nombre, fcall->argumentos);
    fexp->accept(this);
    delete fexp;
}

void EVALVisitor::visit(ReturnStatement* retstm) {
    if (retstm->e) {
        retstm->e->accept(this);
    }
    returnExecuted = true;
}

int EVALVisitor::visit(ArrayExp* exp) {
    // Detecta tipo de array y almacena en env
    if (exp->type == "Int") {
        std::vector<int> vals;
        for (auto e : exp->elements) {
            e->accept(this);
            vals.push_back(lastInt);
        }
        // El almacenamiento real se hace en la declaración de variable (VarDec)
        lastType = 5; // 5 = arrayOf<Int>
        lastArrayInt = vals;
        return lastType;
    } else if (exp->type == "Float") {
        std::vector<float> vals;
        for (auto e : exp->elements) {
            e->accept(this);
            vals.push_back(lastFloat);
        }
        lastType = 6; // 6 = arrayOf<Float>
        lastArrayFloat = vals;
        return lastType;
    }
    return 0;
}

int EVALVisitor::visit(ArrayAccessExp* exp) {
    // Primero evaluamos el índice
    int idxType = exp->index->accept(this);
    int idx = (idxType == 2) ? (int)lastFloat : lastInt;
    auto idExp = dynamic_cast<IdentifierExp*>(exp->array);
    if (idExp) {
        std::string arrType = env.lookup_type(idExp->name);
        if (arrType == "arrayOf<Int>") {
            auto& arr = env.lookup_array_int(idExp->name);
            if (idx >= 0 && idx < (int)arr.size()) {
                lastType = 1;
                lastInt = arr[idx];
                return lastType;
            } else {
                std::cerr << "Error: Índice fuera de rango en arrayOf<Int> '" << idExp->name << "' (idx=" << idx << ")\n";
            }
        } else if (arrType == "arrayOf<Float>") {
            auto& arr = env.lookup_array_float(idExp->name);
            if (idx >= 0 && idx < (int)arr.size()) {
                lastType = 2;
                lastFloat = arr[idx];
                return lastType;
            } else {
                std::cerr << "Error: Índice fuera de rango en arrayOf<Float> '" << idExp->name << "' (idx=" << idx << ")\n";
            }
        } else {
            std::cerr << "Error: Tipo de array no soportado para acceso: '" << arrType << "'\n";
        }
    } else {
        std::cerr << "Error: Acceso a array con expresión no identificador\n";
    }
    return 0;
}

int EVALVisitor::visit(ArrayMethodExp* exp) {
    auto idExp = dynamic_cast<IdentifierExp*>(exp->array);
    if (!idExp) return 0;
    std::string arrType = env.lookup_type(idExp->name);
    if (arrType == "arrayOf<Int>") {
        auto& arr = env.lookup_array_int(idExp->name);
        switch (exp->method) {
            case ArrayMethodType::SIZE:
                lastType = 1;
                lastInt = arr.size();
                return lastType;
            case ArrayMethodType::INDICES:
                lastType = 5;
                lastArrayInt.clear();
                for (int i = 0; i < arr.size(); ++i) lastArrayInt.push_back(i);
                return lastType;
        }
    } else if (arrType == "arrayOf<Float>") {
        auto& arr = env.lookup_array_float(idExp->name);
        switch (exp->method) {
            case ArrayMethodType::SIZE:
                lastType = 1;
                lastInt = arr.size();
                return lastType;
            case ArrayMethodType::INDICES:
                lastType = 5;
                lastArrayInt.clear();
                for (int i = 0; i < arr.size(); ++i) lastArrayInt.push_back(i);
                return lastType;
        }
    }
    return 0;
}

void EVALVisitor::visit(VarDec* stm) {
    if (stm->value) {
        int evaluated_type = stm->value->accept(this);
        std::string declared_type = stm->type;
        if (declared_type == "Int") {
            env.add_var(stm->id, (evaluated_type == 2) ? static_cast<int>(lastFloat) : lastInt, declared_type);
        } else if (declared_type == "Float") {
            env.add_var(stm->id, (evaluated_type == 1) ? static_cast<float>(lastInt) : lastFloat, declared_type);
        } else if (declared_type == "Boolean") {
            env.add_var(stm->id, (bool)lastInt, declared_type);
        } else if (declared_type == "arrayOf<Int>") {
            env.add_array(stm->id, lastArrayInt);
        } else if (declared_type == "arrayOf<Float>") {
            env.add_array(stm->id, lastArrayFloat);
        }
    } else {
        env.add_var(stm->id, stm->type);
    }
}

void EVALVisitor::visit(StatementList* stm){
    for(auto i: stm->stms){
        if (returnExecuted) break; // Si se ejecutó return, dejar de ejecutar statements
        i->accept(this);
    }
}

void EVALVisitor::visit(Body* b){
    env.add_level(); // tener en cuenta que agrego un nivel cada que inicio un programa    
    b->vardecs->accept(this);
    if (!returnExecuted) { // Solo ejecutar statements si no hay return
        b->slist->accept(this);
    }
    env.remove_level(); // quitar el nivel cuando termine de ejecutar el programa
}

void EVALVisitor::visit(Program* program) {
    lastType = 1;
    lastInt = 0;
    lastFloat = 0.0f;
    returnExecuted = false;
    
    env.add_level();
    
    if (program->vardecs) {
        program->vardecs->accept(this);
    }
    
    // Primero registrar todas las funciones
    if (program->fundecs) {
        program->fundecs->accept(this);
    }
    
    // Luego buscar y ejecutar main
    auto it = funciones.find("main");
    if (it != funciones.end()) {
        FunDec* mainFunc = it->second;
        if (mainFunc->cuerpo) {
            mainFunc->cuerpo->accept(this);
        }
    }
    
    env.remove_level();
}

void EVALVisitor::visit(IfStatement* stm) {
    // Evaluar la condición principal
    stm->condition->accept(this);
    bool condition_result = (lastType == 3) ? (lastInt != 0) : (lastInt != 0 || lastFloat != 0.0f);
    
    if (condition_result) {
        stm->then->accept(this);
    } else {
        // Evaluar else if's en orden
        bool executed = false;
        for (const auto& elseif : stm->elseifs) {
            elseif.first->accept(this);
            bool elseif_result = (lastType == 3) ? (lastInt != 0) : (lastInt != 0 || lastFloat != 0.0f);
            
            if (elseif_result) {
                elseif.second->accept(this);
                executed = true;
                break;
            }
        }
        
        if (!executed && stm->els != nullptr) {
            stm->els->accept(this);
        }
    }
}

void EVALVisitor::visit(WhileStatement* stm) {
    while (true) {
        stm->condition->accept(this);
        bool condition_result = (lastType == 3) ? (lastInt != 0) : (lastInt != 0 || lastFloat != 0.0f);
        
        if (!condition_result) break;
        stm->b->accept(this);
    }
}

void EVALVisitor::visit(ForStatement* stm) {
    std::vector<int> indices;
    if (RangeExp* range = dynamic_cast<RangeExp*>(stm->range)) {
        range->start->accept(this);
        int start_val = (lastType == 2) ? (int)lastFloat : lastInt;
        range->end->accept(this);
        int end_val = (lastType == 2) ? (int)lastFloat : lastInt;
        int step_val = 1;
        if (range->step) {
            range->step->accept(this);
            step_val = (lastType == 2) ? (int)lastFloat : lastInt;
        }
        for (int i = start_val; i < end_val; i += step_val) {
            indices.push_back(i);
        }
    } else {
        stm->range->accept(this);
        indices = lastArrayInt;
    }
    string var_type = stm->type.empty() ? "Int" : stm->type;
    for (int i : indices) {
        env.add_level();
        env.add_var(stm->id, i, var_type);
        stm->body->accept(this);
        env.remove_level();
        if (returnExecuted) break;
    }
}

