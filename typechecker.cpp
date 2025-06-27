#include "typechecker.h"
#include "exp.h"
#include <iostream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////
///// TypeChecker
/////////////////////////////////////////////////////////////////////////////////////

// TYPECHECKER 
// UNDEFINED_TYPE = 0
// INT_TYPE = 1
// FLOAT_TYPE = 2
// BOOLEAN_TYPE = 3
// UNIT_TYPE = 4
// RANGE_TYPE = 5

TypeChecker::TypeChecker() : retorno("") {}

void TypeChecker::check(Program* program){
    env.add_level();
    program->accept(this);
    env.remove_level();
}

int TypeChecker::visit(BinaryExp* exp) {
    int left_type = exp->left->accept(this);
    int right_type = exp->right->accept(this);
    
    switch(exp->op) {
        case PLUS_OP:
        case MINUS_OP:
        case MUL_OP: //Si tenemos una operacion de 2 factores tienen que ser numeros
            if (left_type == INT_TYPE && right_type == INT_TYPE) { 
                return INT_TYPE; 
            } else if ((left_type == INT_TYPE || left_type == FLOAT_TYPE) && 
                      (right_type == INT_TYPE || right_type == FLOAT_TYPE)) { 
                return FLOAT_TYPE; 
            } else { 
                cout << "Error de tipos: operación aritmética requiere tipos numéricos (Int o Float)" << endl;
                exit(1);
            }
            break;
            
        case DIV_OP: //Se cumple lo mismo que antes, pero se verifica que el divisor no sea ni 0 entero ni float
            if (left_type == INT_TYPE && right_type == INT_TYPE) {
                if (NumberExp* right_num = dynamic_cast<NumberExp*>(exp->right)) {
                    if (right_num->value == 0) {
                        cout << "Error: División por cero detectada en tiempo de compilación" << endl;
                        exit(1);
                    }
                }
                return INT_TYPE;
            } else if ((left_type == INT_TYPE || left_type == FLOAT_TYPE) && 
                      (right_type == INT_TYPE || right_type == FLOAT_TYPE)) {
                if (DecimalExp* right_dec = dynamic_cast<DecimalExp*>(exp->right)) {
                    if (right_dec->value == 0.0f) {
                        cout << "Error: División por cero detectada en tiempo de compilación" << endl;
                        exit(1);
                    }
                }
                if (NumberExp* right_num = dynamic_cast<NumberExp*>(exp->right)) {
                    if (right_num->value == 0) {
                        cout << "Error: División por cero detectada en tiempo de compilación" << endl;
                        exit(1);
                    }
                }
                return FLOAT_TYPE; 
            } else {
                cout << "Error de tipos: operación de división requiere tipos numéricos (Int o Float)" << endl;
                exit(1);
            }
            break;
            
        case LT_OP:
        case LE_OP:
        case GT_OP:
        case GE_OP: //Ambos factores tienen que ser numeros para que se comparen
            if ((left_type == INT_TYPE || left_type == FLOAT_TYPE) && 
                (right_type == INT_TYPE || right_type == FLOAT_TYPE)) {
                return BOOLEAN_TYPE; 
            } else {
                cout << "Error de tipos: comparación requiere tipos numéricos (Int o Float)" << endl;
                exit(1);
            }
            break;
            
        case EQ_OP:
        case NOT_EQ_OP:
            // Ambos factores pueden ser del mismo tipo o tipos compatibles
            if (left_type == right_type || 
                ((left_type == INT_TYPE || left_type == FLOAT_TYPE) && 
                 (right_type == INT_TYPE || right_type == FLOAT_TYPE))) {
                return BOOLEAN_TYPE; // Boolean
            } else {
                cout << "Error de tipos: comparación de igualdad requiere tipos compatibles" << endl;
                exit(1);
            }
            break;
            
        case AND_OP:
        case OR_OP:
            // Solo aplica para operaciones booleanas
            if (left_type == BOOLEAN_TYPE && right_type == BOOLEAN_TYPE) {
                return BOOLEAN_TYPE; 
            } else {
                cout << "Error de tipos: operaciones lógicas (&&, ||) requieren tipos Boolean" << endl;
                exit(1);
            }
            break;
            
        default:
            cout << "Error: operador binario no reconocido" << endl;
            exit(1);
    }
    return UNDEFINED_TYPE;
}

int TypeChecker::visit(UnaryExp* exp) {
    int operand_type = exp->e->accept(this);
    
    switch(exp->op) {
        case NOT_OP:
            if (operand_type == BOOLEAN_TYPE) {
                return BOOLEAN_TYPE; 
            } else {
                cout << "Error de tipos: operador ! requiere tipo Boolean" << endl;
                exit(1);
            }
            break;
            
        case UMINUS_OP:
            if (operand_type == INT_TYPE) {
                return INT_TYPE; // -Int = Int
            } else if (operand_type == FLOAT_TYPE) {
                return FLOAT_TYPE; // -Float = Float
            } else {
                cout << "Error de tipos: operador unario - requiere tipo numérico (Int o Float)" << endl;
                exit(1);
            }
            break;
            
        default:
            cout << "Error: operador unario no reconocido" << endl;
            exit(1);
    }
    return UNDEFINED_TYPE;
}

int TypeChecker::visit(NumberExp* exp) {
    return INT_TYPE;
}

int TypeChecker::visit(DecimalExp* exp) {
    return FLOAT_TYPE;
}

int TypeChecker::visit(BoolExp* exp) {
    return BOOLEAN_TYPE; 
}

int TypeChecker::visit(IdentifierExp* exp) {
    if (!env.check(exp->name)){ //
        cout << "Error: Variable no declarada: " << exp->name << endl;        
        exit(1);
    }
    
    string var_type = env.lookup_type(exp->name);
    if (var_type == "Int") return INT_TYPE;
    else if (var_type == "Float") return FLOAT_TYPE;
    else if (var_type == "Boolean") return BOOLEAN_TYPE;
    else {
        cout << "Error: Tipo de variable desconocido: " << var_type << endl;
        exit(1);
    }
    return UNDEFINED_TYPE;
}

int TypeChecker::visit(RangeExp* exp) {
    int start_type = exp->start->accept(this);
    int end_type = exp->end->accept(this);
    
    if (start_type != INT_TYPE || end_type != INT_TYPE) { //Los rangos solo pueden ser enteros
        cout << "Error de tipos: Las expresiones de rango deben ser de tipo Int" << endl;
        exit(1);
    }
    
    if (exp->step != nullptr) {
        int step_type = exp->step->accept(this); //El paso tambien debe ser entero si es que existe
        if (step_type != INT_TYPE) {
            cout << "Error de tipos: El step del rango debe ser de tipo Int" << endl;
            exit(1);
        }
    }
    
    return RANGE_TYPE;
}

void TypeChecker::visit(AssignStatement* stm) {
    if (!env.check(stm->id)) {
        cout << "Error: Variable no declarada: " << stm->id << endl;
        exit(1);
    }

    string var_type = env.lookup_type(stm->id);
    int expr_type = stm->rhs->accept(this);
    
    if (var_type == "Int" && expr_type == INT_TYPE) {
    } else if (var_type == "Float" && (expr_type == INT_TYPE || expr_type == FLOAT_TYPE)) {
    } else if (var_type == "Boolean" && expr_type == BOOLEAN_TYPE) {
    } else { //Error donde el tipo de asignacion no es el adecuado, el float soporta tanto int como float
        cout << "Error de tipos: No se puede asignar tipo ";
        switch(expr_type) {
            case INT_TYPE: cout << "Int"; break;
            case FLOAT_TYPE: cout << "Float"; break;
            case BOOLEAN_TYPE: cout << "Boolean"; break;
            default: cout << "desconocido"; break;
        }
        cout << " a variable de tipo " << var_type << ": " << stm->id << endl;
        exit(1);
    }
}

void TypeChecker::visit(PlusAssignStatement* stm) {
    if (!env.check(stm->id)) {
        cout << "Error: Variable no declarada: " << stm->id << endl;
        exit(1);
    }

    string var_type = env.lookup_type(stm->id);
    int expr_type = stm->rhs->accept(this);
    
    if (var_type == "Int" && expr_type == INT_TYPE) {  //Solo se puede aplicar a variables numericas, por lo que se verifica el tipo del que va a recibir
    } else if (var_type == "Float" && (expr_type == INT_TYPE || expr_type == FLOAT_TYPE)) {
    } else {
        cout << "Error de tipos: Operador += requiere tipos numéricos compatibles. ";
        cout << "Variable " << stm->id << " es " << var_type << " pero expresión es ";
        switch(expr_type) {
            case INT_TYPE: cout << "Int"; break;
            case FLOAT_TYPE: cout << "Float"; break;
            case BOOLEAN_TYPE: cout << "Boolean"; break;
            default: cout << "desconocido"; break;
        }
        cout << endl;
        exit(1);
    }
}

void TypeChecker::visit(MinusAssignStatement* stm) { 
    if (!env.check(stm->id)) {
        cout << "Error: Variable no declarada: " << stm->id << endl;
        exit(1);
    }

    string var_type = env.lookup_type(stm->id);
    int expr_type = stm->rhs->accept(this);
  
    if (var_type == "Int" && expr_type == INT_TYPE) {
    } else if (var_type == "Float" && (expr_type == INT_TYPE || expr_type == FLOAT_TYPE)) {
    } else {
        cout << "Error de tipos: Operador -= requiere tipos numéricos compatibles. ";
        cout << "Variable " << stm->id << " es " << var_type << " pero expresión es ";
        switch(expr_type) {
            case INT_TYPE: cout << "Int"; break;
            case FLOAT_TYPE: cout << "Float"; break;
            case BOOLEAN_TYPE: cout << "Boolean"; break;
            default: cout << "desconocido"; break;
        }
        cout << endl;
        exit(1);
    }
}

void TypeChecker::visit(PrintStatement* stm) {
    stm->e->accept(this); 
}

void TypeChecker::visit(IfStatement* stm) {
    int condition_type = stm->condition->accept(this);
    if (condition_type != BOOLEAN_TYPE) { 
        cout << "Error de tipos: La condición del if debe ser de tipo Boolean" << endl;
        exit(1);
    }
    
    stm->then->accept(this);
    
    for (const auto& elseif : stm->elseifs) { //Verificamos que todas las condiciones de los else if sean de tipo booleana
        int elseif_condition_type = elseif.first->accept(this);
        if (elseif_condition_type != BOOLEAN_TYPE) {
            cout << "Error de tipos: La condición del else if debe ser de tipo Boolean" << endl;
            exit(1);
        }
        elseif.second->accept(this);
    }
    
    if (stm->els != nullptr) { //Si existe un body para el else, lo analizamos
        stm->els->accept(this);
    }
}

void TypeChecker::visit(WhileStatement* stm) {
    int condition_type = stm->condition->accept(this);
    if (condition_type != BOOLEAN_TYPE) {
        cout << "Error de tipos: La condición del while debe ser de tipo Boolean" << endl;
        exit(1);
    }
    stm->b->accept(this);
}

void TypeChecker::visit(ForStatement* stm) {
    int range_type = stm->range->accept(this);
    if (range_type != RANGE_TYPE) { 
        cout << "Error de tipos: El for requiere un rango válido" << endl;
        exit(1);
    }
    
   
    env.add_level();
    string var_type = stm->type.empty() ? "Int" : stm->type;
    if (var_type != "Int") { //Hay que verificar que el iterador tabmbien sea un entero
        cout << "Error de tipos: La variable del for debe ser de tipo Int" << endl;
        exit(1);
    }
    env.add_var(stm->id, var_type);
    
    stm->body->accept(this);
    env.remove_level();
}

void TypeChecker::visit(VarDec* stm) {
    // Ahora acepto arrayOf<Int> y arrayOf<Float> como tipos válidos
    if (stm->type != "Int" && stm->type != "Float" && stm->type != "Boolean" && stm->type != "arrayOf<Int>" && stm->type != "arrayOf<Float>" && stm->type != "Unit") {
        cout << "Error: Tipo de variable desconocido o no válido: '" << stm->type << "'\nTipos válidos: Int, Float, Boolean, arrayOf<Int>, arrayOf<Float>, Unit" << endl;
        exit(1);
    }

    if (stm->value != nullptr) { //Si es que hay algun valor que se le esta asignando, hay que verificar que sea compatible
        int init_type = stm->value->accept(this);
        if (stm->type == "Int" && init_type == INT_TYPE) { 
        } else if (stm->type == "Float" && (init_type == INT_TYPE || init_type == FLOAT_TYPE)) {
        } else if (stm->type == "Boolean" && init_type == BOOLEAN_TYPE) {
        } else if (stm->type == "arrayOf<Int>" && init_type == 5) {
        } else if (stm->type == "arrayOf<Float>" && init_type == 6) {
        } else {
            cout << "Error de tipos: No se puede inicializar variable " << stm->id 
                 << " de tipo " << stm->type << " con valor de tipo ";
            switch(init_type) {
                case INT_TYPE: cout << "Int"; break;
                case FLOAT_TYPE: cout << "Float"; break;
                case BOOLEAN_TYPE: cout << "Boolean"; break;
                case 5: cout << "arrayOf<Int>"; break;
                case 6: cout << "arrayOf<Float>"; break;
                default: cout << "desconocido"; break;
            }
            cout << endl;
            exit(1);
        }
    }
    
    // Añadir la variable al entorno
    if (stm->type == "arrayOf<Int>") {
        env.add_array(stm->id, "arrayOf<Int>");
    } else if (stm->type == "arrayOf<Float>") {
        env.add_array(stm->id, "arrayOf<Float>");
    } else {
        env.add_var(stm->id, stm->type);
    }
}

void TypeChecker::visit(VarDecList* stm) {
    for(auto vardec : stm->decls){
        vardec->accept(this);
    }
}

void TypeChecker::visit(StatementList* stm) {
    for(auto stmt : stm->stms){
        stmt->accept(this);
    }
}

void TypeChecker::visit(Body* b) {
    env.add_level();
    b->vardecs->accept(this);
    b->slist->accept(this);
    env.remove_level();
}

void TypeChecker::visit(Program* program) {
    if (program->fundecs) {
        for (auto fundec : program->fundecs->fundecs) {
            if (funciones.find(fundec->nombre) != funciones.end()) {
                cout << "Error: Función duplicada: " << fundec->nombre << endl;
                exit(1);
            }
            funciones[fundec->nombre] = fundec;
        }
    }
    
    //Analizar todas las globales
    if (program->vardecs) {
        program->vardecs->accept(this);
    }
    
    //Analizar todas las funciones que se tienen
    if (program->fundecs) {
        program->fundecs->accept(this);
    }
    
    if (funciones.find("main") == funciones.end()) { //Si es que no existe un main, entonces no se puede ejecutar
        cout << "Error: No se encontró función main" << endl;
        exit(1);
    }
}

void TypeChecker::visit(FunDec* fundec) {
    if (fundec->tipo_retorno != "Int" && fundec->tipo_retorno != "Float" && 
        fundec->tipo_retorno != "Boolean" && fundec->tipo_retorno != "Unit") { //Verificar si es que el tipo de retorno es valido
        cout << "Error: Tipo de retorno no válido en función " << fundec->nombre 
             << ": " << fundec->tipo_retorno << endl;
        exit(1);
    }
    
    env.add_level();
    
    // Establecer el contexto de la función actual 
    string retorno_anterior = retorno;
    retorno = fundec->tipo_retorno;
    
    // Agregar parámetros al entorno
    auto param_it = fundec->parametros.begin();
    auto type_it = fundec->tipos_parametros.begin();
    
    while (param_it != fundec->parametros.end() && type_it != fundec->tipos_parametros.end()) {
        // Verificar que el tipo del parámetro es válido
        if (*type_it != "Int" && *type_it != "Float" && *type_it != "Boolean") {
            cout << "Error: Tipo de parámetro no válido en función " << fundec->nombre 
                 << ", parámetro " << *param_it << ": " << *type_it << endl;
            exit(1);
        }
        
        env.add_var(*param_it, *type_it);
        ++param_it;
        ++type_it;
    }
    
    // Verificar el cuerpo de la función
    if (fundec->cuerpo) {
        fundec->cuerpo->accept(this);
    }
    
    // Restaurar el retorno anterior (Util cuando llamamos a funciones dentro de otra funcion)
    retorno = retorno_anterior;
    env.remove_level();
}

void TypeChecker::visit(FunDecList* fundecs) {
    for (auto fundec : fundecs->fundecs) {
        fundec->accept(this);
    }
}

int TypeChecker::visit(FCallExp* fcall) {
    // Verificar que la función existe
    if (funciones.find(fcall->nombre) == funciones.end()) { 
        cout << "Error: Función no declarada: " << fcall->nombre << endl;
        exit(1);
    }
    
    FunDec* funcion = funciones[fcall->nombre]; //Como ya estamos seguros que existe, la recuperamos
    
    // Verificar que el número de argumentos coincide
    if (fcall->argumentos.size() != funcion->parametros.size()) {
        cout << "Error: Número incorrecto de argumentos en llamada a función " << fcall->nombre 
             << ". Esperado: " << funcion->parametros.size() 
             << ", Recibido: " << fcall->argumentos.size() << endl;
        exit(1);
    }
    
    // Verificar tipos de argumentos
    auto arg_it = fcall->argumentos.begin();
    auto type_it = funcion->tipos_parametros.begin();
    int arg_num = 0;
    
    while (arg_it != fcall->argumentos.end() && type_it != funcion->tipos_parametros.end()) {
        int tipo_arg = (*arg_it)->accept(this);
        string tipo = *type_it;
        
        bool compatible = false; //Verificamos si es que el tipo de variable es compatible
        if (tipo == "Int" && tipo_arg == INT_TYPE) {
            compatible = true;
        } else if (tipo == "Float" && (tipo_arg == INT_TYPE || tipo_arg == FLOAT_TYPE)) {
            compatible = true;
        } else if (tipo == "Boolean" && tipo_arg == BOOLEAN_TYPE) {
            compatible = true;
        }
        
        if (!compatible) {
            cout << "Error de tipos: Argumento " << arg_num << " en llamada a función " 
                 << fcall->nombre << ". Esperado: " << tipo << ", Recibido: ";
            switch(tipo_arg) {
                case INT_TYPE: cout << "Int"; break;
                case FLOAT_TYPE: cout << "Float"; break;
                case BOOLEAN_TYPE: cout << "Boolean"; break;
                default: cout << "desconocido"; break;
            }
            cout << endl;
            exit(1);
        }
        
        ++arg_it;
        ++type_it;
        ++arg_num;
    }
    
    if (funcion->tipo_retorno == "Int") return INT_TYPE;
    else if (funcion->tipo_retorno == "Float") return FLOAT_TYPE;
    else if (funcion->tipo_retorno == "Boolean") return BOOLEAN_TYPE;
    else if (funcion->tipo_retorno == "Unit") return UNIT_TYPE;
    else return UNDEFINED_TYPE;
}

void TypeChecker::visit(FCallStm* fcall) {
    // Reutilizar la lógica de FCallExp
    FCallExp* fexp = new FCallExp(fcall->nombre, fcall->argumentos);
    fexp->accept(this);
    delete fexp;
}

void TypeChecker::visit(ReturnStatement* retstm) {
    if (retorno.empty()) {
        cout << "Error: Return statement fuera de una función" << endl;
        exit(1);
    }
    
    if (retstm->e != nullptr) { //Si si existe el retorno, hay que verificar que sea del tipo correcto
        int tipo_retorno = retstm->e->accept(this);

        bool compatible = false;
        if (retorno == "Int" && tipo_retorno == INT_TYPE) {
            compatible = true;
        } else if (retorno == "Float" && (tipo_retorno == INT_TYPE || tipo_retorno == FLOAT_TYPE)) {
            compatible = true; 
        } else if (retorno == "Boolean" && tipo_retorno == BOOLEAN_TYPE) {
            compatible = true;
        }
        
        if (!compatible) {
            cout << "Error de tipos: Return statement retorna tipo ";
            switch(tipo_retorno) {
                case INT_TYPE: cout << "Int"; break;
                case FLOAT_TYPE: cout << "Float"; break;
                case BOOLEAN_TYPE: cout << "Boolean"; break;
                default: cout << "desconocido"; break;
            }
            cout << " pero se esperaba " << retorno << endl;
            exit(1);
        }
    } else {
        // Return sin valor - debe ser función Unit (Void en Kotlin)
        if (retorno != "Unit") {
            cout << "Error de tipos: Return sin valor en función que debe retornar " 
                 << retorno << endl;
            exit(1);
        }
    }
}

int TypeChecker::visit(ArrayExp* exp) {
    // Si llego aquí, estoy creando un array. Checo el tipo de los elementos.
    bool allInt = true, allFloat = true;
    for (auto e : exp->elements) {
        int t = e->accept(this);
        if (t != 1) allInt = false;
        if (t != 2) allFloat = false;
    }
    if (allInt) return 5; // arrayOf<Int>
    if (allFloat) return 6; // arrayOf<Float>
    // Si hay mezcla, lo marco como error
    std::cout << "Error: Los arrays deben ser homogéneos (todo Int o todo Float)" << std::endl;
    exit(1);
}

int TypeChecker::visit(ArrayAccessExp* exp) {
    // Acceso tipo arr[i], reviso que arr sea array y i sea int
    int arrType = exp->array->accept(this);
    int idxType = exp->index->accept(this);
    if (idxType != 1) {
        std::cout << "Error: El índice de un array debe ser Int" << std::endl;
        exit(1);
    }
    if (arrType == 5) return 1; // arrayOf<Int> -> Int
    if (arrType == 6) return 2; // arrayOf<Float> -> Float
    std::cout << "Error: Solo puedo indexar arrays de Int o Float" << std::endl;
    exit(1);
}

int TypeChecker::visit(ArrayMethodExp* exp) {
    // Para .size y .indices, devuelvo Int o arrayOf<Int>
    int arrType = exp->array->accept(this);
    if (arrType != 5 && arrType != 6) {
        std::cout << "Error: Solo puedo usar métodos de array en arrays" << std::endl;
        exit(1);
    }
    if (exp->method == ArrayMethodType::SIZE) return 1; // .size -> Int
    if (exp->method == ArrayMethodType::INDICES) return 5; // .indices -> arrayOf<Int>
    std::cout << "Error: Método de array no soportado" << std::endl;
    exit(1);
}
