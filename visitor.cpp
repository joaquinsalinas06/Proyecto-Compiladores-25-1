#include <iostream>
#include "exp.h"
#include "visitor.h"
#include <iomanip>
#include <unordered_map>
#include <sstream>
using namespace std;


/////////////////////////////////////////////////////////////////////////////////////
///// Visutors
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
int StatementList::accept(Visitor* visitor) {
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
    cout << "..";
    exp->end->accept(this);
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
    cout << "var " << stm->id << " : " << stm->type;
    if (stm->value != nullptr) {
        cout << " = ";
        stm->value->accept(this);
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
    // Imprimir variables globales primero
    if (program->vardecs && program->vardecs->decls.size() > 0) {
        program->vardecs->accept(this);
    }
    // Luego imprimir funciones
    if (program->fundecs && program->fundecs->fundecs.size() > 0) {
        program->fundecs->accept(this);
    }
}

void PrintVisitor::visit(FunDec* fundec) {
    imprimirIndentacion();
    cout << "fun " << fundec->nombre << "(";
    
    // Imprimir parámetros con sus tipos
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
    int t = evalTypeFromEnv(env, exp->name);  // Obtener el tipo de la variable
    lastType = t;  // Guardamos el tipo de la variable

    if (t == 1) {
        lastInt = env.lookup(exp->name);  // Recuperamos el valor entero
    } else if (t == 2) {
        lastFloat = env.lookup_float(exp->name);  // Recuperamos el valor flotante
    } else if (t == 3) {
        lastInt = env.lookup_bool(exp->name) ? 1 : 0;  // Asignamos 1 si es true, 0 si es false
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

// int EVALVisitor::visit(BinaryExp* exp) {
//     int lt = exp->left->accept(this);
//     int leftType = lastType;
//     int ri = exp->right->accept(this);
//     int rightType = lastType;

//     // si hay float, convertimos ambos lados a float y operamos en float
//     if (leftType == 2 || rightType == 2) {
//         float lv, rv;
//         // obtener valores float de ambas ramas
//         exp->left->accept(this);
//         lv = (lastType == 2) ? lastFloat : (float)lastInt;
//         exp->right->accept(this);
//         rv = (lastType == 2) ? lastFloat : (float)lastInt;
//         lastType = 2;
//         switch(exp->op) {
//             case PLUS_OP: lastFloat = lv + rv; break;
//             case MINUS_OP: lastFloat = lv - rv; break;
//             case MUL_OP: lastFloat = lv * rv; break;
//             case DIV_OP: lastFloat = lv / rv; break;
//             case LT_OP:   lastType=3; lastInt=(lv < rv); break;
//             case LE_OP:   lastType=3; lastInt=(lv <= rv); break;
//             case EQ_OP:   lastType=3; lastInt=(lv == rv); break;
//             case AND_OP:   // Evaluar AND lógico
//                 lastType = 3;
//                 lastInt = (lv != 0 && rv != 0) ? 1 : 0;
//                 break;
//             case OR_OP:    // Evaluar OR lógico
//                 lastType = 3;
//                 lastInt = (lv != 0 || rv != 0) ? 1 : 0;
//                 break;
//         }
//         return lastType;
//     }
//     // si ambos son enteros o bool
//     exp->left->accept(this);
//     int lv = lastInt;
//     exp->right->accept(this);
//     int rv = lastInt;
//     switch(exp->op) {
//         case PLUS_OP: lastType=1; lastInt = lv + rv; break;
//         case MINUS_OP: lastType=1; lastInt = lv - rv; break;
//         case MUL_OP: lastType=1; lastInt = lv * rv; break;
//         case DIV_OP: lastType=1; lastInt = lv / rv; break;
//         case LT_OP:  lastType=3; lastInt = (lv < rv); break;
//         case LE_OP:  lastType=3; lastInt = (lv <= rv); break;
//         case EQ_OP:  lastType=3; lastInt = (lv == rv); break;
//     }
//     return lastType;
// }


int EVALVisitor::visit(BinaryExp* exp) {
    int lt = exp->left->accept(this);
    int leftType = lastType;
    int ri = exp->right->accept(this);
    int rightType = lastType;

    // Si hay un float, convertimos ambos lados a float y operamos en float
    if (leftType == 2 || rightType == 2) {
        float lv, rv;
        // Obtener valores float de ambas ramas
        exp->left->accept(this); // Re-evaluar para obtener el valor float
        lv = (lastType == 2) ? lastFloat : (float)lastInt;        
        exp->right->accept(this); // Re-evaluar para obtener el valor float
        rv = (lastType == 2) ? lastFloat : (float)lastInt;
        lastType = 2; // Por defecto, el resultado de una operación numérica mixta es float

        switch(exp->op) {
            case PLUS_OP:  lastFloat = lv + rv; break;
            case MINUS_OP: lastFloat = lv - rv; break;
            case MUL_OP:   lastFloat = lv * rv; break;
            case DIV_OP:   lastFloat = lv / rv; break;
            case LT_OP:    lastType = 3; lastInt = (lv < rv); break;    // Resultado booleano
            case LE_OP:    lastType = 3; lastInt = (lv <= rv); break;   // Resultado booleano
            case GT_OP:    lastType = 3; lastInt = (lv > rv); break;    // Resultado booleano
            case GE_OP:    lastType = 3; lastInt = (lv >= rv); break;   // Resultado booleano
            case EQ_OP:    lastType = 3; lastInt = (lv == rv); break;   // Resultado booleano
            case NOT_EQ_OP: lastType = 3; lastInt = (lv != rv); break;  // Resultado booleano
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
    exp->left->accept(this); // Obtener el valor int
    int lv = lastInt;
    exp->right->accept(this); // Obtener el valor int
    int rv = lastInt;

    switch(exp->op) {
        case PLUS_OP:  lastType = 1; lastInt = lv + rv; break;
        case MINUS_OP: lastType = 1; lastInt = lv - rv; break;
        case MUL_OP:   lastType = 1; lastInt = lv * rv; break;
        case DIV_OP:   lastType = 1; lastInt = lv / rv; break;
        case LT_OP:    lastType = 3; lastInt = (lv < rv); break;    // Resultado booleano
        case LE_OP:    lastType = 3; lastInt = (lv <= rv); break;   // Resultado booleano
        case GT_OP:    lastType = 3; lastInt = (lv > rv); break;    // Resultado booleano
        case GE_OP:    lastType = 3; lastInt = (lv >= rv); break;   // Resultado booleano
        case EQ_OP:    lastType = 3; lastInt = (lv == rv); break;   // Resultado booleano
        case NOT_EQ_OP: lastType = 3; lastInt = (lv != rv); break;  // Resultado booleano
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

void EVALVisitor::visit(VarDec* stm) {
    if (stm->value) {
        int evaluated_type = stm->value->accept(this);
        string declared_type = stm->type;

        if (declared_type == "Int") {
            env.add_var(stm->id, (evaluated_type == 2) ? static_cast<int>(lastFloat) : lastInt, declared_type);
        } else if (declared_type == "Float") {
            // **Aquí es donde 31 (de NumberExp) se convierte a 31.0f para almacenamiento**
            env.add_var(stm->id, (evaluated_type == 1) ? static_cast<float>(lastInt) : lastFloat, declared_type);
        } else if (declared_type == "Boolean") {
            env.add_var(stm->id, (bool)lastInt, declared_type);
        }
    } else {
        env.add_var(stm->id, stm->type);
    }
}

void EVALVisitor::visit(VarDecList* stm){
    for(auto i: stm->decls){
        i->accept(this);
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
    
    // Agregar nivel de entorno para el programa
    env.add_level();
    
    // Visitar las declaraciones de variables globales
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
    if (RangeExp* range = dynamic_cast<RangeExp*>(stm->range)) {
        range->start->accept(this);
        int start_val = (lastType == 2) ? (int)lastFloat : lastInt;
        
        range->end->accept(this);
        int end_val = (lastType == 2) ? (int)lastFloat : lastInt;
        
        string var_type = stm->type.empty() ? "Int" : stm->type;
        env.add_level();
        
        for (int i = start_val; i <= end_val; i++) {
            env.add_var(stm->id, i, var_type);
            stm->body->accept(this);
        }
        
        env.remove_level();
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

/////////////////////////////////////////////////////////////////////////////////////
///// TypeVisitor
/////////////////////////////////////////////////////////////////////////////////////

// FALTA TYPECHECKER
// //0 = undefined
// //1 = int
// //2 = bool
// //3 = float

void TypeVisitor::check(Program* program){
    // program->body->accept(this);
}

int TypeVisitor::visit(BinaryExp* exp) {
    // int result;
    // int v1 = exp->left->accept(this);
    // int v2 = exp->right->accept(this);
    // switch(exp->op) {
    //     case PLUS_OP: result = 1; break;
    //     case MINUS_OP: result = 1; break;
    //     case MUL_OP: result = 1; break;
    //     case DIV_OP: result = 1; break; // Evalúa -> Typechecker debe analizar que no se divida por 0
    //     case LT_OP: result = 2; break;
    //     case LE_OP: result = 2; break;
    //     case EQ_OP: result = 2; break;
    // }
    // return result;
    return 0;
}

int TypeVisitor::visit(UnaryExp* exp) {
    return 3;  // Tipo booleano
}

int TypeVisitor::visit(NumberExp* exp) {
    return 1;
}

int TypeVisitor::visit(DecimalExp* exp) {
    return 2;
}


int TypeVisitor::visit(BoolExp* exp) {
    return 3;
}

int TypeVisitor::visit(IdentifierExp* exp) {
    // if (!env.check(exp->name)){
    //     cout << "Variable no definida: "<< exp->name << endl;        
    //     exit(0);
    // }
    return 0;
}

int TypeVisitor::visit(RangeExp* exp) {
    // int start_type = exp->start->accept(this);
    // int end_type = exp->end->accept(this);
    // if (start_type != 1 || end_type != 1) {
    //     cout << "Error: Las expresiones tienen que estar dentro de un rango valido" << endl;
    //     exit(0);
    // }
    return 1; // Range returns int type
}

void TypeVisitor::visit(AssignStatement* stm) {
    // if (!env.check(stm->id)) {
    //     cout << "Variable no declarada: " << stm->id << endl;
    //     exit(0);
    // }

    // if (env.lookup_type(stm->id) == "int" and stm->rhs->accept(this) == 1) {
    // } else if (env.lookup_type(stm->id) == "float" and stm->rhs->accept(this) == 2) {
    // } else if (env.lookup_type(stm->id) == "bool" and stm->rhs->accept(this) == 3) {
    //     cout << "Error: No coinciden los tipos en la asignacion: " << stm->id << endl;
    //     exit(0);
    // }
}

// Para la declaración de suma +=
void TypeVisitor::visit(PlusAssignStatement* stm) {
    // if (!env.check(stm->id)) {
    //     cout << "Variable no declarada: " << stm->id << endl;
    //     exit(0);
    // }

    // if (env.lookup_type(stm->id) == "int" and stm->rhs->accept(this) == 1) {
    // } else if (env.lookup_type(stm->id) == "float" and stm->rhs->accept(this) == 2) {
    // } else if (env.lookup_type(stm->id) == "bool" and stm->rhs->accept(this) == 3) {
    //     cout << "Error: No coinciden los tipos en la asignacion: " << stm->id << endl;
    //     exit(0);
    // }
}

void TypeVisitor::visit(MinusAssignStatement* stm) {
    // if (!env.check(stm->id)) {
    //     cout << "Variable no declarada: " << stm->id << endl;
    //     exit(0);
    // }

    // if (env.lookup_type(stm->id) == "int" and stm->rhs->accept(this) == 1) {
    // } else if (env.lookup_type(stm->id) == "float" and stm->rhs->accept(this) == 2) {
    // } else if (env.lookup_type(stm->id) == "bool" and stm->rhs->accept(this) == 3) {
    //     cout << "Error: No coinciden los tipos en la asignacion: " << stm->id << endl;
    //     exit(0);
    // }
}

void TypeVisitor::visit(PrintStatement* stm) {
    // cout << stm->e->accept(this);
}

void TypeVisitor::visit(IfStatement* stm) {
    // int condition_type = stm->condition->accept(this);
    // if (condition_type != 3) { // 3 = bool
    //     cout << "Error: La condicion tiene que se booleana" << endl;
    //     exit(0);
    // }
    // stm->then->accept(this);
    // if (stm->els != nullptr) {
    //     stm->els->accept(this);
    // }
}

void TypeVisitor::visit(WhileStatement* stm) {
    // int condition_type = stm->condition->accept(this);
    // if (condition_type != 3) { // 3 = bool
    //     cout << "Error: La condicion tiene que se booleana" << endl;
    //     exit(0);
    // }
    // stm->b->accept(this);
}

void TypeVisitor::visit(ForStatement* stm) {
    // int range_type = stm->range->accept(this);
    // if (range_type != 1) {
    //     cout << "Error: El rango tiene que usar enteros" << endl;
    //     exit(0);
    // }
    // 
    // env.add_var(stm->id, stm->type.empty() ? "Int" : stm->type);
    // stm->body->accept(this);
}

void TypeVisitor::visit(VarDec* stm) {
    
}

void TypeVisitor::visit(VarDecList* stm) {
    // for(auto i: stm->decls){
    //     i->accept(this);
    // }
}

void TypeVisitor::visit(StatementList* stm) {
    // for(auto i: stm->stms){
    //     i->accept(this);
    // }
}

void TypeVisitor::visit(Body* b) {
    // env.add_level(); // tener en cuenta que agrego un nivel cada que inicio un programa    
    // b->vardecs->accept(this);
    // b->slist->accept(this);
    // env.remove_level(); // quitar el nivel cuando termine de ejecutar el programa
}


void TypeVisitor::visit(Program* program) {
    if (program->vardecs) {
        program->vardecs->accept(this);
    }
    
    if (program->fundecs) {
        program->fundecs->accept(this);
    }
}

void TypeVisitor::visit(FunDec* fundec) {
    // env.add_level();
    // if (fundec->params) {
    //     fundec->params->accept(this);
    // }
    // if (fundec->body) {
    //     fundec->body->accept(this);
    // }
    // env.remove_level();
}

void TypeVisitor::visit(FunDecList* fundecs) {
    for (auto fundec : fundecs->fundecs) {
        fundec->accept(this);
    }
}

int TypeVisitor::visit(FCallExp* fcall) {

    return 1;
}

void TypeVisitor::visit(FCallStm* fcall) {
    if (!fcall->argumentos.empty()) {
        for (auto arg : fcall->argumentos) {
            arg->accept(this);
        }
    }
}

void TypeVisitor::visit(ReturnStatement* retstm) {
    if (retstm->e) {
        retstm->e->accept(this);
    }
}

/////////////////////////////////////////////////////////////////////////////////////
///// CodeGen
/////////////////////////////////////////////////////////////////////////////////////

int CodeGenVisitor::visit(BinaryExp* exp) {
    return 0;
}

int CodeGenVisitor::visit(UnaryExp* exp) {
    return 0;
}

int CodeGenVisitor::visit(NumberExp* exp) {
    return 0;
}

int CodeGenVisitor::visit(DecimalExp* exp) {
    return 0;
}

int CodeGenVisitor::visit(BoolExp* exp) {
    return 0;
}

int CodeGenVisitor::visit(IdentifierExp* exp) {
    return 0;
}

int CodeGenVisitor::visit(RangeExp* exp) {
    return 0;
}

void CodeGenVisitor::visit(AssignStatement* stm) {
}

// Para la declaración de suma +=
void CodeGenVisitor::visit(PlusAssignStatement* stm) {
}

// Para la declaración de resta -=
void CodeGenVisitor::visit(MinusAssignStatement* stm) {
}

void CodeGenVisitor::visit(PrintStatement* stm) {
}

void CodeGenVisitor::visit(IfStatement* stm) {
}

void CodeGenVisitor::visit(WhileStatement* stm) {
}

void CodeGenVisitor::visit(ForStatement* stm) {
}

void CodeGenVisitor::visit(VarDec* stm) {
}

void CodeGenVisitor::visit(VarDecList* stm) {
}

void CodeGenVisitor::visit(StatementList* stm) {
}

void CodeGenVisitor::visit(Body* b) {
}

void CodeGenVisitor::visit(Program* program) {
}



/////////////////////////////////////////////////////////////////////////////////////
///// GenCodeVisitor - Generador de código Assembly x86-64
/////////////////////////////////////////////////////////////////////////////////////

void GenCodeVisitor::generar(Program* program) {
    program->accept(this); 
}

void GenCodeVisitor::visit(Program* program) {
    entornoFuncion = false;
    memoriaGlobal.clear();
    memoria.clear();
    
    if (program->vardecs) {
        program->vardecs->accept(this);
    }
    
    out << ".data" << endl;
    out << "print_fmt: .string \"%ld\\n\"" << endl;
    out << "print_float_fmt: .string \"%.1f\\n\"" << endl;
    out << "print_bool_true: .string \"true\\n\"" << endl;
    out << "print_bool_false: .string \"false\\n\"" << endl;

    for (auto it = memoriaGlobal.begin(); it != memoriaGlobal.end(); ++it) {
        out << it->first << ": .quad 0" << endl;
    }
    
    out << endl << ".text" << endl;
    
    if (program->fundecs) {
        program->fundecs->accept(this);
    }
    
    out << ".section .note.GNU-stack,\"\",@progbits" << endl;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << "    pushq %rax" << endl;  // Guardar operando izquierdo
    exp->right->accept(this);
    out << "    movq %rax, %rcx" << endl;  // Operando derecho en %rcx
    out << "    popq %rax" << endl;   // Operando izquierdo en %rax
    
    switch (exp->op) {
        case PLUS_OP:
            out << "    addq %rcx, %rax" << endl;
            break;
        case MINUS_OP:
            out << "    subq %rcx, %rax" << endl;
            break;
        case MUL_OP:
            out << "    imulq %rcx, %rax" << endl;
            break;
        case DIV_OP:
            out << "    cqto" << endl;  // Extender signo de %rax a %rdx:%rax
            out << "    idivq %rcx" << endl;
            break;
        case LT_OP:
            out << "    cmpq %rcx, %rax" << endl;
            out << "    movl $0, %eax" << endl;
            out << "    setl %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case LE_OP:
            out << "    cmpq %rcx, %rax" << endl;
            out << "    movl $0, %eax" << endl;
            out << "    setle %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case GT_OP:
            out << "    cmpq %rcx, %rax" << endl;
            out << "    movl $0, %eax" << endl;
            out << "    setg %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case GE_OP:
            out << "    cmpq %rcx, %rax" << endl;
            out << "    movl $0, %eax" << endl;
            out << "    setge %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case EQ_OP:
            out << "    cmpq %rcx, %rax" << endl;
            out << "    movl $0, %eax" << endl;
            out << "    sete %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case NOT_EQ_OP:
            out << "    cmpq %rcx, %rax" << endl;
            out << "    movl $0, %eax" << endl;
            out << "    setne %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
        case AND_OP:
            out << "    andq %rcx, %rax" << endl;
            break;
        case OR_OP:
            out << "    orq %rcx, %rax" << endl;
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(UnaryExp* exp) {
    exp->e->accept(this);
    switch (exp->op) {
        case NOT_OP:
            out << "    cmpq $0, %rax" << endl;
            out << "    movl $0, %eax" << endl;
            out << "    sete %al" << endl;
            out << "    movzbq %al, %rax" << endl;
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(NumberExp* exp) {
    out << "    movq $" << exp->value << ", %rax" << endl;
    return 0;
}

int GenCodeVisitor::visit(DecimalExp* exp) {
    // Para floats, por simplicidad, convertimos a entero
    out << "    movq $" << (int)exp->value << ", %rax" << endl;
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    out << "    movq $" << exp->value << ", %rax" << endl;
    return 0;
}

int GenCodeVisitor::visit(IdentifierExp* exp) {
    if (memoriaGlobal.count(exp->name)) {
        out << "    movq " << exp->name << "(%rip), %rax" << endl;
    } else {
        out << "    movq " << memoria[exp->name] << "(%rbp), %rax" << endl;
    }
    return 0;
}

int GenCodeVisitor::visit(RangeExp* exp) {
    // Los rangos se manejan en ForStatement, aquí solo retornamos 0
    return 0;
}

void GenCodeVisitor::visit(AssignStatement* stm) {
    stm->rhs->accept(this);  // Evaluar el lado derecho
    if (memoriaGlobal.count(stm->id)) {
        out << "    movq %rax, " << stm->id << "(%rip)" << endl;
    } else {
        out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
    }
}

void GenCodeVisitor::visit(PlusAssignStatement* stm) {
    // Cargar valor actual
    if (memoriaGlobal.count(stm->id)) {
        out << "    movq " << stm->id << "(%rip), %rax" << endl;
    } else {
        out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
    }
    out << "    pushq %rax" << endl;  // Guardar valor actual
    
    stm->rhs->accept(this);  // Evaluar operando derecho
    out << "    movq %rax, %rcx" << endl;
    out << "    popq %rax" << endl;
    out << "    addq %rcx, %rax" << endl;  // Sumar
    
    // Guardar resultado
    if (memoriaGlobal.count(stm->id)) {
        out << "    movq %rax, " << stm->id << "(%rip)" << endl;
    } else {
        out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
    }
}

void GenCodeVisitor::visit(MinusAssignStatement* stm) {
    // Similar a PlusAssignStatement pero con resta
    if (memoriaGlobal.count(stm->id)) {
        out << "    movq " << stm->id << "(%rip), %rax" << endl;
    } else {
        out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
    }
    out << "    pushq %rax" << endl;
    
    stm->rhs->accept(this);
    out << "    movq %rax, %rcx" << endl;
    out << "    popq %rax" << endl;
    out << "    subq %rcx, %rax" << endl;  // Restar
    
    if (memoriaGlobal.count(stm->id)) {
        out << "    movq %rax, " << stm->id << "(%rip)" << endl;
    } else {
        out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
    }
}

void GenCodeVisitor::visit(PrintStatement* stm) {
    stm->e->accept(this);
    out << "    movq %rax, %rsi" << endl;
    out << "    leaq print_fmt(%rip), %rdi" << endl;
    out << "    movl $0, %eax" << endl;
    out << "    call printf@PLT" << endl;
}

void GenCodeVisitor::visit(IfStatement* stm) {
    int labelId = labelCounter++;
    
    // Evaluar condición principal
    stm->condition->accept(this);
    out << "    cmpq $0, %rax" << endl;
    out << "    je .else_if_" << labelId << "_0" << endl;
    
    // Cuerpo del if principal
    stm->then->accept(this);
    out << "    jmp .endif_" << labelId << endl;
    
    // Manejar else if's
    for (size_t i = 0; i < stm->elseifs.size(); i++) {
        out << ".else_if_" << labelId << "_" << i << ":" << endl;
        stm->elseifs[i].first->accept(this);  // Evaluar condición else if
        out << "    cmpq $0, %rax" << endl;
        
        if (i + 1 < stm->elseifs.size()) {
            out << "    je .else_if_" << labelId << "_" << (i + 1) << endl;
        } else {
            out << "    je .else_" << labelId << endl;
        }
        
        stm->elseifs[i].second->accept(this);  // Cuerpo del else if
        out << "    jmp .endif_" << labelId << endl;
    }
    
    // Cuerpo del else final si existe
    if (stm->els != nullptr) {
        out << ".else_" << labelId << ":" << endl;
        stm->els->accept(this);
    } else if (!stm->elseifs.empty()) {
        out << ".else_" << labelId << ":" << endl;
    }
    
    out << ".endif_" << labelId << ":" << endl;
}

void GenCodeVisitor::visit(WhileStatement* stm) {
    int labelId = labelCounter++;
    
    out << ".while_start_" << labelId << ":" << endl;
    stm->condition->accept(this);
    out << "    cmpq $0, %rax" << endl;
    out << "    je .while_end_" << labelId << endl;
    
    stm->b->accept(this);
    out << "    jmp .while_start_" << labelId << endl;
    out << ".while_end_" << labelId << ":" << endl;
}

void GenCodeVisitor::visit(ForStatement* stm) {
    int labelId = labelCounter++;
    
    if (RangeExp* range = dynamic_cast<RangeExp*>(stm->range)) {
        range->start->accept(this);
        int startOffset = offset;
        offset -= 8;
        out << "    movq %rax, " << startOffset << "(%rbp)" << endl;  // Guardar start

        range->end->accept(this);
        int endOffset = offset;
        offset -= 8;
        out << "    movq %rax, " << endOffset << "(%rbp)" << endl;    // Guardar end
        
        // Reservar espacio para la variable del for si no existe
        if (memoria.find(stm->id) == memoria.end()) {
            memoria[stm->id] = offset;
            offset -= 8;
        }
        
        // Inicializar variable del loop con el valor de start
        out << "    movq " << startOffset << "(%rbp), %rax" << endl;
        out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;  // i = start
        
        out << ".for_start_" << labelId << ":" << endl;
        // Verificar condición: i <= end
        out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;  // Cargar i
        out << "    movq " << endOffset << "(%rbp), %rcx" << endl;         // Cargar end
        out << "    cmpq %rcx, %rax" << endl;                             // Comparar i con end
        out << "    jg .for_end_" << labelId << endl;                     // Si i > end, salir
        
        // Cuerpo del for
        stm->body->accept(this);
        
        // Incrementar i
        out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
        out << "    incq %rax" << endl;
        out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
        
        out << "    jmp .for_start_" << labelId << endl;
        out << ".for_end_" << labelId << ":" << endl;
    }
}

void GenCodeVisitor::visit(VarDec* stm) {
    if (!entornoFuncion) {
        memoriaGlobal[stm->id] = true;

        if (stm->value != nullptr) {
            stm->value->accept(this);
            out << "    movq %rax, " << stm->id << "(%rip)" << endl;
        }
    } else {
        memoria[stm->id] = offset;
        
        if (stm->value != nullptr) {
            stm->value->accept(this);
            out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
        }
        
        offset -= 8;
    }
}

void GenCodeVisitor::visit(VarDecList* stm) {
    for (auto dec : stm->decls) {
        dec->accept(this);
    }
}

void GenCodeVisitor::visit(StatementList* stm) {
    for (auto stmt : stm->stms) {
        stmt->accept(this);
    }
}

void GenCodeVisitor::visit(Body* b) {
    b->vardecs->accept(this);
    b->slist->accept(this);
}

void GenCodeVisitor::visit(FunDec* fundec) {
    entornoFuncion = true;
    currentFunction = fundec->nombre; 
    memoria.clear();
    offset = -8;
    
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    
    out << ".globl " << fundec->nombre << endl;
    out << fundec->nombre << ":" << endl;
    out << "    pushq %rbp" << endl;
    out << "    movq %rsp, %rbp" << endl;

    if (!fundec->parametros.empty()) {
        int paramIndex = 0;
        for (auto param : fundec->parametros) {
            if (paramIndex < argRegs.size()) {
                memoria[param] = offset;
                out << "    movq " << argRegs[paramIndex] << ", " << offset << "(%rbp)" << endl;
                offset -= 8;
                paramIndex++;
            }
        }
    }
    if (fundec->cuerpo) {
        if (fundec->cuerpo->vardecs) {
            fundec->cuerpo->vardecs->accept(this);
        }
        
        int reserva = -offset - 8;
        if (reserva > 0) {
            out << "    subq $" << reserva << ", %rsp" << endl;
        }
        
        if (fundec->cuerpo->slist) {
            fundec->cuerpo->slist->accept(this);
        }
    }

    out << ".end_" << fundec->nombre << ":" << endl;
    out << "    leave" << endl;
    out << "    ret" << endl;
    
    entornoFuncion = false;
}

void GenCodeVisitor::visit(FunDecList* fundecs) {
    for (auto fundec : fundecs->fundecs) {
        fundec->accept(this);
    }
}

int GenCodeVisitor::visit(FCallExp* fcall) {
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    
    if (!fcall->argumentos.empty()) {
        int argIndex = 0;
        for (auto arg : fcall->argumentos) {
            if (argIndex < argRegs.size()) {
                arg->accept(this);
                out << "    movq %rax, " << argRegs[argIndex] << endl;
                argIndex++;
            } else {
                arg->accept(this);
                out << "    pushq %rax" << endl;
            }
        }
    }
    
    out << "    call " << fcall->nombre << endl;
    
    if (fcall->argumentos.size() > 6) {
        int numArgsOnStack = fcall->argumentos.size() - 6;
        out << "    addq $" << (numArgsOnStack * 8) << ", %rsp" << endl;
    }
    
    return 0;
}

void GenCodeVisitor::visit(FCallStm* fcall) {
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    
    if (!fcall->argumentos.empty()) {
        int argIndex = 0;
        for (auto arg : fcall->argumentos) {
            if (argIndex < argRegs.size()) {
                arg->accept(this);
                out << "    movq %rax, " << argRegs[argIndex] << endl;
                argIndex++;
            } else {
                arg->accept(this);
                out << "    pushq %rax" << endl;
            }
        }
    }
    
    out << "    call " << fcall->nombre << endl;
    
    if (fcall->argumentos.size() > 6) {
        int numArgsOnStack = fcall->argumentos.size() - 6;
        out << "    addq $" << (numArgsOnStack * 8) << ", %rsp" << endl;
    }
}

void GenCodeVisitor::visit(ReturnStatement* retstm) {
    if (retstm->e) {
        retstm->e->accept(this);
    }
    
    out << "    jmp .end_" << currentFunction << endl;
}