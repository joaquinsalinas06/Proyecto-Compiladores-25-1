import subprocess
import os
import sys
import glob
import shutil

# Archivos fuente
source_files = [
    "main.cpp", "parser.cpp", "scanner.cpp", "token.cpp",
    "visitor.cpp", "exp.cpp", "codegen.cpp"
]

# Configuración de WSL
WSL_PATH = r"\\wsl.localhost\Ubuntu-20.04\home\salinsuwu"
WSL_ASSEMBLY_DIR = os.path.join(WSL_PATH, "assembly_files")

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

def execute_category(category, steps_to_run=None, auto_wsl=True):
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
    
    # Copiar archivos assembly a WSL si se generaron y compilar/ejecutar automáticamente
    if (steps_to_run is None or 5 in steps_to_run) and auto_wsl:
        print(f"\n🔄 Copiando archivos assembly de '{category}' a WSL...")
        if copy_assembly_to_wsl(category_path, category):
            print(f"\n🔨 Compilando y ejecutando '{category}' en WSL automáticamente...")
            if compile_category_in_wsl(category):
                print(f"\n🚀 Ejecutando archivos compilados de '{category}' en WSL...")
                run_all_in_category_wsl(category)

def execute_all(steps_to_run=None, auto_wsl=True):
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
    
    # Copiar archivos assembly a WSL si se generaron y compilar/ejecutar automáticamente
    if (steps_to_run is None or 5 in steps_to_run) and auto_wsl:
        print(f"\n🔄 Copiando archivos assembly a WSL...")
        if copy_assembly_to_wsl(None):  # Copiar todos
            print(f"\n🔨 Compilando todas las categorías en WSL automáticamente...")
            # Compilar cada categoría que tenga archivos .s
            for category in input_categories.keys():
                category_path = input_categories[category]
                if os.path.exists(category_path):
                    s_files = glob.glob(f"{category_path}/*.s")
                    if s_files:
                        print(f"\n📁 Compilando y ejecutando '{category}' en WSL...")
                        if compile_category_in_wsl(category):
                            run_all_in_category_wsl(category)

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
    print("  --no-wsl              - No compilar automáticamente en WSL después del paso 5")
    print("  --steps=1,2,3         - Mostrar solo los pasos especificados")
    print("  --clean               - Limpiar todos los archivos .s")
    print("  --clean=<categoría>   - Limpiar archivos .s de una categoría")
    print("  --copy-wsl            - Copiar archivos .s existentes a WSL")
    print("  --copy-wsl=<categoría> - Copiar archivos .s de una categoría a WSL")
    print("  --setup-wsl           - Crear Makefile en WSL para compilación")
    print("  --test-wsl            - Probar configuración de WSL")
    print("  --compile-wsl=<cat>   - Compilar categoría en WSL")
    print("  --run-wsl=<cat>       - Ejecutar archivos compilados en WSL")
    print("  --wsl=<cat>           - Compilar y ejecutar categoría en WSL")
    print("  --wsl-file=<file>     - Compilar y ejecutar archivo específico en WSL")
    print("  help                  - Muestra esta ayuda")
    
    print("\nPasos disponibles:")
    for num, name in execution_options.items():
        print(f"  {num} - {name}")
    
    print("\nCategorías disponibles:")
    for category, path in input_categories.items():
        exists = "✓" if os.path.exists(path) else "✗"
        print(f"  {category:<15} - {path} {exists}")
    
    print(f"\nIntegración con WSL:")
    wsl_available = "✓" if os.path.exists(WSL_PATH) else "✗"
    print(f"  WSL Ubuntu disponible: {wsl_available}")
    print(f"  Directorio WSL: {WSL_ASSEMBLY_DIR}")
    
    print("\nEjemplos:")
    print("  python make.py all                    # Ejecutar todo")
    print("  python make.py vars                   # Solo variables (con auto-WSL)")
    print("  python make.py --no-wsl vars          # Variables sin auto-WSL")
    print("  python make.py --no-compile vars      # Variables sin compilar")
    print("  python make.py --steps=1,3 vars       # Solo scanner y print")
    print("  python make.py --steps=4 all          # Solo evaluación")
    print("  python make.py --steps=5 --no-wsl all # Solo assembly sin WSL")
    print("  python make.py --clean                # Limpiar todos los .s")
    print("  python make.py --clean=floats         # Limpiar .s de floats")
    print("  python make.py --copy-wsl             # Copiar archivos .s a WSL")
    print("  python make.py --copy-wsl=floats      # Copiar solo floats a WSL")
    print("  python make.py --setup-wsl            # Crear Makefile en WSL")
    print("  python make.py --test-wsl             # Probar WSL")
    print("  python make.py --wsl=floats           # Compilar y ejecutar floats en WSL")
    print("  python make.py --compile-wsl=floats   # Solo compilar floats en WSL")
    print("  python make.py --run-wsl=floats       # Solo ejecutar floats en WSL")
    print("  python make.py --wsl-file=input_floats_1.s  # Archivo específico")

def setup_wsl_directory():
    """Crea el directorio de assembly en WSL si no existe"""
    try:
        if not os.path.exists(WSL_PATH):
            print(f"Advertencia: WSL path {WSL_PATH} no está disponible")
            return False
        
        if not os.path.exists(WSL_ASSEMBLY_DIR):
            os.makedirs(WSL_ASSEMBLY_DIR, exist_ok=True)
            print(f"Creado directorio: {WSL_ASSEMBLY_DIR}")
        
        return True
    except Exception as e:
        print(f"Error configurando directorio WSL: {e}")
        return False

def copy_assembly_to_wsl(source_path, category=None):
    """Copia archivos .s al directorio WSL"""
    if not setup_wsl_directory():
        return False
    
    try:
        copied_files = 0
        
        if category:
            # Copiar archivos de una categoría específica
            category_path = input_categories.get(category)
            if category_path and os.path.exists(category_path):
                pattern = f"{category_path}/*.s"
                files = glob.glob(pattern)
                
                # Crear subdirectorio por categoría
                wsl_category_dir = os.path.join(WSL_ASSEMBLY_DIR, category)
                os.makedirs(wsl_category_dir, exist_ok=True)
                
                for file in files:
                    filename = os.path.basename(file)
                    dest_path = os.path.join(wsl_category_dir, filename)
                    shutil.copy2(file, dest_path)
                    print(f"Copiado: {filename} → WSL/{category}/")
                    copied_files += 1
        else:
            # Copiar archivos de todas las categorías
            for cat_name, cat_path in input_categories.items():
                if os.path.exists(cat_path):
                    pattern = f"{cat_path}/*.s"
                    files = glob.glob(pattern)
                    
                    if files:
                        # Crear subdirectorio por categoría
                        wsl_category_dir = os.path.join(WSL_ASSEMBLY_DIR, cat_name)
                        os.makedirs(wsl_category_dir, exist_ok=True)
                        
                        for file in files:
                            filename = os.path.basename(file)
                            dest_path = os.path.join(wsl_category_dir, filename)
                            shutil.copy2(file, dest_path)
                            print(f"Copiado: {filename} → WSL/{cat_name}/")
                            copied_files += 1
        
        if copied_files > 0:
            print(f"\n✓ {copied_files} archivos .s copiados a WSL")
            print(f"📁 Ubicación: {WSL_ASSEMBLY_DIR}")
            print("\n📋 Para compilar en WSL:")
            print("   cd ~/assembly_files")
            print("   gcc -no-pie archivo.s -o archivo")
            print("   ./archivo")
        else:
            print("No se encontraron archivos .s para copiar")
        
        return True
        
    except Exception as e:
        print(f"Error copiando a WSL: {e}")
        return False

def create_wsl_makefile():
    """Crea un Makefile en WSL para facilitar compilación"""
    if not setup_wsl_directory():
        return
    
    makefile_content = """# Makefile para compilar archivos assembly en WSL
# Uso: make compile FILE=archivo.s
# Ejemplo: make compile FILE=floats/input_floats_1.s

CC = gcc
CFLAGS = -no-pie

# Compilar un archivo específico
compile:
	@if [ -z "$(FILE)" ]; then \\
		echo "Error: Especifica FILE=archivo.s"; \\
		echo "Ejemplo: make compile FILE=floats/input_floats_1.s"; \\
	else \\
		echo "Compilando $(FILE)..."; \\
		$(CC) $(CFLAGS) $(FILE) -o $$(basename $(FILE) .s); \\
		echo "Ejecutable creado: $$(basename $(FILE) .s)"; \\
		echo "Para ejecutar: ./$$(basename $(FILE) .s)"; \\
	fi

# Compilar todos los archivos .s en un directorio
compile-dir:
	@if [ -z "$(DIR)" ]; then \\
		echo "Error: Especifica DIR=directorio"; \\
		echo "Ejemplo: make compile-dir DIR=floats"; \\
	else \\
		for file in $(DIR)/*.s; do \\
			if [ -f "$$file" ]; then \\
				echo "Compilando $$file..."; \\
				$(CC) $(CFLAGS) "$$file" -o "$$(basename "$$file" .s)"; \\
			fi; \\
		done; \\
	fi

# Limpiar ejecutables
clean:
	find . -type f -executable ! -name "Makefile" ! -name "*.s" -delete
	echo "Ejecutables eliminados"

# Mostrar ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make compile FILE=archivo.s    - Compilar un archivo"
	@echo "  make compile-dir DIR=categoria - Compilar toda una categoría"
	@echo "  make clean                     - Limpiar ejecutables"
	@echo "  make help                      - Mostrar esta ayuda"

.PHONY: compile compile-dir clean help
"""
    
    try:
        makefile_path = os.path.join(WSL_ASSEMBLY_DIR, "Makefile")
        with open(makefile_path, 'w', encoding='utf-8') as f:
            f.write(makefile_content)
        
        print(f"✓ Makefile creado en: {makefile_path}")
        return True
        
    except Exception as e:
        print(f"Error creando Makefile: {e}")
        return False

def compile_assembly_in_wsl(category=None):
    """Compila archivos assembly en WSL usando el comando wsl"""
    if not setup_wsl_directory():
        return False
    
    try:
        compiled_files = 0
        results = []
        
        if category:
            # Compilar archivos de una categoría específica
            wsl_category_dir = f"~/assembly_files/{category}"
            
            # Listar archivos .s en la categoría
            list_cmd = f'wsl bash -c "find {wsl_category_dir} -name \'*.s\' 2>/dev/null"'
            result = subprocess.run(list_cmd, shell=True, capture_output=True, text=True)
            
            if result.returncode != 0:
                print(f"No se encontraron archivos .s en {category}")
                return False
            
            files = result.stdout.strip().split('\n')
            files = [f for f in files if f.strip()]
            
            for file_path in files:
                if not file_path.strip():
                    continue
                    
                filename = file_path.split('/')[-1].replace('.s', '')
                
                # Compilar archivo
                compile_cmd = f'wsl bash -c "cd ~/assembly_files && gcc -no-pie {file_path} -o {filename} 2>/dev/null"'
                compile_result = subprocess.run(compile_cmd, shell=True, capture_output=True, text=True)
                
                if compile_result.returncode == 0:
                    print(f"✓ Compilado: {filename}")
                    compiled_files += 1
                    
                    # Ejecutar y capturar salida
                    exec_cmd = f'wsl bash -c "cd ~/assembly_files && ./{filename}"'
                    exec_result = subprocess.run(exec_cmd, shell=True, capture_output=True, text=True)
                    
                    if exec_result.returncode == 0:
                        output = exec_result.stdout.strip()
                        results.append((filename, output))
                        print(f"  Salida: {output}")
                    else:
                        print(f"  ❌ Error ejecutando {filename}")
                else:
                    print(f"❌ Error compilando {filename}")
                    if compile_result.stderr:
                        print(f"  Error: {compile_result.stderr.strip()}")
        
        else:
            # Compilar todas las categorías
            for cat_name in input_categories.keys():
                print(f"\n📁 Compilando categoría: {cat_name}")
                cat_results = compile_assembly_in_wsl(cat_name)
                if cat_results:
                    results.extend(cat_results)
        
        if compiled_files > 0:
            print(f"\n🎉 {compiled_files} archivos compilados en WSL")
            return results
        else:
            print("No se compilaron archivos")
            return False
            
    except Exception as e:
        print(f"Error compilando en WSL: {e}")
        return False

def compile_file_in_wsl(filename, category=None):
    """Compila un archivo específico en WSL"""
    try:
        if category:
            file_path = f"~/assembly_files/{category}/{filename}"
            exec_name = filename.replace('.s', '')
        else:
            file_path = f"~/assembly_files/{filename}"
            exec_name = filename.replace('.s', '')
        
        print(f"🔨 Compilando {filename} en WSL...")
        
        # Compilar archivo
        compile_cmd = f'wsl bash -c "cd ~/assembly_files && make compile FILE={category}/{filename}"' if category else f'wsl bash -c "cd ~/assembly_files && gcc -no-pie {filename} -o {exec_name}"'
        result = subprocess.run(compile_cmd, shell=True, capture_output=True, text=True)
        
        if result.returncode == 0:
            print(f"✓ Compilación exitosa: {exec_name}")
            return exec_name
        else:
            print(f"❌ Error compilando {filename}")
            if result.stderr:
                print(f"Error: {result.stderr.strip()}")
            return None
            
    except Exception as e:
        print(f"Error: {e}")
        return None

def run_file_in_wsl(exec_name, category=None):
    """Ejecuta un archivo compilado en WSL"""
    try:
        if category:
            exec_cmd = f'wsl bash -c "cd ~/assembly_files && ./{exec_name}"'
        else:
            exec_cmd = f'wsl bash -c "cd ~/assembly_files && ./{exec_name}"'
        
        print(f"🚀 Ejecutando {exec_name}...")
        result = subprocess.run(exec_cmd, shell=True, capture_output=True, text=True)
        
        if result.returncode == 0:
            output = result.stdout.strip()
            print(f"📋 Salida: {output}")
            return output
        else:
            print(f"❌ Error ejecutando {exec_name}")
            if result.stderr:
                print(f"Error: {result.stderr.strip()}")
            return None
            
    except Exception as e:
        print(f"Error: {e}")
        return None

def compile_and_run_in_wsl(filename, category=None):
    """Compila y ejecuta un archivo en WSL"""
    exec_name = compile_file_in_wsl(filename, category)
    if exec_name:
        return run_file_in_wsl(exec_name, category)
    return None

def compile_category_in_wsl(category):
    """Compila todos los archivos .s de una categoría en WSL"""
    print(f"🔨 Compilando toda la categoría '{category}' en WSL...")
    
    try:
        # Usar el Makefile para compilar toda la categoría
        compile_cmd = f'wsl bash -c "cd ~/assembly_files && make compile-dir DIR={category}"'
        result = subprocess.run(compile_cmd, shell=True, capture_output=True, text=True)
        
        print(result.stdout)
        if result.stderr:
            print(result.stderr)
            
        return result.returncode == 0
        
    except Exception as e:
        print(f"Error: {e}")
        return False

def run_all_in_category_wsl(category):
    """Ejecuta todos los archivos compilados de una categoría en WSL"""
    print(f"🚀 Ejecutando archivos de la categoría '{category}' en WSL...")
    
    try:
        # Listar ejecutables en la categoría (archivos sin extensión)
        list_cmd = f'wsl bash -c "cd ~/assembly_files && find . -maxdepth 1 -type f -executable -name \'input_{category}_*\' 2>/dev/null"'
        result = subprocess.run(list_cmd, shell=True, capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"No se encontraron ejecutables para {category}")
            return False
        
        executables = result.stdout.strip().split('\n')
        executables = [exec.replace('./', '') for exec in executables if exec.strip()]
        
        if not executables:
            print(f"No hay ejecutables compilados para {category}")
            return False
        
        results = []
        for exec_name in sorted(executables):
            print(f"\n{'='*50}")
            print(f"Ejecutando: {exec_name}")
            print('='*50)
            
            output = run_file_in_wsl(exec_name)
            if output:
                results.append((exec_name, output))
        
        print(f"\n🎉 Ejecutados {len(results)} archivos de {category}")
        return results
        
    except Exception as e:
        print(f"Error: {e}")
        return False

def test_wsl_compilation():
    """Prueba la compilación en WSL y compara resultados"""
    print("🧪 Probando compilación en WSL...")
    
    # Verificar que WSL esté disponible
    try:
        result = subprocess.run("wsl echo test", shell=True, capture_output=True, text=True)
        if result.returncode != 0:
            print("❌ WSL no está disponible")
            return False
    except:
        print("❌ Error accediendo a WSL")
        return False
    
    # Verificar que gcc esté disponible en WSL
    try:
        result = subprocess.run("wsl gcc --version", shell=True, capture_output=True, text=True)
        if result.returncode != 0:
            print("❌ GCC no está disponible en WSL")
            print("💡 Ejecuta en WSL: sudo apt update && sudo apt install gcc")
            return False
        else:
            print("✓ GCC disponible en WSL")
    except:
        print("❌ Error verificando GCC en WSL")
        return False
    
    # Verificar que make esté disponible en WSL
    try:
        result = subprocess.run("wsl make --version", shell=True, capture_output=True, text=True)
        if result.returncode != 0:
            print("❌ Make no está disponible en WSL")
            print("💡 Ejecuta en WSL: sudo apt install make")
            return False
        else:
            print("✓ Make disponible en WSL")
    except:
        print("❌ Error verificando Make en WSL")
        return False
    
    print("✅ WSL configurado correctamente para compilación")
    return True

def main():
    args = sys.argv[1:]
    
    if not args:
        show_help()
        return
    
    # Parsear argumentos
    should_compile = True
    auto_wsl = True  # Nueva opción para auto-compilación en WSL
    steps_to_run = None
    clean_mode = None
    copy_wsl_mode = None
    setup_wsl_flag = False
    test_wsl_flag = False
    compile_wsl_mode = None
    run_wsl_mode = None
    wsl_mode = None
    wsl_file_mode = None
    target = None
    
    i = 0
    while i < len(args):
        arg = args[i]
        
        if arg == "help":
            show_help()
            return
        elif arg == "--no-compile":
            should_compile = False
        elif arg == "--no-wsl":
            auto_wsl = False
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
        elif arg == "--copy-wsl":
            copy_wsl_mode = "all"
        elif arg.startswith("--copy-wsl="):
            copy_wsl_mode = arg.split("=", 1)[1]
        elif arg == "--setup-wsl":
            setup_wsl_flag = True
        elif arg == "--test-wsl":
            test_wsl_flag = True
        elif arg.startswith("--compile-wsl="):
            compile_wsl_mode = arg.split("=", 1)[1]
        elif arg.startswith("--run-wsl="):
            run_wsl_mode = arg.split("=", 1)[1]
        elif arg.startswith("--wsl="):
            wsl_mode = arg.split("=", 1)[1]
        elif arg.startswith("--wsl-file="):
            wsl_file_mode = arg.split("=", 1)[1]
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
    
    # Ejecutar modo de copia a WSL
    if copy_wsl_mode is not None:
        if copy_wsl_mode == "all":
            copy_assembly_to_wsl(None)
        else:
            copy_assembly_to_wsl(None, copy_wsl_mode)
        return
    
    # Ejecutar setup de WSL
    if setup_wsl_flag:
        create_wsl_makefile()
        return
    
    # Ejecutar test de WSL
    if test_wsl_flag:
        test_wsl_compilation()
        return
    
    # Ejecutar compilación en WSL
    if compile_wsl_mode is not None:
        if compile_wsl_mode not in input_categories:
            print(f"Categoría '{compile_wsl_mode}' no encontrada.")
            return
        compile_category_in_wsl(compile_wsl_mode)
        return
    
    # Ejecutar archivos en WSL
    if run_wsl_mode is not None:
        if run_wsl_mode not in input_categories:
            print(f"Categoría '{run_wsl_mode}' no encontrada.")
            return
        run_all_in_category_wsl(run_wsl_mode)
        return
    
    # Compilar y ejecutar en WSL
    if wsl_mode is not None:
        if wsl_mode not in input_categories:
            print(f"Categoría '{wsl_mode}' no encontrada.")
            return
        print(f"🔄 Compilando y ejecutando '{wsl_mode}' en WSL...")
        if compile_category_in_wsl(wsl_mode):
            run_all_in_category_wsl(wsl_mode)
        return
    
    # Compilar y ejecutar archivo específico en WSL
    if wsl_file_mode is not None:
        filename = wsl_file_mode
        if not filename.endswith('.s'):
            filename += '.s'
        
        # Detectar categoría del archivo
        category = None
        for cat_name, cat_path in input_categories.items():
            if os.path.exists(os.path.join(cat_path, filename)):
                category = cat_name
                break
        
        if category:
            print(f"🔄 Compilando y ejecutando '{filename}' de '{category}' en WSL...")
            compile_and_run_in_wsl(filename, category)
        else:
            print(f"Archivo '{filename}' no encontrado en ninguna categoría")
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
        execute_all(steps_to_run, auto_wsl)
    else:
        execute_category(target, steps_to_run, auto_wsl)

if __name__ == "__main__":
    main()