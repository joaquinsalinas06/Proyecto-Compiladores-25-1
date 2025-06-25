#include "codegen.h"
#include "exp.h"
#include <iostream>

using namespace std;

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
