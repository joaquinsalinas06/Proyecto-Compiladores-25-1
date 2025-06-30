#include "codegen.h"
#include "exp.h"
#include <iostream>

using namespace std;

// ===================== GenCodeVisitor =====================
// Acá arranca todo el show de generación de código

void GenCodeVisitor::generar(Program* program) {
    // Simplemente le digo al programa que se acepte a sí mismo
    program->accept(this); 
}

void GenCodeVisitor::collectConstants(Program* program) {
    // Primero junto todas las constantes de las variables globales y después de las funciones
    if (program->vardecs) {
        for (auto vardec : program->vardecs->decls) {
            if (vardec->value) {
                collectConstantsFromExp(vardec->value);
            }
        }
    }
    if (program->fundecs) {
        for (auto fundec : program->fundecs->fundecs) {
            if (fundec->cuerpo) {
                collectConstantsFromBody(fundec->cuerpo);
            }
        }
    }
}

void GenCodeVisitor::visit(Program* program) {
    // Arranco con todo limpio
    entornoFuncion = false;
    memoriaGlobal.clear();
    memoria.clear();
    varTypes.clear(); 
    floatConstants.clear();
    globalVarDecs.clear();
    constantCounter = 0;
    
    collectConstants(program); // Junto todas las constantes que voy a necesitar
    
    out << ".data" << endl;
    out << "print_fmt: .string \"%ld\\n\"" << endl;
    out << "print_float_fmt: .string \"%.6g\\n\"" << endl;
    out << "print_bool_true: .string \"true\\n\"" << endl;
    out << "print_bool_false: .string \"false\\n\"" << endl;

    // Ahora sí, reservo espacio para todas las variables globales
    if (program->vardecs) {
        for (auto vardec : program->vardecs->decls) {
            varTypes[vardec->id] = vardec->type;
            memoriaGlobal[vardec->id] = true;
            globalVarDecs.push_back(vardec); 
            
            if (vardec->value != nullptr) {
                if (ArrayExp* arr = dynamic_cast<ArrayExp*>(vardec->value)) { 
                    int num_elements = arr->elements.size();
                    allocateArray(vardec->id, num_elements, arr->type); //Reservamos el espacio 
                    
                    varTypes[vardec->id] = "Array<" + arr->type + ">";
                    
                    // Creamos espacio para cada elemento del array
                    out << vardec->id << "_data:" << endl;
                    for (int i = 0; i < num_elements; i++) {
                        if (arr->type == "Float") {
                            if (DecimalExp* decExp = dynamic_cast<DecimalExp*>(arr->elements[i])) {
                                out << "    .double " << decExp->value << endl;
                            } else if (NumberExp* numExp = dynamic_cast<NumberExp*>(arr->elements[i])) {
                                out << "    .double " << (double)numExp->value << endl;
                            } else {
                                out << "    .double 0.0" << endl;
                            }
                        } else {
                            if (NumberExp* numExp = dynamic_cast<NumberExp*>(arr->elements[i])) {
                                out << "    .quad " << numExp->value << endl;
                            } else {
                                out << "    .quad 0" << endl;
                            }
                        }
                    }
                    out << vardec->id << ": .quad " << vardec->id << "_data" << endl;
                } else if (vardec->type == "Float") {
                    // Si es float, lo guardo como double
                    if (DecimalExp* decExp = dynamic_cast<DecimalExp*>(vardec->value)) {
                        out << vardec->id << ": .double " << decExp->value << endl;
                    } else if (NumberExp* numExp = dynamic_cast<NumberExp*>(vardec->value)) {
                        out << vardec->id << ": .double " << (double)numExp->value << endl;
                    } else {
                        out << vardec->id << ": .double 0.0" << endl;
                    }
                } else {
                    // Si es int, lo guardo como quad
                    if (NumberExp* numExp = dynamic_cast<NumberExp*>(vardec->value)) {
                        out << vardec->id << ": .quad " << numExp->value << endl;
                    } else {
                        out << vardec->id << ": .quad 0" << endl;
                    }
                }
            } else {
                // Si no tiene valor inicial, le pongo 0
                if (vardec->type == "Float") {
                    out << vardec->id << ": .double 0.0" << endl;
                } else {
                    out << vardec->id << ": .quad 0" << endl;
                }
            }
        }
    }
    
    // Si tengo floats, los meto en .rodata para tenerlos a mano
    if (!floatConstants.empty()) {
        out << endl << ".section .rodata" << endl;
        out << ".align 8" << endl;
        for (auto it = floatConstants.begin(); it != floatConstants.end(); ++it) {
            out << it->first << ":" << endl;
            out << "    .double " << it->second << endl;
        }
        out << ".L_neg_zero:" << endl;
        out << "    .double -0.0" << endl;
    }
    
    out << endl << ".text" << endl;
    
    if (program->fundecs) {
        program->fundecs->accept(this);
    }
    
    out << ".section .note.GNU-stack,\"\",@progbits" << endl;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    bool isFloatOp = needsFloatOperation(exp); //Verificamos si la operación es de tipo float
    
    if (isFloatOp) { //Si es de tipo float
        exp->left->accept(this);
        if (isIntegerResult) { //Pero estamos con una variable etera, transformamos de int a double, con los registros xmm
            out << "    cvtsi2sd %rax, %xmm0" << endl;
        }
        out << "    subq $8, %rsp" << endl;
        out << "    movsd %xmm0, (%rsp)" << endl;
        
        exp->right->accept(this);
        if (isIntegerResult) { // Si el operando derecho es un entero, lo convertimos a double
            out << "    cvtsi2sd %rax, %xmm0" << endl;
        }
        out << "    movsd (%rsp), %xmm1" << endl;
        out << "    addq $8, %rsp" << endl;

        
        switch (exp->op) { //Aplicamos la operación correspondiente pero con los registros xmm
            case PLUS_OP:
                out << "    addsd %xmm1, %xmm0" << endl;
                break;
            case MINUS_OP:
                out << "    movsd %xmm1, %xmm2" << endl;
                out << "    subsd %xmm0, %xmm2" << endl;
                out << "    movsd %xmm2, %xmm0" << endl;
                break;
            case MUL_OP:
                out << "    mulsd %xmm1, %xmm0" << endl;
                break;
            case DIV_OP:
                out << "    movsd %xmm1, %xmm2" << endl;
                out << "    divsd %xmm0, %xmm2" << endl;
                out << "    movsd %xmm2, %xmm0" << endl;
                break;
            case LT_OP:
                out << "    comisd %xmm0, %xmm1" << endl;
                out << "    movl $0, %eax" << endl;
                out << "    setb %al" << endl;
                out << "    movzbq %al, %rax" << endl;
                isIntegerResult = true;
                return 0;
            case LE_OP:
                out << "    comisd %xmm0, %xmm1" << endl;
                out << "    movl $0, %eax" << endl;
                out << "    setbe %al" << endl;
                out << "    movzbq %al, %rax" << endl;
                isIntegerResult = true;
                return 0;
            case GT_OP:
                out << "    comisd %xmm0, %xmm1" << endl;
                out << "    movl $0, %eax" << endl;
                out << "    seta %al" << endl;
                out << "    movzbq %al, %rax" << endl;
                isIntegerResult = true;
                return 0;
            case GE_OP:
                out << "    comisd %xmm0, %xmm1" << endl;
                out << "    movl $0, %eax" << endl;
                out << "    setae %al" << endl;
                out << "    movzbq %al, %rax" << endl;
                isIntegerResult = true;
                return 0;
            case EQ_OP:
                out << "    comisd %xmm0, %xmm1" << endl;
                out << "    movl $0, %eax" << endl;
                out << "    sete %al" << endl;
                out << "    movzbq %al, %rax" << endl;
                isIntegerResult = true;
                return 0;
            case NOT_EQ_OP:
                out << "    comisd %xmm0, %xmm1" << endl;
                out << "    movl $0, %eax" << endl;
                out << "    setne %al" << endl;
                out << "    movzbq %al, %rax" << endl;
                isIntegerResult = true;
                return 0;
            case AND_OP: //Convierte ambas entradas a enteros y realiza la operación lógica AND
                out << "    cvttsd2si %xmm1, %rax" << endl; 
                out << "    cvttsd2si %xmm0, %rcx" << endl;
                out << "    andq %rcx, %rax" << endl;
                isIntegerResult = true;
                return 0;
            case OR_OP: //Convierte ambas entradas a enteros y realiza la operación lógica OR
                out << "    cvttsd2si %xmm1, %rax" << endl;
                out << "    cvttsd2si %xmm0, %rcx" << endl;
                out << "    orq %rcx, %rax" << endl;
                isIntegerResult = true;
                return 0;
        }
        isIntegerResult = false; //El resultado es un float
    } else {
        // Si no es una operacio de float ejecutamos la operación con enteros en los registros regulares
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
        isIntegerResult = true;
    }
    return 0;
}

int GenCodeVisitor::visit(UnaryExp* exp) {
    exp->e->accept(this);
    switch (exp->op) {
        case NOT_OP:
            if (isIntegerResult) { //dependiendo si es entero o float tomamos un caso en especifico, en el caso del notop necesitamos transformar el float a entero para poder operar
                out << "    cmpq $0, %rax" << endl;
                out << "    movl $0, %eax" << endl;
                out << "    sete %al" << endl;
                out << "    movzbq %al, %rax" << endl;
            } else {
                out << "    cvttsd2si %xmm0, %rax" << endl;
                out << "    cmpq $0, %rax" << endl;
                out << "    movl $0, %eax" << endl;
                out << "    sete %al" << endl;
                out << "    movzbq %al, %rax" << endl;
                isIntegerResult = true;
            }
            break;
        case UMINUS_OP: //Aqui dependiendo si es entero o float tomamos un caso en especifico, en el caso del uminus necesitamos transformar el float a entero para poder operar
            if (isIntegerResult) {
                out << "    negq %rax" << endl;
            } else {
                out << "    movsd .L_neg_zero(%rip), %xmm1" << endl;
                out << "    xorpd %xmm1, %xmm0" << endl;
            }
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(NumberExp* exp) {
    if (exp->has_f) { //Si es un float, tenemos que recuperarlo de las constantes
        string constantLabel = getFloatConstant((double)exp->value);
        out << "    movsd " << constantLabel << "(%rip), %xmm0" << endl;
        isIntegerResult = false;
    } else { //Si no seguimos el proceso normal
        out << "    movq $" << exp->value << ", %rax" << endl;
        isIntegerResult = true;
    }
    return 0;
}

int GenCodeVisitor::visit(DecimalExp* exp) { //Guardamos el float en una constante
    string constantLabel = getFloatConstant(exp->value);
    out << "    movsd " << constantLabel << "(%rip), %xmm0" << endl;
    isIntegerResult = false;
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    out << "    movq $" << exp->value << ", %rax" << endl;
    isIntegerResult = true;
    return 0;
}

int GenCodeVisitor::visit(IdentifierExp* exp) {
    bool isFloat = (varTypes.count(exp->name) && varTypes[exp->name] == "Float"); // Verificamos si la variable es de tipo float y existe
    
    if (memoriaGlobal.count(exp->name)) { // Si la variable es global
        if (arrayInfo.count(exp->name)) { //Si tenemos el caso de un array
            out << "    movq " << exp->name << "(%rip), %rax" << endl;
            isIntegerResult = true; // Es una dirección (puntero)
        } else if (isFloat) { // Si es float, movemos el valor de la variable global a xmm0
            out << "    movsd " << exp->name << "(%rip), %xmm0" << endl;
            isIntegerResult = false;
        } else { // Si es entero, movemos el valor de la variable global a rax
            out << "    movq " << exp->name << "(%rip), %rax" << endl;
            isIntegerResult = true;
        }
    } else { // Si la variable es local (en el stack)
        if (isFloat) { // Si es float, movemos el valor de la variable local a xmm0
            out << "    movsd " << memoria[exp->name] << "(%rbp), %xmm0" << endl;
            isIntegerResult = false;
        } else {
            out << "    movq " << memoria[exp->name] << "(%rbp), %rax" << endl;
            isIntegerResult = true;
        }
    }
    return 0;
}

int GenCodeVisitor::visit(RangeExp* exp) {
    return 0;
}

void GenCodeVisitor::visit(AssignStatement* stm) {
    stm->rhs->accept(this);
    
    bool isFloat = (varTypes.count(stm->id) && varTypes[stm->id] == "Float"); // Verificamos si la variable es de tipo float
    
    if (memoriaGlobal.count(stm->id)) { // Si la variable es global
        if (isFloat) { // Si es float, movemos el valor de xmm0 a la variable global
            out << "    movsd %xmm0, " << stm->id << "(%rip)" << endl;
        } else { // Si es entero, movemos el valor de rax a la variable global
            out << "    movq %rax, " << stm->id << "(%rip)" << endl;
        }
    } else { // Si la variable es local (en el stack)
        if (isFloat) {
            out << "    movsd %xmm0, " << memoria[stm->id] << "(%rbp)" << endl;
        } else {
            out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
        }
    }
}

void GenCodeVisitor::visit(PlusAssignStatement* stm) {
    bool isFloat = (varTypes.count(stm->id) && varTypes[stm->id] == "Float");
    
    if (isFloat) {
        if (memoriaGlobal.count(stm->id)) { //Si es que la variable es de tipo float y es global
            out << "    movsd " << stm->id << "(%rip), %xmm0" << endl;
        } else { // Si es local
            out << "    movsd " << memoria[stm->id] << "(%rbp), %xmm0" << endl;
        }
        out << "    movsd %xmm0, %xmm1" << endl; 
        
        stm->rhs->accept(this);  
        out << "    addsd %xmm1, %xmm0" << endl;  //Le añadimos el valor que extrajimos
        
        if (memoriaGlobal.count(stm->id)) { // Si es que la variable es de tipo float y es global
            out << "    movsd %xmm0, " << stm->id << "(%rip)" << endl;
        } else { // Si es local
            out << "    movsd %xmm0, " << memoria[stm->id] << "(%rbp)" << endl;
        }
    } else {
        if (memoriaGlobal.count(stm->id)) {
            out << "    movq " << stm->id << "(%rip), %rax" << endl;
        } else {
            out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
        }
        out << "    pushq %rax" << endl;
        
        stm->rhs->accept(this);
        out << "    movq %rax, %rcx" << endl;
        out << "    popq %rax" << endl;
        out << "    addq %rcx, %rax" << endl; //Se realiza el mismo proceso de extracción que en el caso anterior, pero con los registros regulares
        
        if (memoriaGlobal.count(stm->id)) {
            out << "    movq %rax, " << stm->id << "(%rip)" << endl;
        } else {
            out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
        }
    }
}

void GenCodeVisitor::visit(MinusAssignStatement* stm) { //Mismmo procedimiento, identifica si es float, segun eso busca la variable objetivo y el valor y se lo añade, en el registro correspondiente
    bool isFloat = (varTypes.count(stm->id) && varTypes[stm->id] == "Float");
    
    if (isFloat) {
        if (memoriaGlobal.count(stm->id)) {
            out << "    movsd " << stm->id << "(%rip), %xmm0" << endl;
        } else {
            out << "    movsd " << memoria[stm->id] << "(%rbp), %xmm0" << endl;
        }
        out << "    movsd %xmm0, %xmm1" << endl;
        
        stm->rhs->accept(this); 
        out << "    movsd %xmm1, %xmm2" << endl;
        out << "    subsd %xmm0, %xmm2" << endl;
        out << "    movsd %xmm2, %xmm0" << endl;
        
        if (memoriaGlobal.count(stm->id)) {
            out << "    movsd %xmm0, " << stm->id << "(%rip)" << endl;
        } else {
            out << "    movsd %xmm0, " << memoria[stm->id] << "(%rbp)" << endl;
        }
    } else {
        if (memoriaGlobal.count(stm->id)) {
            out << "    movq " << stm->id << "(%rip), %rax" << endl;
        } else {
            out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
        }
        out << "    pushq %rax" << endl;
        
        stm->rhs->accept(this);
        out << "    movq %rax, %rcx" << endl;
        out << "    popq %rax" << endl;
        out << "    subq %rcx, %rax" << endl;
        
        if (memoriaGlobal.count(stm->id)) {
            out << "    movq %rax, " << stm->id << "(%rip)" << endl;
        } else {
            out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
        }
    }
}

void GenCodeVisitor::visit(ArrayAssignStatement* stm) {
    // Evaluar el valor a asignar
    stm->rhs->accept(this);
    
    ArrayAccessExp* arrayAccess = dynamic_cast<ArrayAccessExp*>(stm->lhs);
    
    bool isFloatArray = false;
    if (IdentifierExp* arrayId = dynamic_cast<IdentifierExp*>(arrayAccess->array)) { //Buscamos el tipo del array para determinar si es float
        if (arrayInfo.count(arrayId->name) && arrayInfo[arrayId->name].first == "Float") {
            isFloatArray = true;
        }
    }
    
    // Guardar el valor en el stack
    if (isFloatArray) { //Convertimos los tipos si es que no concuerdan con el del array
        if (isIntegerResult) {
            out << "    cvtsi2sd %rax, %xmm0" << endl; 
        }
        out << "    subq $8, %rsp" << endl;
        out << "    movsd %xmm0, (%rsp)" << endl;
    } else {
        if (!isIntegerResult) {
            out << "    cvttsd2si %xmm0, %rax" << endl;
        }
        out << "    pushq %rax" << endl;
    }
    
    arrayAccess->array->accept(this);
    out << "    movq %rax, %rbx" << endl; // Recuperamos la direccion del array
    
    arrayAccess->index->accept(this);
    out << "    movq %rax, %rcx" << endl;
    out << "    imulq $8, %rcx" << endl;
    out << "    addq %rcx, %rbx" << endl; //Generamos un nuevo espacio, y a partir de la posicion encontrada nos vamos a esa ubicacion desde la base
    
    if (isFloatArray) { //Movemos el elemento a la direccion calculada (Base Address + Offset)
        out << "    movsd (%rsp), %xmm0" << endl;
        out << "    addq $8, %rsp" << endl;
        out << "    movsd %xmm0, (%rbx)" << endl;
    } else {
        out << "    popq %rax" << endl;
        out << "    movq %rax, (%rbx)" << endl;
    }
}

void GenCodeVisitor::visit(PrintStatement* stm) {
    stm->e->accept(this);
    
    if (isIntegerResult) { //Si el resultado es un entero se printea de forma regular y de los registros de siempre
        out << "    movq %rax, %rsi" << endl;
        out << "    leaq print_fmt(%rip), %rdi" << endl;
        out << "    movl $0, %eax" << endl;
        out << "    call printf@PLT" << endl;
    } else { //Si no es un entero, es un float, se printea de forma diferente
        out << "    leaq print_float_fmt(%rip), %rdi" << endl;
        out << "    movl $1, %eax" << endl;
        out << "    call printf@PLT" << endl;
    }
}

void GenCodeVisitor::visit(IfStatement* stm) {
    int labelId = labelCounter++;
    
    stm->condition->accept(this);

    if (!isIntegerResult) { //Para los ifs con condiciones de tipo float, convertimos el resultado a entero
        out << "    cvttsd2si %xmm0, %rax" << endl;
    }
    out << "    cmpq $0, %rax" << endl;
    
    // Determinar etiqueta de salto según si hay else if's o else
    if (!stm->elseifs.empty()) {
        out << "    je .else_if_" << labelId << "_0" << endl;
    } else if (stm->els != nullptr) {
        out << "    je .else_" << labelId << endl;
    } else {
        out << "    je .endif_" << labelId << endl;
    }
    
    // Cuerpo del if principal
    stm->then->accept(this);
    out << "    jmp .endif_" << labelId << endl;
    
    // Manejar else if's
    for (size_t i = 0; i < stm->elseifs.size(); i++) {
        out << ".else_if_" << labelId << "_" << i << ":" << endl;
        stm->elseifs[i].first->accept(this);  // Evaluar condición else if
        
        if (!isIntegerResult) {
            out << "    cvttsd2si %xmm0, %rax" << endl;
        }
        out << "    cmpq $0, %rax" << endl;
        
        if (i + 1 < stm->elseifs.size()) {
            out << "    je .else_if_" << labelId << "_" << (i + 1) << endl;
        } else if (stm->els != nullptr) {
            out << "    je .else_" << labelId << endl;
        } else {
            out << "    je .endif_" << labelId << endl;
        }
        
        stm->elseifs[i].second->accept(this);  // Cuerpo del else if
        out << "    jmp .endif_" << labelId << endl;
    }
    
    // Cuerpo del else final si existe
    if (stm->els != nullptr) {
        out << ".else_" << labelId << ":" << endl;
        stm->els->accept(this);
    }
    
    out << ".endif_" << labelId << ":" << endl;
}

void GenCodeVisitor::visit(WhileStatement* stm) {
    int labelId = labelCounter++;
    
    out << ".while_start_" << labelId << ":" << endl;
    stm->condition->accept(this);
    if (!isIntegerResult) {
        out << "    cvttsd2si %xmm0, %rax" << endl;
    }
    out << "    cmpq $0, %rax" << endl;
    out << "    je .while_end_" << labelId << endl;
    
    stm->b->accept(this);
    out << "    jmp .while_start_" << labelId << endl;
    out << ".while_end_" << labelId << ":" << endl;
}

void GenCodeVisitor::visit(ForStatement* stm) { //Verirficamos si es que el rango es de tipo rango, si usa un id o un metodo de arrays
    if (RangeExp* range = dynamic_cast<RangeExp*>(stm->range)) { // Manejar for (i in range)
        int labelId = labelCounter++;

        // Evaluar y guardar start y end
        range->start->accept(this);
        offset -= 8;
        int startOffset = offset;
        out << "    movq %rax, " << startOffset << "(%rbp)" << endl;

        range->end->accept(this);
        offset -= 8;
        int endOffset = offset;
        out << "    movq %rax, " << endOffset << "(%rbp)" << endl;
        
        int stepValue = 1;
        int stepOffset = 0;
        if (range->step != nullptr) { //si es que estamos definiendo un step distinto de 1
            range->step->accept(this); //genero el valor de ese step
            offset -= 8; 
            stepOffset = offset;
            out << "    movq %rax, " << stepOffset << "(%rbp)" << endl; //lo guardo en memoria
        }
        
        // variable del loop
        if (memoria.find(stm->id) == memoria.end()) {
            offset -= 8;
            memoria[stm->id] = offset;
            varTypes[stm->id] = "Int";
        }
        
        out << "    movq " << startOffset << "(%rbp), %rax" << endl;
        out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
        
        out << ".for_start_" << labelId << ":" << endl;
        
        out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
        out << "    movq " << endOffset << "(%rbp), %rcx" << endl;
        out << "    cmpq %rcx, %rax" << endl;
        
        if (range->type == RANGE_DOTDOT) {
            //i <= end
            out << "    jg .for_end_" << labelId << endl;
        } else if (range->type == RANGE_UNTIL) {
            //i < end
            out << "    jge .for_end_" << labelId << endl;
        } else if (range->type == RANGE_DOWNTO) {
            // i >= end
            out << "    jl .for_end_" << labelId << endl;
        }
        
        stm->body->accept(this);
        
        out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
        
        if (range->type == RANGE_DOWNTO) {
            // Para downTo: decrementar
            if (range->step != nullptr) { //si es que tomamos un step distinto a uno, lo recuperamos del stepOffSet
                out << "    movq " << stepOffset << "(%rbp), %rcx" << endl;
                out << "    subq %rcx, %rax" << endl;
            } else {
                out << "    decq %rax" << endl;  // so reducimos en 1 como siempre
            }
        } else {
            if (range->step != nullptr) { //misma logica, si es que tenemos un step que no es 1, lo recuperamos, sino añadimos 1
                out << "    movq " << stepOffset << "(%rbp), %rcx" << endl;
                out << "    addq %rcx, %rax" << endl;
            } else {
                out << "    incq %rax" << endl;
            }
        }
        
        out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
        
        out << "    jmp .for_start_" << labelId << endl;
        out << ".for_end_" << labelId << ":" << endl;
    } else if (IdentifierExp* arrayId = dynamic_cast<IdentifierExp*>(stm->range)) { // Manejar for (i in arr)
        int labelId = labelCounter++;
        
        if (memoria.find(stm->id) == memoria.end()) {
            offset -= 8;
            memoria[stm->id] = offset;
            varTypes[stm->id] = getArrayElementType(arrayId->name);
        }
        
        // Variable para el índice del loop
        offset -= 8;
        int indexOffset = offset;
        
        // Inicializar índice en 0
        out << "    movq $0, %rax" << endl;
        out << "    movq %rax, " << indexOffset << "(%rbp)" << endl;
        
        out << ".for_start_" << labelId << ":" << endl;
        
        // Verificar si el índice es menor que el tamaño del array
        out << "    movq " << indexOffset << "(%rbp), %rax" << endl;
        if (arrayInfo.count(arrayId->name)) {
            out << "    cmpq $" << arrayInfo[arrayId->name].second << ", %rax" << endl;
        } else {
            out << "    cmpq $0, %rax" << endl;
        }
        out << "    jge .for_end_" << labelId << endl;
        
        arrayId->accept(this);
        out << "    movq %rax, %rbx" << endl;
        out << "    movq " << indexOffset << "(%rbp), %rcx" << endl;
        out << "    imulq $8, %rcx" << endl;
        out << "    addq %rcx, %rbx" << endl;
        
        if (arrayInfo.count(arrayId->name) && arrayInfo[arrayId->name].first == "Float") {
            out << "    movsd (%rbx), %xmm0" << endl;
            out << "    movsd %xmm0, " << memoria[stm->id] << "(%rbp)" << endl;
        } else {
            out << "    movq (%rbx), %rax" << endl;
            out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
        }
        
        // Ejecutar el cuerpo del loop
        stm->body->accept(this);
        
        // Incrementar índice
        out << "    movq " << indexOffset << "(%rbp), %rax" << endl;
        out << "    incq %rax" << endl;
        out << "    movq %rax, " << indexOffset << "(%rbp)" << endl;
        
        out << "    jmp .for_start_" << labelId << endl;
        out << ".for_end_" << labelId << ":" << endl;
    } else if (ArrayMethodExp* arrayMethod = dynamic_cast<ArrayMethodExp*>(stm->range)) {        // Manejar for (i in arr.indices)
        if (arrayMethod->method == ArrayMethodType::INDICES) {
            int labelId = labelCounter++;
            
            // Variable del loop para el índice
            if (memoria.find(stm->id) == memoria.end()) {
                offset -= 8;
                memoria[stm->id] = offset;
                varTypes[stm->id] = "Int";
            }
            
            // Inicializar índice en 0
            out << "    movq $0, %rax" << endl;
            out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
            
            out << ".for_start_" << labelId << ":" << endl;
            
            // Verificar si el índice es menor que el tamaño del array
            out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
            
            // Obtener el tamaño del array
            IdentifierExp* idExp = dynamic_cast<IdentifierExp*>(arrayMethod->array);
            if (idExp && arrayInfo.count(idExp->name)) {
                out << "    cmpq $" << arrayInfo[idExp->name].second << ", %rax" << endl;
            } else {
                out << "    cmpq $0, %rax" << endl;
            }
            out << "    jge .for_end_" << labelId << endl;
            
            // Ejecutar el cuerpo del loop
            stm->body->accept(this);
            
            // Incrementar índice
            out << "    movq " << memoria[stm->id] << "(%rbp), %rax" << endl;
            out << "    incq %rax" << endl;
            out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
            
            out << "    jmp .for_start_" << labelId << endl;
            out << ".for_end_" << labelId << ":" << endl;
        }
    }
}

void GenCodeVisitor::visit(VarDec* stm) {
    varTypes[stm->id] = stm->type;
    
    if (!entornoFuncion) {
        memoriaGlobal[stm->id] = true;
    } else {
        offset -= 8;
        memoria[stm->id] = offset;
        
        if (stm->value != nullptr) {
            // si la variable es un array, registro en arrayInfo
            if (ArrayExp* arr = dynamic_cast<ArrayExp*>(stm->value)) {
                allocateArray(stm->id, arr->elements.size(), arr->type);
                arr->accept(this); //Creamos el array y lo generamo, ademas de guardarlo en la direcion base
                out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
            } else if (stm->type == "Float") {
                stm->value->accept(this);
                out << "    movsd %xmm0, " << memoria[stm->id] << "(%rbp)" << endl;
            } else {
                stm->value->accept(this);
                out << "    movq %rax, " << memoria[stm->id] << "(%rbp)" << endl;
            }
        }
        
        if (stm->type.find("Array<") == 0) {
            size_t start = stm->type.find('<') + 1;
            size_t end = stm->type.find('>');
            if (start != string::npos && end != string::npos && end > start) {
                string elementType = stm->type.substr(start, end - start);
                // Si no fue registrado antes y no tenemos el valor
                if (arrayInfo.find(stm->id) == arrayInfo.end() && stm->value == nullptr) {
                    // Para arrays sin inicialización, registrar con tamaño 0 
                    allocateArray(stm->id, 0, elementType);
                }
            }
        }
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
        auto paramIt = fundec->parametros.begin();
        auto typeIt = fundec->tipos_parametros.begin();
        
        while (paramIt != fundec->parametros.end() && typeIt != fundec->tipos_parametros.end()) {
            string paramName = *paramIt;
            string paramType = *typeIt;
            
            varTypes[paramName] = paramType;
            
            if (paramIndex < argRegs.size()) {
                memoria[paramName] = offset;
                out << "    movq " << argRegs[paramIndex] << ", " << offset << "(%rbp)" << endl;
                offset -= 8;
                paramIndex++;
            }
            
            ++paramIt;
            ++typeIt;
        }
    }
    out << "    subq $256, %rsp" << endl;
    if (fundec->cuerpo) {
        if (fundec->cuerpo->vardecs) {
            fundec->cuerpo->vardecs->accept(this);
        }
        
        if (fundec->cuerpo->slist) {
            fundec->cuerpo->slist->accept(this);
        }
    }

    out << ".end_" << fundec->nombre << ":" << endl;
    if (fundec->nombre == "main") {
        out << "    movq $0, %rax" << endl;
    }
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
    
    isIntegerResult = true;
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
        if (!isIntegerResult) {
            //TODO
        }
    }
    
    out << "    jmp .end_" << currentFunction << endl;
}

bool GenCodeVisitor::needsFloatOperation(BinaryExp* exp) { // Verifica si una operación binaria necesita ser tratada como float o no, revisando ambos lados de la operación y si alguno es float, lo retorna como true
    if (dynamic_cast<DecimalExp*>(exp->left)) return true;
    if (dynamic_cast<DecimalExp*>(exp->right)) return true;
    
    if (NumberExp* num = dynamic_cast<NumberExp*>(exp->left)) {
        if (num->has_f) return true;
    }
    if (NumberExp* num = dynamic_cast<NumberExp*>(exp->right)) {
        if (num->has_f) return true;
    }
    
    if (IdentifierExp* id = dynamic_cast<IdentifierExp*>(exp->left)) {
        if (varTypes.count(id->name) && varTypes[id->name] == "Float") return true;
    }
    if (IdentifierExp* id = dynamic_cast<IdentifierExp*>(exp->right)) {
        if (varTypes.count(id->name) && varTypes[id->name] == "Float") return true;
    }
    if (ArrayAccessExp* arrayAccess = dynamic_cast<ArrayAccessExp*>(exp->left)) {
        if (IdentifierExp* arrayId = dynamic_cast<IdentifierExp*>(arrayAccess->array)) {
            if (arrayInfo.count(arrayId->name) && arrayInfo[arrayId->name].first == "Float") {
                return true;
            }
        }
    }
    if (ArrayAccessExp* arrayAccess = dynamic_cast<ArrayAccessExp*>(exp->right)) {
        if (IdentifierExp* arrayId = dynamic_cast<IdentifierExp*>(arrayAccess->array)) {
            if (arrayInfo.count(arrayId->name) && arrayInfo[arrayId->name].first == "Float") {
                return true;
            }
        }
    }

    if (BinaryExp* binLeft = dynamic_cast<BinaryExp*>(exp->left)) {
        if (needsFloatOperation(binLeft)) return true;
    }
    if (BinaryExp* binRight = dynamic_cast<BinaryExp*>(exp->right)) {
        if (needsFloatOperation(binRight)) return true;
    }
    
    return false;
}

std::string GenCodeVisitor::getFloatConstant(double value) {
    for (auto it = floatConstants.begin(); it != floatConstants.end(); ++it) { //leemmos el mapa de constantes flotantes y si el valor ya existe, retornamos la etiqueta correspondiente
        if (it->second == value) {
            return it->first;
        }
    }
    
    string label = ".L_float_" + std::to_string(constantCounter++); // Si el valor no existe, creamos una nueva etiqueta
    floatConstants[label] = value; //la guardamos
    return label;
}

void GenCodeVisitor::collectConstantsFromExp(Exp* exp) { // Recorre la expresión y recoge constantes de tipo float o enteros
    if (DecimalExp* decExp = dynamic_cast<DecimalExp*>(exp)) {
        getFloatConstant(decExp->value);
    } else if (NumberExp* numExp = dynamic_cast<NumberExp*>(exp)) {
        if (numExp->has_f) {
            getFloatConstant((double)numExp->value);
        }
    } else if (BinaryExp* binExp = dynamic_cast<BinaryExp*>(exp)) {
        collectConstantsFromExp(binExp->left);
        collectConstantsFromExp(binExp->right);
    } else if (UnaryExp* unExp = dynamic_cast<UnaryExp*>(exp)) {
        collectConstantsFromExp(unExp->e);
    } else if (FCallExp* fcall = dynamic_cast<FCallExp*>(exp)) {
        for (auto arg : fcall->argumentos) {
            collectConstantsFromExp(arg);
        }
    } else if (RangeExp* range = dynamic_cast<RangeExp*>(exp)) {
        collectConstantsFromExp(range->start);
        collectConstantsFromExp(range->end);
        if (range->step != nullptr) {
            collectConstantsFromExp(range->step);
        }
    } else if (ArrayExp* arrayExp = dynamic_cast<ArrayExp*>(exp)) { // Recorre los elementos de un array y recoge constantes
        for (auto element : arrayExp->elements) {
            collectConstantsFromExp(element);
        } 
    } else if (ArrayAccessExp* arrayAccess = dynamic_cast<ArrayAccessExp*>(exp)) { //Si es un acceso de memoria, recupera el valor de los elementos del array como del indice
        collectConstantsFromExp(arrayAccess->array);
        collectConstantsFromExp(arrayAccess->index);
    }
}

void GenCodeVisitor::collectConstantsFromBody(Body* body) { // Recorre el cuerpo y recoge constantes de tipo float o enteros
    if (body->vardecs) {
        for (auto vardec : body->vardecs->decls) {
            if (vardec->value) {
                collectConstantsFromExp(vardec->value);
            }
        }
    }
    
    if (body->slist) {
        for (auto stmt : body->slist->stms) {
            collectConstantsFromStmt(stmt);
        }
    }
}

void GenCodeVisitor::collectConstantsFromStmt(Stm* stmt) { // Recorre una sentencia y recoge constantes de tipo float o enteros
    if (AssignStatement* assign = dynamic_cast<AssignStatement*>(stmt)) {
        collectConstantsFromExp(assign->rhs);
    } else if (PlusAssignStatement* plusAssign = dynamic_cast<PlusAssignStatement*>(stmt)) {
        collectConstantsFromExp(plusAssign->rhs);
    } else if (MinusAssignStatement* minusAssign = dynamic_cast<MinusAssignStatement*>(stmt)) {
        collectConstantsFromExp(minusAssign->rhs);
    } else if (ArrayAssignStatement* arrayAssign = dynamic_cast<ArrayAssignStatement*>(stmt)) {
        collectConstantsFromExp(arrayAssign->lhs);
        collectConstantsFromExp(arrayAssign->rhs);
    } else if (PrintStatement* print = dynamic_cast<PrintStatement*>(stmt)) {
        collectConstantsFromExp(print->e);
    } else if (IfStatement* ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        collectConstantsFromExp(ifStmt->condition);
        collectConstantsFromBody(ifStmt->then);
        for (auto& elseif : ifStmt->elseifs) {
            collectConstantsFromExp(elseif.first);
            collectConstantsFromBody(elseif.second);
        }
        if (ifStmt->els) {
            collectConstantsFromBody(ifStmt->els);
        }
    } else if (WhileStatement* whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        collectConstantsFromExp(whileStmt->condition);
        collectConstantsFromBody(whileStmt->b);
    } else if (ForStatement* forStmt = dynamic_cast<ForStatement*>(stmt)) {
        if (RangeExp* range = dynamic_cast<RangeExp*>(forStmt->range)) {
            collectConstantsFromExp(range->start);
            collectConstantsFromExp(range->end);
            if (range->step != nullptr) {
                collectConstantsFromExp(range->step);
            }
        } else {
            if (forStmt->range) { // Si es un array o método de array, recolectamos los elementos
                collectConstantsFromExp(forStmt->range);
            }
        }
        collectConstantsFromBody(forStmt->body);
    } else if (FCallStm* fcall = dynamic_cast<FCallStm*>(stmt)) {
        for (auto arg : fcall->argumentos) {
            collectConstantsFromExp(arg);
        }
    } else if (ReturnStatement* ret = dynamic_cast<ReturnStatement*>(stmt)) {
        if (ret->e) {
            collectConstantsFromExp(ret->e);
        }
    }
}

int GenCodeVisitor::visit(ArrayMethodExp* exp) {
    // Intentar obtener el nombre del array si es un identificador
    IdentifierExp* idExp = dynamic_cast<IdentifierExp*>(exp->array);
    std::string arrayName = idExp ? idExp->name : "";
    switch (exp->method) {
        case ArrayMethodType::INDICES: {
            // Para .indices, solo devolvemos el tamaño (el rango lo usará el for)
            if (!arrayName.empty() && arrayInfo.count(arrayName)) {
                out << "    movq $" << arrayInfo[arrayName].second << ", %rax" << std::endl;
            } else {
                out << "    movq $0, %rax" << std::endl; // fallback
            }
            break;
        }
        case ArrayMethodType::SIZE: {
            if (!arrayName.empty() && arrayInfo.count(arrayName)) {
                out << "    movq $" << arrayInfo[arrayName].second << ", %rax" << std::endl;
            } else {
                out << "    movq $0, %rax" << std::endl; // fallback
            }
            break;
        }
    }
    isIntegerResult = true;
    return 0;
}


// Implementación para generar código para arrayOf
int GenCodeVisitor::visit(ArrayExp* exp) {
    int num_elements = exp->elements.size();
    int element_size = 8; // Usaremos 8 bytes (quad) por defecto para Int, Float (double) y Boolean

    if (exp->type != "Int" && exp->type != "Float" && exp->type != "Boolean") {
        cerr << "Error: arrayOf solo soporta Int, Float y Boolean." << endl;
        exit(1);
    }

    int total_size = num_elements * element_size;
    
    // Usar offset desde %rbp en lugar de %rsp para evitar corrupción de arrays
    offset -= total_size;
    int array_base_offset = offset;

    // 2. Evaluar y guardar cada elemento en el stack usando offsets desde %rbp
    for (int i = 0; i < num_elements; i++) {
        exp->elements[i]->accept(this); // El resultado de la expresión estará en %rax (si es Int/Bool) o %xmm0 (si es Float)

        int element_offset = array_base_offset + (i * element_size);
        
        if (exp->type == "Float") {
             if (isIntegerResult) { // Si el elemento es un Int (e.g. arrayOf<Float>(1, 2.5))
                out << "    cvtsi2sd %rax, %xmm0" << endl;
             }
             out << "    movsd %xmm0, " << element_offset << "(%rbp)" << endl;
        } else { // Int o Boolean
            if (!isIntegerResult) { // Si el elemento es un Float
                // Aquí podrías decidir truncar el float a int o lanzar un error.
                // Por simplicidad, lo truncamos.
                 out << "    cvttsd2si %xmm0, %rax" << endl;
            }
            out << "    movq %rax, " << element_offset << "(%rbp)" << endl;
        }
    }

    // 3. El resultado de la expresión 'arrayOf' es un puntero al inicio del array.
    //    Calculamos la dirección del array usando el %rbp
    out << "    leaq " << array_base_offset << "(%rbp), %rax" << endl;

    // Marcamos que el resultado es un puntero (un entero en ensamblador).
    isIntegerResult = true;

    return 0;
}

// Genera el código para acceder a un elemento del array: arr[i]
int GenCodeVisitor::visit(ArrayAccessExp* exp) {
    // Evaluar la expresión del array (debe dejar la dirección base en %rax)
    exp->array->accept(this);
    out << "    movq %rax, %rbx" << endl; // Guardar base en %rbx
    // Evaluar el índice
    exp->index->accept(this);
    out << "    movq %rax, %rcx" << endl; // Índice en %rcx
    out << "    imulq $8, %rcx" << endl; // Multiplicar por 8 (tamaño de elemento)
    out << "    addq %rcx, %rbx" << endl; // Sumar desplazamiento

    bool isFloatArray = false;
    if (IdentifierExp* arrayId = dynamic_cast<IdentifierExp*>(exp->array)) {
        if (arrayInfo.count(arrayId->name) && arrayInfo[arrayId->name].first == "Float") {
            isFloatArray = true;
        }
    }
    
    // Cargar el valor según el tipo
    if (isFloatArray) {
        out << "    movsd (%rbx), %xmm0" << endl;
        isIntegerResult = false;
    } else {
        out << "    movq (%rbx), %rax" << endl;
        isIntegerResult = true;
    }
    return 0;
}

// Registramos el array, con su tipo y el tamaño que requiere
void GenCodeVisitor::allocateArray(const std::string& arrayName, int size, const std::string& type) {
    arrayInfo[arrayName] = std::make_pair(type, size);
}

// Devuelve el tipo de elemento de un array por nombre
std::string GenCodeVisitor::getArrayElementType(const std::string& arrayName) {
    if (arrayInfo.count(arrayName)) {
        return arrayInfo[arrayName].first;
    }
    return "Int";
}
