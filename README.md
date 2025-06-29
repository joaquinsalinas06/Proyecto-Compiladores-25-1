# Compilador Kotlin-like - Proyecto de Compiladores

## Integrantes 

| Nombre              | Codigo UTEC                                           |  Email                                                                |
|-------------------|----------------------------------------------------------|-----------------------------------------------------------------------|
| Joaquin Mauricio Salinas Salas  | 202210604 |[joaquin.salinas@utec.edu.pe](mailto:joaquin.salinas@utec.edu.pe)      |
| Gilver Alexis Raza Estrada  | 202020129 |[gilver.raza@utec.edu.pe](mailto:gilver.raza@utec.edu.pe)          |


## Resumen del Proyecto

Este compilador busca reconocer el lenguaje de programación Kotlin, y mediante un proceso de analisis sintactico, lexico y semantico, poder obtener un resultado, ademas de permitir la generacion de codigo Assembly para su ejecucion. Este compilador permite el uso de funciones, asignacion de variables, condicionales, bucles y la capacidad de soportar Floats junto a Arrays.

## Gramática del Lenguaje

```ebnf
• Program ::= { VarDec | FunDec }*
• VarDec ::= 'var' ID ':' Type [ '=' AExp ]
• FunDec ::= 'fun' ID '(' ParamList? ')' [ ':' Type ] '{' Body '}'
• ParamList ::= Param ( ',' Param )*
• Param ::= ID ':' Type
• Body ::= { VarDec | Statement }*

• Type ::= 'Int' | 'Float' | 'Boolean' | 'Unit' | 'Array' '<' Type '>'

• Statement ::= AssignStatement | PlusAssignStatement | MinusAssignStatement 
              | ArrayAssignStatement | PrintStatement | IfStatement 
              | WhileStatement | ForStatement | FCallStm | ReturnStatement

• AssignStatement ::= ID '=' AExp | ArrayAccess '=' AExp
• PlusAssignStatement ::= ID '+=' AExp
• MinusAssignStatement ::= ID '-=' AExp
• ArrayAssignStatement ::= ArrayAccess '=' AExp
• PrintStatement ::= ( 'print' | 'println' ) '(' AExp ')'
• IfStatement ::= 'if' '(' AExp ')' '{' Body '}' 
                  { 'else' 'if' '(' AExp ')' '{' Body '}' }*
                  [ 'else' '{' Body '}' ]
• WhileStatement ::= 'while' '(' AExp ')' '{' Body '}'
• ForStatement ::= 'for' '(' [ Type ] ID 'in' RangeExp ')' '{' Body '}'
• FCallStm ::= ID '(' ArgList? ')'
• ReturnStatement ::= 'return' [ AExp ]
• ArgList ::= AExp ( ',' AExp )*

• AExp ::= CExp { ( '&&' | '||' ) CExp }*
• CExp ::= Expression [ ( '<' | '<=' | '>' | '>=' | '==' | '!=' ) Expression ]
• Expression ::= Term { ( '+' | '-' ) Term }*
• Term ::= Unary { ( '*' | '/' ) Unary }*
• Unary ::= [ '!' ] Factor
• Factor ::= '(' AExp ')' | NUM | DECIMAL | 'true' | 'false' | ArrayLiteral 
           | ID [ '(' ArgList? ')' | ArrayAccess | ArrayMethod ] | FCallExp

• FCallExp ::= ID '(' ArgList? ')'
• ArrayLiteral ::= 'arrayOf' '<' Type '>' '(' ElementList? ')'
• ElementList ::= AExp ( ',' AExp )*
• ArrayAccess ::= '[' AExp ']'
• ArrayMethod ::= '.' ( 'size' | 'indices' )
• RangeExp ::= AExp ( '..' | 'until' | 'downTo' ) AExp [ 'step' AExp ]

• ID ::= [a-zA-Z_][a-zA-Z0-9_]*
• NUM ::= [0-9]+ [ 'f' ]?
• DECIMAL ::= [0-9]+ '.' [0-9]+ 'f'
```
## Extensiones Implementadas

### 1. **Soporte para Números Float**

#### Clases Elaboradas

La implementación del soporte para números de punto flotante se centra en la clase **`DecimalExp`**, que extiende la jerarquía de expresiones para representar literales decimales. Esta clase encapsula un valor `float` y sigue el patrón Visitor establecido en el compilador, permitiendo que cada visitor especializado maneje los números decimales de manera específica según su propósito.
```cpp
class DecimalExp : public Exp {
public:
    float value;
    DecimalExp(float v);
    int accept(Visitor* visitor);
};
```


#### Operaciones Implementadas

El reconocimiento de valores decimales en la fase de **parseo** requiere que todos los números de punto flotante terminen con el sufijo obligatorio 'f', siguiendo la convención de Kotlin. Esta decisión elimina ambigüedades en el análisis léxico y hace explícito el uso de aritmética en operaciones con floats.

Durante la **evaluación**, el compilador implementa aritmética de punto flotante con precisión completa, haciendo un soporte tanto a operaciones de tipo int o float, y permitiendo una compatibilidad entre ambas. El EVALVisitor maneja las operaciones entre tipos mixtos promoviendo automáticamente los valores Int a Float cuando es necesario.

En el TypeChecker establece compatibilidad bidireccional entre Int y Float en operaciones aritméticas, donde Int puede transformarse implícitamente a Float, pero no viceversa. Esta implementación previene pérdida de datos mientras mantiene flexibilidad en las operaciones matemáticas.

La **generación de código** utiliza registros XMM y el conjunto de instrucciones SSE para operaciones de punto flotante. Estos registros XMM son registros especiales introducidos a Assembly, con una capacidad de 128 bits, lo cual permite almacenar los valores junto con los 6 a 7 decimales que se traen los float. Los valores float se almacenan en la sección `.rodata` del assembly generado, optimizando el acceso a memoria.

#### Estructura de Visitors
```cpp
int PrintVisitor::visit(DecimalExp* exp) {
    cout << exp->value << "f";
    return 0;
}

int EVALVisitor::visit(DecimalExp* exp) {
    lastType = 2; // FLOAT_TYPE
    lastFloat = exp->value;
    return lastType;
}

int TypeChecker::visit(DecimalExp* exp) {
    return FLOAT_TYPE; // Tipo 2
}

int GenCodeVisitor::visit(DecimalExp* exp) {
    out << "    movsd .L_float_" << float_counter << "(%rip), %xmm0" << endl;
    // Generar valor en sección .rodata
    return 0;
}
```

#### Diseño Específico
En la evaluación de los inputs junto con la generación de código, fijamos que las operaciones puedan usar hasta 6 decimales, esto permite que casos donde los resultados sean ... .99 no se haga un redondeo a enteros, sino que se permita la manipulación precisa de todos los dígitos. 

Otro elemento previamente mencionado es la compatibilidad de tipos, si quisiéramos sumar un valor entero con uno de tipo Float, el resultado es conveniente que resulte float, pero no viceversa, por lo que nos permite la facilidad de realizar operaciones. 

Además de ello, se fuerza a que todos los ejemplos con Floats terminen en f, de tal forma reconocemos de forma adecuada cuando estamos a un caso de tipo float o no.

### 2. **Soporte para Arrays**

#### Clases Elaboradas

La implementación de arrays se basa en un conjunto de cuatro clases que cubren todas las operaciones necesarias para el manejo de estructuras de datos. La clase **`ArrayExp`** constituye las bases de la declaración de arrays, almacenando tanto el tipo explícito de elementos como un vector de expresiones que representan los valores iniciales.

```cpp
class ArrayExp : public Exp {
public:
    string type;  // "Int", "Float", "Boolean"
    vector<Exp*> elements;
    ArrayExp(const string& type, vector<Exp*>& elements);
};
```

La clase **`ArrayAccessExp`** maneja el acceso a elementos individuales mediante indexación, encapsulando tanto la expresión que representa el array como la expresión del índice. Esta separación permite que los índices sean expresiones complejas evaluadas dinámicamente.

```cpp
class ArrayAccessExp : public Exp {
public:
    Exp* array;
    Exp* index;
    ArrayAccessExp(Exp* array, Exp* index);
};
```

Para proporcionar funcionalidad adicional similar a Kotlin, la clase **`ArrayMethodExp`** implementa métodos incorporados como `.size` e `.indices`, utilizando un enum para distinguir entre diferentes tipos de métodos y manteniendo extensibilidad futura.

```cpp
enum class ArrayMethodType { SIZE, INDICES };
class ArrayMethodExp : public Exp {
public:
    Exp* array;
    ArrayMethodType method;
};
```

Finalmente, **`ArrayAssignStatement`** permite la modificación de elementos específicos del array después de su declaración, distinguiendo entre la expresión de acceso (lado izquierdo) y el valor a asignar (lado derecho).

#### Operaciones Implementadas

El sistema de arrays soporta un conjunto completo de operaciones que abarcan desde la **declaración** inicial con sintaxis `arrayOf<Type>(elem1, elem2, ...)` hasta el **acceso** directo mediante `array[index]` y la **asignación** de nuevos valores con `array[index] = value`.

Los **métodos incorporados** `array.size` y `array.indices` proporcionan funcionalidad adicional para iteración y validación de límites. El método `.size` devuelve un valor Int representando el número de elementos, mientras que `.indices` genera un `arrayOf<Int>` conteniendo todos los índices válidos del array.

La **iteración** sobre arrays se facilita mediante construcciones como `for (item in array.indices)` o `for (item in 0..array.size-1)`, integrándose naturalmente con el sistema de rangos del lenguaje para proporcionar patrones de acceso seguros y eficientes.

#### Estructura de Visitors

```cpp
// TypeChecker - Sistema de tipos para arrays
int TypeChecker::visit(ArrayExp* exp) {
    // Verificar homogeneidad de elementos
    for (auto e : exp->elements) {
        int t = e->accept(this);
        // Validar que todos los elementos sean del mismo tipo
    }
    if (allInt) return 5;      // arrayOf<Int>
    if (allFloat) return 6;    // arrayOf<Float>
    if (allBool) return 7;     // arrayOf<Boolean>
}

// GenCodeVisitor - Generación de código para arrays
int GenCodeVisitor::visit(ArrayExp* exp) {
    // Alocar espacio en stack para elementos
    // Inicializar cada elemento secuencialmente
    for (size_t i = 0; i < exp->elements.size(); ++i) {
        exp->elements[i]->accept(this);
        // Almacenar en offset correspondiente
        out << "    movq %rax, -" << (base_offset + i * 8) << "(%rbp)" << endl;
    }
}

// EVALVisitor - Evaluación de arrays en memoria
int EVALVisitor::visit(ArrayAccessExp* exp) {
    int arrType = exp->array->accept(this);
    int idxType = exp->index->accept(this);
    // Acceder al elemento en el vector correspondiente
    if (arrType == 5) { // arrayOf<Int>
        lastInt = lastIntArray[lastInt];
        return 1;
    }
    // Similar para Float y Boolean arrays
}
```

#### Diseño Específico

El sistema de arrays fuerza a que este sea de un tipado único, donde todos los elementos de un array deben pertenecer al mismo tipo declarado. Esta restricción se verifica durante la fase de análisis semántico, rechazando arrays con elementos mixtos y garantizando coherencia de tipos en tiempo de compilación.

La indexación utiliza **índices base cero**, donde `array[0]` representa el primer elemento. Esta convención se mantiene consistente tanto en el acceso directo como en los métodos `.indices`.

Los **métodos incorporados** forman parte integral del sistema de tipos: `.size` devuelve un valor Int que representa la cardinalidad del array, mientras que `.indices` produce un `Array<Int>` conteniendo la secuencia `[0, 1, 2, ..., size-1]`. Esta implementación permite una iteración más segura sobre todo un elemento del array, además de poder realizar operaciones más complejas en el futuro con los atributos dados.

---

## Uso del Compilador

### Compilación
```bash
g++ -std=c++11 -o compiler main.cpp scanner.cpp parser.cpp typechecker.cpp visitor.cpp codegen.cpp exp.cpp token.cpp
```

### Ejecución
```bash
./compiler archivo.txt
```

### Ejemplo de Código Soportado
```kotlin
var numbers: Array<Float> = arrayOf<Float>(1.5f, 2.7f, 3.14f)
var sum: Float = 0.0f

fun main() {
    for (i in 0..numbers.size-1) {
        sum += numbers[i]
    }
    println(sum)
}
```
