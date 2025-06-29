import subprocess
import os
import sys
import glob
import platform
import shutil

# Detectar sistema operativo
IS_WINDOWS = platform.system() == 'Windows'
EXECUTABLE_NAME = "main.exe" if IS_WINDOWS else "main"
COMPILER_CMD = "g++"

# Archivos fuente
source_files = [
    "main.cpp", "parser.cpp", "scanner.cpp", "token.cpp",
    "visitor.cpp", "exp.cpp", "codegen.cpp", "typechecker.cpp"
]

# Carpetas de entrada disponibles
input_categories = {
    "vars": "inputs/vars",           # 3 ejemplos de declaración de variables
    "exp": "inputs/exp",             # 3 ejemplos de expresiones  
    "selectivas": "inputs/selectivas", # 6 ejemplos de control selectivo (2 if, 2 while, 2 for)
    "funciones": "inputs/funciones", # 3 ejemplos de funciones
    "floats": "inputs/floats",       # 5 ejemplos de primera extensión (floats)
    "arrays": "inputs/arrays",       # 5 ejemplos de segunda extensión (arrays)
    "tests": "inputs/tests",         # Ejemplos adicionales de prueba
    "web": "inputs/web"              # Inputs desde la interfaz web
}

# Opciones de ejecución
execution_options = {
    1: "Scanner",
    2: "Parser", 
    3: "Print Visitor",
    4: "Eval Visitor",
    5: "Assembly Generation"
}

# Pasos que requieren salida en consola
step_output_map = {
    1: ["scanner"],
    2: ["parser"],
    3: ["print_visitor"],
    4: ["eval_visitor"],
    5: ["assembly"]
}

# Mapear pasos a strings que aparecen en la salida
step_to_marker = {
    1: ["TOKEN("],
    2: ["Parsing exitoso"],
    3: ["IMPRIMIR:"],
    4: ["EJECUTAR:"],
    5: ["GENERANDO CODIGO ASSEMBLY:"]
}

def compile_project():
    """Compila el proyecto con mejor manejo de errores"""
    print("🔨 Compilando proyecto...")
    compile_cmd = [COMPILER_CMD, "-g"] + source_files + ["-o", EXECUTABLE_NAME]
    try:
        print(f"Comando: {' '.join(compile_cmd)}")
        result = subprocess.run(compile_cmd, capture_output=True, text=True, timeout=60)
        if result.returncode != 0:
            print("❌ Error de compilación.")
            if result.stderr:
                print("Errores:")
                print(result.stderr)
            if result.stdout:
                print("Salida adicional:")
                print(result.stdout)
            return False
        if not os.path.exists(EXECUTABLE_NAME):
            print(f"❌ Error: {EXECUTABLE_NAME} no se creó correctamente")
            return False
        if not IS_WINDOWS:
            os.chmod(EXECUTABLE_NAME, 0o755)
            print(f"✅ Permisos de ejecución asignados a {EXECUTABLE_NAME}")
        print("✅ Compilación exitosa.\n")
        return True
    except subprocess.TimeoutExpired:
        print("❌ Timeout en compilación (más de 60 segundos)")
        return False
    except FileNotFoundError:
        print(f"❌ Error: {COMPILER_CMD} no encontrado")
        print("💡 Asegúrate de tener g++ instalado y en el PATH")
        return False
    except Exception as e:
        print(f"❌ Error en compilación: {e}")
        return False

def is_file_empty_or_placeholder(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read().strip()
            if not content or content.lower() in ['empty', 'vacio', 'todo', 'placeholder']:
                return True
            if len(content) < 20:
                return True
        return False
    except:
        return True

def clean_assembly_files(category=None):
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

def filter_output_by_steps(output, steps_to_run):
    if not steps_to_run:
        return output
    lines = output.splitlines()
    keep = set()
    for step in steps_to_run:
        for marker in step_to_marker.get(step, []):
            for i, line in enumerate(lines):
                if marker in line:
                    keep.add(i)
    # Always keep lines after the last selected marker
    if keep:
        min_idx = min(keep)
        max_idx = max(keep)
        filtered = lines[min_idx:]
        # Remove lines for steps not selected
        filtered_lines = []
        for line in filtered:
            for step in steps_to_run:
                for marker in step_to_marker.get(step, []):
                    if marker in line:
                        filtered_lines.append(line)
                        break
                else:
                    continue
                break
            else:
                # If line doesn't match any marker, keep it if it's after the first marker
                filtered_lines.append(line)
        return '\n'.join(filtered_lines)
    return output

def execute_file(filepath, steps_to_run=None, wsl_path=None):
    print(f"\n{'='*60}")
    print(f"Ejecutando: {filepath}")
    if steps_to_run:
        step_names = [execution_options[step] for step in steps_to_run if step in execution_options]
        print(f"Pasos seleccionados: {', '.join(step_names)}")
    print('='*60)
    if is_file_empty_or_placeholder(filepath):
        print(f"OMITIDO: {filepath} está vacío o es un placeholder")
        return
    try:
        exec_cmd = [f"./{EXECUTABLE_NAME}" if not IS_WINDOWS else EXECUTABLE_NAME, filepath]
        result = subprocess.run(exec_cmd, capture_output=True, text=True)
        output = result.stdout if hasattr(result, 'stdout') else ''
        if steps_to_run:
            output = filter_output_by_steps(output, steps_to_run)
        print(output)
        if result.returncode != 0:
            print(f"Error ejecutando {filepath} (código: {result.returncode})")
        # Copiar .s generado si hay wsl_path y paso 5 (assembly)
        if wsl_path and steps_to_run and 5 in steps_to_run:
            s_file = os.path.splitext(filepath)[0] + '.s'
            if os.path.exists(s_file):
                # Crear subcarpeta en WSL si no existe
                rel_dir = os.path.dirname(s_file)
                wsl_subdir = os.path.join(wsl_path, rel_dir)
                os.makedirs(wsl_subdir, exist_ok=True)
                dst = os.path.join(wsl_subdir, os.path.basename(s_file))
                try:
                    shutil.copy2(s_file, dst)
                    print(f"Copiado: {s_file} -> {dst}")
                except Exception as e:
                    print(f"Error copiando {s_file}: {e}")
    except FileNotFoundError:
        print(f"❌ Error: {EXECUTABLE_NAME} no encontrado. Ejecuta compilación primero.")
        if not IS_WINDOWS:
            print("💡 En Linux: Compila con 'g++ -g *.cpp -o main'")
    except KeyboardInterrupt:
        print(f"\n❌ Interrumpido por el usuario: {filepath}")
    except Exception as e:
        print(f"❌ Error ejecutando {filepath}: {e}")

def execute_category(category, steps_to_run=None, wsl_path=None):
    if category not in input_categories:
        print(f"Categoría '{category}' no encontrada.")
        print(f"Categorías disponibles: {list(input_categories.keys())}")
        return
    category_path = input_categories[category]
    if not os.path.exists(category_path):
        print(f"La carpeta {category_path} no existe.")
        return
    print(f"\nEjecutando archivos de la categoría: {category}")
    files = []
    for filename in os.listdir(category_path):
        if filename.endswith('.txt'):
            files.append(filename)
    files.sort(key=lambda x: int(x.split('_')[-1].split('.')[0]) if x.split('_')[-1].split('.')[0].isdigit() else 999)
    executed_count = 0
    for filename in files:
        filepath = os.path.join(category_path, filename)
        if not is_file_empty_or_placeholder(filepath):
            execute_file(filepath, steps_to_run, wsl_path)
            executed_count += 1
        else:
            print(f"OMITIDO: {filename} (vacío o placeholder)")
    print(f"\nEjecutados {executed_count} archivos de {len(files)} en la categoría '{category}'")

def execute_all(steps_to_run=None, wsl_path=None):
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
                execute_file(filepath, steps_to_run, wsl_path)
                executed_in_category += 1
                total_executed += 1
            else:
                print(f"OMITIDO: {filename} (vacío o placeholder)")
        print(f"\nEjecutados {executed_in_category} archivos de {len(files)} en '{category}'")
    print(f"\n{'='*80}")
    print(f"RESUMEN FINAL: {total_executed} archivos ejecutados de {total_files} totales")
    print('='*80)

def build_wsl_script(wsl_path, subfolder=None):
    # If subfolder is given, generate the script in that subfolder
    script_dir = os.path.join(wsl_path, subfolder) if subfolder else wsl_path
    os.makedirs(script_dir, exist_ok=True)
    script_path = os.path.join(script_dir, "build_all.sh")
    with open(script_path, "w", encoding="utf-8") as f:
        f.write("#!/bin/bash\n")
        f.write("# Compila y ejecuta todos los archivos .s en el directorio actual\n\n")
        f.write("for sfile in *.s; do\n")
        f.write("    if [ -f \"$sfile\" ]; then\n")
        f.write("        exe=\"${sfile%.s}.o\"\n")
        f.write("        echo \"Compilando $sfile -> $exe\"\n")
        f.write("        gcc \"$sfile\" -o \"$exe\"\n")
        f.write("        chmod +x \"$exe\"\n")
        f.write("        echo \"----- Ejecutando $exe -----\"\n")
        f.write("        ./\"$exe\"\n")
        f.write("        echo \"---------------------------\"\n")
        f.write("    fi\n")
        f.write("done\n\n")
        f.write("echo \"Compilación y ejecución de todos los .s finalizada.\"\n")
    os.chmod(script_path, 0o755)
    print(f"Script generado: {script_path}")

def parse_steps(steps_str):
    if not steps_str:
        return None
    try:
        steps = [int(x.strip()) for x in steps_str.split(',')]
        valid_steps = [s for s in steps if s in execution_options]
        return valid_steps if valid_steps else None
    except:
        return None

def show_help():
    os_info = "Windows" if IS_WINDOWS else "Linux/Unix"
    print(f"USO: python make.py [opciones] [categoría|all] (Detectado: {os_info})")
    print("\nOpciones:")
    print("  --no-compile          - No compilar antes de ejecutar")
    print("  --steps=1,2,3         - Mostrar solo los pasos especificados")
    print("  --clean               - Limpiar todos los archivos .s")
    print("  --clean=<categoría>   - Limpiar archivos .s de una categoría")
    print("  --copy-wsl=<ruta>     - Copiar todos los .s a WSL y generar script build_all.sh")
    print("  help                  - Muestra esta ayuda")
    print("\nPasos disponibles:")
    for num, name in execution_options.items():
        print(f"  {num} - {name}")
    print("\nCategorías disponibles:")
    for category, path in input_categories.items():
        exists = "[OK]" if os.path.exists(path) else "[--]"
        print(f"  {category:<15} - {path} {exists}")
    print("\nEjemplos:")
    print("  python make.py all                    # Ejecutar todo")
    print("  python make.py vars                   # Solo variables")
    print("  python make.py --no-compile vars      # Variables sin compilar")
    print("  python make.py --steps=1,3 vars       # Solo scanner y print")
    print("  python make.py --steps=5 all          # Solo assembly")
    print("  python make.py --clean                # Limpiar todos los .s")
    print("  python make.py --clean=vars           # Limpiar .s de vars")
    print("  python make.py --copy-wsl=\\wsl.localhost\\Ubuntu\\home\\joaqu   # Copiar .s a WSL y generar script")
    if IS_WINDOWS:
        print("\n🐧 Para usar los archivos .s generados en Ubuntu:")
        print("  1. python make.py --steps=5 all      # Generar archivos .s")
        print("  2. Copiar archivos .s a Ubuntu")
        print("  3. En Ubuntu: gcc archivo.s -o ejecutable")
        print("  4. En Ubuntu: ./ejecutable")
    else:
        print("\n🐧 Compilación y ejecución en Linux:")
        print("  1. python make.py --steps=5 all      # Generar archivos .s")
        print("  2. gcc archivo.s -o ejecutable       # Compilar assembly")
        print("  3. ./ejecutable                      # Ejecutar")

def main():
    args = sys.argv[1:]
    if not args:
        show_help()
        return
    should_compile = True
    steps_to_run = None
    clean_mode = None
    wsl_path = r"\\wsl.localhost\\Ubuntu\\home\\joaqu"  # Cambia aquí si tu ruta WSL es diferente
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
    if clean_mode is not None:
        if clean_mode == "all":
            clean_assembly_files()
        else:
            clean_assembly_files(clean_mode)
        return
    if target is None:
        print("Error: Debe especificar una categoría o 'all'")
        show_help()
        return
    if should_compile and not compile_project():
        return
    if steps_to_run:
        step_names = [execution_options[s] for s in steps_to_run]
        print(f"Mostrando solo: {', '.join(step_names)}\n")
    if target == "all":
        execute_all(steps_to_run, wsl_path)
    else:
        execute_category(target, steps_to_run, wsl_path)
    # Generar script de build en WSL si hubo paso 5
    if steps_to_run and ((5 in steps_to_run) or (4 in steps_to_run)):
        if target and target in input_categories and target != "all":
            build_wsl_script(wsl_path, input_categories[target])
        else:
            build_wsl_script(wsl_path)

if __name__ == "__main__":
    main()
