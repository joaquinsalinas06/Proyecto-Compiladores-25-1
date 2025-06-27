#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "visitor.h"
#include "codegen.h"
#include "typechecker.h"

using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cout << "Numero incorrecto de argumentos. Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        exit(1);
    }

    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        exit(1);
    }

    string input;
    string line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    Scanner scanner(input.c_str());

    string input_copy = input;
    Scanner scanner_test(input_copy.c_str());
    test_scanner(&scanner_test);
    cout << "Scanner exitoso" << endl;
    cout << endl;
    cout << "Iniciando parsing:" << endl;
    Parser parser(&scanner); 
    try {
        Program* program = parser.parseProgram();
        cout << "Parsing exitoso" << endl << endl;
        
        cout << "Iniciando Visitor:" << endl;
        PrintVisitor printVisitor;
        EVALVisitor evalVisitor;
        // TypeChecker typeChecker;
        
        // cout << "VERIFICANDO TIPOS:" << endl;
        // typeChecker.check(program);
        // cout << "Verificación de tipos exitosa" << endl;
        
        cout << endl;
        cout << "IMPRIMIR:" << endl;
        printVisitor.imprimir(program);        cout  << endl;
        cout << "EJECUTAR:" << endl;
        evalVisitor.ejecutar(program);
        
        // Generar código assembly
        cout << endl << "GENERANDO CODIGO ASSEMBLY:" << endl;
        string inputFile(argv[1]);
        size_t dotPos = inputFile.find_last_of('.');
        string baseName = (dotPos == string::npos) ? inputFile : inputFile.substr(0, dotPos);
        string outputFilename = baseName + ".s";
        ofstream outfile(outputFilename);
        if (!outfile.is_open()) {
            cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
            return 1;
        }
        cout << "Generando codigo ensamblador en " << outputFilename << endl;
        GenCodeVisitor genCodeVisitor(outfile);
        genCodeVisitor.generar(program);
        outfile.close();
        cout << "Codigo assembly generado exitosamente." << endl;
        
        delete program;
    } catch (const exception& e) {
        cout << "Error durante la ejecución: " << e.what() << endl;
        return 1;
    }

    return 0;
}