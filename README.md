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

#### Generación de Código Assembly para Floats

**Asignación de Float (var x: Float = 3.14f):**
| Instrucción | Descripción |
|-------------|-------------|
| Genera código para DecimalExp (3.14f) ||
| `movsd .L_float_1(%rip), %xmm0` | Carga el literal 3.14f desde .rodata al registro XMM0 |
| `movsd %xmm0, -8(%rbp)` | Almacena el valor float en la variable x en la pila |

**Conversión Int a Float (operación mixta):**
| Instrucción | Descripción |
|-------------|-------------|
| Genera código para operando Int (intExp->accept(this)) ||
| `movq %rax, -32(%rbp)` | Almacena temporalmente el valor entero |
| Genera código para operando Float (floatExp->accept(this)) ||
| `movsd %xmm0, -24(%rbp)` | Almacena temporalmente el valor float |
| `movq -32(%rbp), %rax` | Recupera el valor entero |
| `cvtsi2sd %rax, %xmm0` | Convierte el entero a float en %xmm0 |
| `movsd -24(%rbp), %xmm1` | Recupera el operando float |
| `addsd %xmm1, %xmm0` | Realiza la operación mixta (Int + Float) |

**Operaciones Aritméticas Float:**
| Instrucción | Descripción |
|-------------|-------------|
| Genera código para operando izquierdo (left->accept(this)) ||
| `movsd %xmm0, -24(%rbp)` | Almacena temporalmente el resultado del operando izquierdo |
| Genera código para operando derecho (right->accept(this)) ||
| `movsd -24(%rbp), %xmm1` | Recupera el operando izquierdo al registro XMM1 |
| `addsd %xmm0, %xmm1` | Suma: %xmm1 = %xmm1 + %xmm0 |
| `subsd %xmm0, %xmm1` | Resta: %xmm1 = %xmm1 - %xmm0 |
| `mulsd %xmm0, %xmm1` | Multiplicación: %xmm1 = %xmm1 * %xmm0 |
| `divsd %xmm0, %xmm1` | División: %xmm1 = %xmm1 / %xmm0 |
| `movsd %xmm1, %xmm0` | Mueve el resultado final a %xmm0 |

**Sección .rodata para constantes float:**
```assembly
.L_float_1:
    .double 3.14159
.L_float_2:
    .double 2.71828
```

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

#### Generación de Código Assembly para Arrays

**Declaración de Array (arrayOf<Int>(1, 2, 3)):**
| Instrucción | Descripción |
|-------------|-------------|
| `subq $24, %rsp` | Reserva espacio en la pila para 3 elementos (3 * 8 bytes) |
| Genera código para elemento 1 (elements[0]->accept(this)) ||
| `movq %rax, -8(%rbp)` | Almacena el primer elemento en posición 0 |
| Genera código para elemento 2 (elements[1]->accept(this)) ||
| `movq %rax, -16(%rbp)` | Almacena el segundo elemento en posición 1 |
| Genera código para elemento 3 (elements[2]->accept(this)) ||
| `movq %rax, -24(%rbp)` | Almacena el tercer elemento en posición 2 |
| `leaq -8(%rbp), %rax` | Carga la dirección base del array en %rax |
| `movq %rax, -32(%rbp)` | Almacena la dirección del array en la variable |
| `movq $3, -40(%rbp)` | Almacena el tamaño del array (metadato) |

**Acceso a Elementos (array[index]):**
| Instrucción | Descripción |
|-------------|-------------|
| Genera código para el array (array->accept(this)) ||
| `movq %rax, -48(%rbp)` | Almacena temporalmente la dirección del array |
| Genera código para el índice (index->accept(this)) ||
| `movq %rax, %rbx` | Mueve el índice calculado a %rbx |
| `movq -48(%rbp), %rax` | Recupera la dirección base del array |
| `imulq $8, %rbx` | Multiplica índice por 8 (tamaño de palabra en bytes) |
| `addq %rbx, %rax` | Calcula dirección: base + (índice * 8) |
| `movq (%rax), %rax` | Desreferencia para obtener el valor del elemento |

**Asignación a Elementos (array[index] = value):**
| Instrucción | Descripción |
|-------------|-------------|
| Genera código para el array (arrayAccess->array->accept(this)) ||
| `movq %rax, -48(%rbp)` | Almacena temporalmente la dirección del array |
| Genera código para el índice (arrayAccess->index->accept(this)) ||
| `movq %rax, %rbx` | Mueve el índice a %rbx |
| `movq -48(%rbp), %rax` | Recupera la dirección del array |
| `imulq $8, %rbx` | Multiplica índice por tamaño de elemento |
| `addq %rbx, %rax` | Calcula dirección del elemento destino |
| `movq %rax, -56(%rbp)` | Almacena la dirección destino |
| Genera código para el valor (rhs->accept(this)) ||
| `movq %rax, %rbx` | Mueve el valor a asignar a %rbx |
| `movq -56(%rbp), %rax` | Recupera la dirección destino |
| `movq %rbx, (%rax)` | Almacena el valor en la posición calculada |

**Método .size:**
| Instrucción | Descripción |
|-------------|-------------|
| Genera código para el array (array->accept(this)) ||
| `movq -40(%rbp), %rax` | Carga el tamaño almacenado del array (metadato) |

**Método .indices:**
| Instrucción | Descripción |
|-------------|-------------|
| Genera código para el array (array->accept(this)) ||
| `movq -40(%rbp), %rcx` | Carga el tamaño del array original |
| `imulq $8, %rcx` | Calcula espacio necesario (tamaño * 8) |
| `subq %rcx, %rsp` | Reserva espacio para el array de índices |
| `movq $0, %rax` | Inicializa contador en 0 |
| **loop_indices:** | Etiqueta de inicio del bucle |
| `cmpq -40(%rbp), %rax` | Compara contador con tamaño original |
| `jge end_indices` | Si contador >= tamaño, termina |
| `movq %rax, -64(%rbp, %rax, 8)` | Almacena índice actual en array resultado |
| `incq %rax` | Incrementa contador |
| `jmp loop_indices` | Repite el bucle |
| **end_indices:** | Etiqueta de fin del bucle |
| `leaq -64(%rbp), %rax` | Retorna dirección del array de índices |

---

## Uso del Compilador

### Sistema de Construcción Automatizado

El proyecto incluye un archivo (`make.py`) que automatiza la compilación, ejecución y gestión de pruebas del compilador.

#### Uso Básico

```bash
# Mostrar ayuda completa
python make.py help

# Ejecutar todas las pruebas
python make.py all

# Ejecutar categoría específica
python make.py floats

# Ejecutar archivo individual
python make.py test_ejemplo.txt
```

#### Opciones Avanzadas

```bash
# Ejecutar solo pasos específicos
python make.py --steps=1,3 floats        # Solo scanner y print visitor
python make.py --steps=5 all             # Solo generación de assembly
python make.py --steps=4 arrays          # Solo evaluación

# Gestión de archivos generados
python make.py --clean                   # Limpiar todos los .s
python make.py --clean=floats            # Limpiar solo floats

# Omitir recompilación
python make.py --no-compile vars         # Ejecutar sin compilar
```

#### Integración con WSL

Si es que se ejecuta en Windows y se quiere probar los archivos generados por gencode en Ubuntu:

```bash
# Generar assembly y script de ejecución para WSL
python make.py --steps=5 --copy-wsl=/mnt/c/Users/tu_usuario/wsl_folder all

# Luego en WSL Ubuntu:
cd /mnt/c/Users/tu_usuario/wsl_folder
./build_all.sh                          # Script auto-generado con permisos correctos
```

#### Pasos de Compilación Disponibles

1. **Scanner** - Análisis léxico
2. **Parser** - Análisis sintáctico  
3. **Print Visitor** - Impresión del AST
4. **Eval Visitor** - Evaluación/interpretación
5. **Assembly Generation** - Generación de código assembly


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
