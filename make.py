import subprocess
import os
import sys
import glob

# Archivos fuente
source_files = [
    "main.cpp", "parser.cpp", "scanner.cpp", "token.cpp",
    "visitor.cpp", "exp.cpp"
]

# Carpetas de entrada disponibles
input_categories = {
    "vars": "inputs/vars",           # 3 ejemplos de declaración de variables
    "exp": "inputs/exp",             # 3 ejemplos de expresiones  
    "selectivas": "inputs/selectivas", # 6 ejemplos de control selectivo (2 if, 2 while, 2 for)
    "funciones": "inputs/funciones", # 3 ejemplos de funciones
    "floats": "inputs/floats",       # 5 ejemplos de primera extensión (floats)
    "arrays": "inputs/arrays",       # 5 ejemplos de segunda extensión (arrays) - pendiente
    "tests": "inputs/tests"          # Ejemplos adicionales de prueba
}

# Opciones de ejecución
execution_options = {
    1: "Scanner",
    2: "Parser", 
    3: "Print Visitor",
    4: "Eval Visitor",
    5: "Assembly Generation"
}

def compile_project():
    """Compila el proyecto"""
    print("Compilando...")
    compile_cmd = ["g++", "-g"] + source_files + ["-o", "main.exe"]
    try:
        result = subprocess.run(compile_cmd, timeout=30)
        
        if result.returncode != 0:
            print("Error de compilación.")
            return False
        
        print("Compilación exitosa.\n")
        return True
    except subprocess.TimeoutExpired:
        print("Timeout en compilación.")
        return False
    except Exception as e:
        print(f"Error en compilación: {e}")
        return False

def is_file_empty_or_placeholder(filepath):
    """Verifica si el archivo está vacío o contiene solo placeholders"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read().strip()
            # Considera vacío si no tiene contenido o solo tiene palabras clave de placeholder
            if not content or content.lower() in ['empty', 'vacio', 'todo', 'placeholder']:
                return True
            # También considera vacío si tiene menos de 20 caracteres (muy corto)
            if len(content) < 20:
                return True
        return False
    except:
        return True

def clean_assembly_files(category=None):
    """Limpia archivos .s de assembly"""
    if category:
        if category not in input_categories:
            print(f"Categoría '{category}' no encontrada.")
            return
        
        pattern = f"{input_categories[category]}/*.s"
        files = glob.glob(pattern)
        
        if files:
            print(f"Eliminando {len(files)} archivos .s de la categoría '{category}':")
            for file in files:
                try:
                    os.remove(file)
                    print(f"  Eliminado: {file}")
                except Exception as e:
                    print(f"  Error eliminando {file}: {e}")
        else:
            print(f"No se encontraron archivos .s en la categoría '{category}'")
    else:
        # Limpiar todos los archivos .s
        total_files = 0
        for cat_name, cat_path in input_categories.items():
            if os.path.exists(cat_path):
                pattern = f"{cat_path}/*.s"
                files = glob.glob(pattern)
                if files:
                    print(f"Eliminando {len(files)} archivos .s de '{cat_name}':")
                    for file in files:
                        try:
                            os.remove(file)
                            print(f"  Eliminado: {file}")
                            total_files += 1
                        except Exception as e:
                            print(f"  Error eliminando {file}: {e}")
        
        if total_files > 0:
            print(f"\nTotal: {total_files} archivos .s eliminados")
        else:
            print("No se encontraron archivos .s para eliminar")

def execute_file_with_filter(filepath, steps_to_run=None):
    """Ejecuta un archivo y filtra la salida según los pasos seleccionados"""
    print(f"\n{'='*60}")
    print(f"Ejecutando: {filepath}")
    if steps_to_run:
        step_names = [execution_options[step] for step in steps_to_run if step in execution_options]
        print(f"Mostrando solo: {', '.join(step_names)}")
    print('='*60)
    
    if is_file_empty_or_placeholder(filepath):
        print(f"OMITIDO: {filepath} está vacío o es un placeholder")
        return
    
    try:
        # Ejecutar el programa y capturar toda la salida
        result = subprocess.run(["./main.exe", filepath], 
                              capture_output=True, 
                              text=True,
                              timeout=10)
        
        if result.returncode != 0:
            print(f"Error ejecutando {filepath}")
            if result.stderr:
                print(f"Error: {result.stderr}")
            return
        
        # Filtrar la salida según los pasos seleccionados
        output_lines = result.stdout.split('\n')
        
        if steps_to_run is None:
            # Mostrar todo
            print(result.stdout)
        else:
            # Filtrar según los pasos
            show_current = False
            current_section = None
            
            for line in output_lines:
                # Detectar inicio de secciones
                if "Iniciando Scanner:" in line and 1 in steps_to_run:
                    show_current = True
                    current_section = 1
                    print(line)
                elif "Scanner exitoso" in line and current_section == 1:
                    print(line)
                    show_current = False
                elif "Iniciando parsing:" in line and 2 in steps_to_run:
                    show_current = True
                    current_section = 2
                    print(line)
                elif "Parsing exitoso" in line and current_section == 2:
                    print(line)
                    show_current = False
                elif "IMPRIMIR:" in line and 3 in steps_to_run:
                    show_current = True
                    current_section = 3
                    print(line)
                elif "EJECUTAR:" in line:
                    if current_section == 3:
                        show_current = False
                    if 4 in steps_to_run:
                        show_current = True
                        current_section = 4
                        print(line)
                elif "GENERANDO" in line:
                    if current_section == 4:
                        show_current = False
                    if 5 in steps_to_run:
                        show_current = True
                        current_section = 5
                        print(line)
                elif "Codigo assembly generado" in line and current_section == 5:
                    print(line)
                    show_current = False
                elif show_current:
                    print(line)
                    
    except subprocess.TimeoutExpired:
        print(f"Timeout ejecutando {filepath}")
    except Exception as e:
        print(f"Error ejecutando {filepath}: {e}")

def execute_file(filepath, steps_to_run=None):
    """Ejecuta un archivo y maneja errores - versión simple sin filtro"""
    print(f"\n{'='*60}")
    print(f"Ejecutando: {filepath}")
    if steps_to_run:
        step_names = [execution_options[step] for step in steps_to_run if step in execution_options]
        print(f"Pasos seleccionados: {', '.join(step_names)}")
    print('='*60)
    
    if is_file_empty_or_placeholder(filepath):
        print(f"OMITIDO: {filepath} está vacío o es un placeholder")
        return
    
    # Si se especificaron pasos, usar la versión con filtro
    if steps_to_run:
        execute_file_with_filter(filepath, steps_to_run)
    else:
        # Versión sin filtro (mostrar todo)
        try:
            result = subprocess.run(["./main.exe", filepath], 
                                  capture_output=False, 
                                  text=True,
                                  timeout=10)
            if result.returncode != 0:
                print(f"Error ejecutando {filepath}")
        except subprocess.TimeoutExpired:
            print(f"Timeout ejecutando {filepath}")
        except Exception as e:
            print(f"Error ejecutando {filepath}: {e}")

def execute_category(category, steps_to_run=None):
    """Ejecuta todos los archivos de una categoría"""
    if category not in input_categories:
        print(f"Categoría '{category}' no encontrada.")
        print(f"Categorías disponibles: {list(input_categories.keys())}")
        return
    
    category_path = input_categories[category]
    
    if not os.path.exists(category_path):
        print(f"La carpeta {category_path} no existe.")
        return
    
    print(f"\nEjecutando archivos de la categoría: {category}")
    
    # Buscar archivos en la categoría
    files = []
    for filename in os.listdir(category_path):
        if filename.endswith('.txt'):
            files.append(filename)
    
    # Ordenar archivos numéricamente
    files.sort(key=lambda x: int(x.split('_')[-1].split('.')[0]) if x.split('_')[-1].split('.')[0].isdigit() else 999)
    
    executed_count = 0
    for filename in files:
        filepath = os.path.join(category_path, filename)
        if not is_file_empty_or_placeholder(filepath):
            execute_file(filepath, steps_to_run)
            executed_count += 1
        else:
            print(f"OMITIDO: {filename} (vacío o placeholder)")
    
    print(f"\nEjecutados {executed_count} archivos de {len(files)} en la categoría '{category}'")

def execute_all(steps_to_run=None):
    """Ejecuta todos los archivos de todas las categorías"""
    total_executed = 0
    total_files = 0
    
    for category in input_categories.keys():
        category_path = input_categories[category]
        
        if not os.path.exists(category_path):
            print(f"OMITIDO: La carpeta {category_path} no existe.")
            continue
        
        print(f"\n{'#'*80}")
        print(f"CATEGORÍA: {category.upper()}")
        print('#'*80)
        
        files = [f for f in os.listdir(category_path) if f.endswith('.txt')]
        files.sort(key=lambda x: int(x.split('_')[-1].split('.')[0]) if x.split('_')[-1].split('.')[0].isdigit() else 999)
        
        executed_in_category = 0
        for filename in files:
            filepath = os.path.join(category_path, filename)
            total_files += 1
            if not is_file_empty_or_placeholder(filepath):
                execute_file(filepath, steps_to_run)
                executed_in_category += 1
                total_executed += 1
            else:
                print(f"OMITIDO: {filename} (vacío o placeholder)")
        
        print(f"\nEjecutados {executed_in_category} archivos de {len(files)} en '{category}'")
    
    print(f"\n{'='*80}")
    print(f"RESUMEN FINAL: {total_executed} archivos ejecutados de {total_files} totales")
    print('='*80)

def parse_steps(steps_str):
    """Parsea la cadena de pasos a ejecutar"""
    if not steps_str:
        return None
    
    try:
        steps = [int(x.strip()) for x in steps_str.split(',')]
        valid_steps = [s for s in steps if s in execution_options]
        return valid_steps if valid_steps else None
    except:
        return None

def show_help():
    """Muestra la ayuda del programa"""
    print("USO: python make.py [opciones] [categoría|all]")
    print("\nOpciones:")
    print("  --no-compile          - No compilar antes de ejecutar")
    print("  --steps=1,2,3         - Mostrar solo los pasos especificados")
    print("  --clean               - Limpiar todos los archivos .s")
    print("  --clean=<categoría>   - Limpiar archivos .s de una categoría")
    print("  help                  - Muestra esta ayuda")
    
    print("\nPasos disponibles:")
    for num, name in execution_options.items():
        print(f"  {num} - {name}")
    
    print("\nCategorías disponibles:")
    for category, path in input_categories.items():
        exists = "✓" if os.path.exists(path) else "✗"
        print(f"  {category:<15} - {path} {exists}")
    
    print("\nEjemplos:")
    print("  python make.py all                    # Ejecutar todo")
    print("  python make.py vars                   # Solo variables")
    print("  python make.py --no-compile vars      # Variables sin compilar")
    print("  python make.py --steps=1,3 vars       # Solo scanner y print")
    print("  python make.py --steps=4 all          # Solo evaluación")
    print("  python make.py --clean                # Limpiar todos los .s")
    print("  python make.py --clean=floats         # Limpiar .s de floats")

def main():
    args = sys.argv[1:]
    
    if not args:
        show_help()
        return
    
    # Parsear argumentos
    should_compile = True
    steps_to_run = None
    clean_mode = None
    target = None
    
    i = 0
    while i < len(args):
        arg = args[i]
        
        if arg == "help":
            show_help()
            return
        elif arg == "--no-compile":
            should_compile = False
        elif arg.startswith("--steps="):
            steps_str = arg.split("=", 1)[1]
            steps_to_run = parse_steps(steps_str)
            if steps_to_run is None:
                print("Error: Pasos inválidos. Use números del 1-5 separados por comas.")
                return
        elif arg == "--clean":
            clean_mode = "all"
        elif arg.startswith("--clean="):
            clean_mode = arg.split("=", 1)[1]
        elif arg in input_categories or arg == "all":
            target = arg
        else:
            print(f"Opción no reconocida: {arg}")
            show_help()
            return
        
        i += 1
    
    # Ejecutar modo de limpieza
    if clean_mode is not None:
        if clean_mode == "all":
            clean_assembly_files()
        else:
            clean_assembly_files(clean_mode)
        return
    
    # Verificar que se especificó un target
    if target is None:
        print("Error: Debe especificar una categoría o 'all'")
        show_help()
        return
    
    # Compilar si es necesario
    if should_compile and not compile_project():
        return
    
    # Mostrar información sobre los pasos seleccionados
    if steps_to_run:
        step_names = [execution_options[s] for s in steps_to_run]
        print(f"Mostrando solo: {', '.join(step_names)}\n")
    
    # Ejecutar según el target
    if target == "all":
        execute_all(steps_to_run)
    else:
        execute_category(target, steps_to_run)

if __name__ == "__main__":
    main()