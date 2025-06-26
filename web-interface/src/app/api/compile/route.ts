import { NextRequest, NextResponse } from 'next/server';
import { writeFile, unlink, mkdir, readFile, readdir } from 'fs/promises';
import { exec } from 'child_process';
import { promisify } from 'util';
import path from 'path';

const execAsync = promisify(exec);

export async function POST(request: NextRequest) {
  try {
    const { code, mode = 'eval' } = await request.json();
    
    // Crear directorio web si no existe
    const webDir = path.join(process.cwd(), '..', 'inputs', 'web');
    await mkdir(webDir, { recursive: true });
    
    // Limpiar archivos temporales antiguos (más de 1 hora)
    try {
      const files = await readdir(webDir);
      const now = Date.now();
      for (const file of files) {
        if (file.startsWith('temp_') && (file.endsWith('.txt') || file.endsWith('.s'))) {
          const timestamp = parseInt(file.replace('temp_', '').replace('.txt', '').replace('.s', ''));
          if (now - timestamp > 3600000) { // 1 hora en ms
            await unlink(path.join(webDir, file));
          }
        }
      }
    } catch (cleanupError) {
      console.log('Error limpiando archivos antiguos:', cleanupError);
    }
    
    // Crear archivo temporal
    const filename = `temp_${Date.now()}.txt`;
    const filepath = path.join(webDir, filename);
    await writeFile(filepath, code, 'utf8');
    
    // Mapear mode a steps de make.py según tu configuración
    const modeToSteps: { [key: string]: string } = {
      'eval': '4',        // Solo Eval Visitor (ejecutar)
      'assembly': '4,5'   // Eval Visitor + Assembly Generation
    };
    
    const steps = modeToSteps[mode] || '4'; // Default a Eval
    
    // Detectar comando Python según el sistema operativo
    const isWindows = process.platform === 'win32';
    const pythonCmd = isWindows ? 'python' : 'python3';
    const executableName = isWindows ? 'main.exe' : 'main';
    
    let compilerCmd: string;
    
    if (mode === 'eval') {
      // Para eval, ejecutar directamente el compilador (más rápido)
      compilerCmd = isWindows ? `${executableName} ${filepath}` : `./${executableName} ${filepath}`;
    } else {
      // Para assembly, usar make.py con steps 4,5 (eval + assembly)
      compilerCmd = `${pythonCmd} make.py --no-compile --steps=${steps} web`;
    }
    
    console.log(`Ejecutando: ${compilerCmd}`);
    
    const { stdout, stderr } = await execAsync(compilerCmd, {
      cwd: path.join(process.cwd(), '..'),
      timeout: 30000,
      maxBuffer: 1024 * 1024
    });
    
    // Limpiar archivo temporal
    try {
      await unlink(filepath);
    } catch (cleanupError) {
      console.log('No se pudo eliminar archivo temporal:', cleanupError);
    }
    
    // Procesar salida según el modo
    let output = '';
    let assembly = '';
    
    if (mode === 'eval') {
      // Con ejecución directa, la salida es mucho más limpia
      const lines = stdout.split('\n');
      const executionOutput: string[] = [];
      let captureOutput = false;
      
      for (const line of lines) {
        const trimmed = line.trim();
        
        // Comenzar a capturar después de "EJECUTAR:"
        if (trimmed === 'EJECUTAR:') {
          captureOutput = true;
          continue;
        }
        
        // Si estamos capturando y encontramos contenido útil
        if (captureOutput && trimmed) {
          // Parar si encontramos "GENERANDO" o similar
          if (trimmed.includes('GENERANDO') || trimmed.includes('Generando')) {
            break;
          }
          
          // Capturar números (enteros y decimales) y resultados útiles
          if (/^-?\d+(\.\d+)?$/.test(trimmed) || 
              (!trimmed.includes('TOKEN(') && 
               !trimmed.includes('Scanner') && 
               !trimmed.includes('parsing') &&
               !trimmed.includes('Visitor') &&
               !trimmed.includes('IMPRIMIR') &&
               !trimmed.includes('fun main') &&
               !trimmed.includes('var ') &&
               !trimmed.includes('return') &&
               !trimmed.includes('}') &&
               !trimmed.includes('println'))) {
            executionOutput.push(trimmed);
          }
        }
      }
      
      if (executionOutput.length > 0) {
        output = executionOutput.join('\n') + '\n\n✅ Compilación exitosa';
      } else {
        // Buscar números directamente en todo el output (incluir decimales)
        const numberMatches = stdout.match(/^-?\d+(\.\d+)?$/gm);
        if (numberMatches && numberMatches.length > 0) {
          output = numberMatches.join('\n') + '\n\n✅ Compilación exitosa';
        } else {
          output = '✅ Programa ejecutado correctamente (sin salida numérica)';
        }
      }
        
    } else if (mode === 'assembly') {
      // Para assembly: usar EXACTAMENTE la misma lógica que eval
      const lines = stdout.split('\n');
      const executionOutput: string[] = [];
      let captureEval = false;
      let foundOurFile = false;
      
      for (const line of lines) {
        const trimmed = line.trim();
        
        // Detectar cuando se procesa nuestro archivo específico
        if (trimmed.includes(`Ejecutando: inputs/web\\${filename}`) || 
            trimmed.includes(`Ejecutando: inputs/web/${filename}`)) {
          foundOurFile = true;
          captureEval = false;
          continue;
        }
        
        if (foundOurFile) {
          // Capturar resultado del eval (EXACTAMENTE igual que en modo eval)
          if (trimmed === 'EJECUTAR:') {
            captureEval = true;
            continue;
          }
          
          if (captureEval && trimmed) {
            // Parar de capturar eval si encontramos "GENERANDO"
            if (trimmed.includes('GENERANDO') || trimmed.includes('Generando')) {
              break;
            }
            
            // USAR EXACTAMENTE EL MISMO FILTRO QUE EN EVAL
            if (/^-?\d+(\.\d+)?$/.test(trimmed) || 
                (!trimmed.includes('TOKEN(') && 
                 !trimmed.includes('Scanner') && 
                 !trimmed.includes('parsing') &&
                 !trimmed.includes('Visitor') &&
                 !trimmed.includes('IMPRIMIR') &&
                 !trimmed.includes('fun main') &&
                 !trimmed.includes('var ') &&
                 !trimmed.includes('return') &&
                 !trimmed.includes('}') &&
                 !trimmed.includes('println'))) {
              executionOutput.push(trimmed);
            }
          }
        }
      }
      
      // Formatear output EXACTAMENTE igual que en modo eval
      if (executionOutput.length > 0) {
        output = executionOutput.join('\n') + '\n\n✅ Compilación exitosa';
      } else {
        // Buscar números directamente en todo el output (incluir decimales)
        const numberMatches = stdout.match(/^-?\d+(\.\d+)?$/gm);
        if (numberMatches && numberMatches.length > 0) {
          output = numberMatches.join('\n') + '\n\n✅ Compilación exitosa';
        } else {
          output = '✅ Programa ejecutado correctamente (sin salida numérica)';
        }
      }
      
      // Leer el archivo assembly generado para el panel separado
      try {
        const assemblyFilename = filename.replace('.txt', '.s');
        const assemblyPath = path.join(webDir, assemblyFilename);
        assembly = await readFile(assemblyPath, 'utf8');
        
        // En Linux/Unix, NO limpiar el archivo assembly para permitir ejecución posterior
        if (isWindows) {
          await unlink(assemblyPath);
        }
      } catch (assemblyError) {
        console.log('No se pudo leer archivo assembly:', assemblyError);
        const assemblyMatch = stdout.match(/\.text[\s\S]*$/);
        if (assemblyMatch) {
          assembly = assemblyMatch[0];
        }
      }
    }
    
    return NextResponse.json({
      success: true,
      output: output || 'Compilación completada',
      assembly: assembly,
      error: stderr || null,
      mode: mode,
      steps: steps,
      canExecuteAssembly: !isWindows && mode === 'assembly' && assembly, // Solo en Linux/Unix si hay assembly
      assemblyFilename: mode === 'assembly' ? filename.replace('.txt', '.s') : null
    });
    
  } catch (error: unknown) {
    console.error('Error en compilación:', error);
    
    const errorMessage = error instanceof Error ? error.message : 'Error desconocido';
    
    return NextResponse.json({
      success: false,
      error: errorMessage,
      output: null
    }, { status: 500 });
  }
}

// ✨ API DINÁMICA: Lee automáticamente los archivos reales de inputs/
export async function GET() {
  try {
    const examples: { [key: string]: string } = {};
    
    // Categorías definidas en tu make.py (basado en input_categories)
    const categories: { [key: string]: string } = {
      "vars": "inputs/vars",           // 3 ejemplos de declaración de variables
      "exp": "inputs/exp",             // 3 ejemplos de expresiones  
      "selectivas": "inputs/selectivas", // 6 ejemplos de control selectivo
      "funciones": "inputs/funciones", // 3 ejemplos de funciones
      "floats": "inputs/floats",       // 5 ejemplos de primera extensión (floats)
      "arrays": "inputs/arrays"        // 5 ejemplos de segunda extensión (arrays)
    };
    
    const basePath = path.join(process.cwd(), '..');
    
    // Para cada categoría, leer archivos .txt disponibles
    for (const [categoryName, categoryPath] of Object.entries(categories)) {
      try {
        const fullCategoryPath = path.join(basePath, categoryPath);
        const files = await readdir(fullCategoryPath);
        
        // Buscar archivos .txt, preferir los numerados (1_, 2_, etc.)
        const txtFiles = files
          .filter(f => f.endsWith('.txt'))
          .sort((a, b) => {
            const aNum = parseInt(a.split('_')[0]) || 999;
            const bNum = parseInt(b.split('_')[0]) || 999;
            return aNum - bNum;
          });
        
        if (txtFiles.length > 0) {
          // Tomar solo el PRIMER archivo válido (no vacío) de cada categoría
          for (const file of txtFiles) {
            try {
              const filePath = path.join(fullCategoryPath, file);
              const content = await readFile(filePath, 'utf8');
              
              // Verificar que no esté vacío o sea placeholder
              if (content.trim() && 
                  content.length > 20 && 
                  !content.toLowerCase().includes('empty') &&
                  !content.toLowerCase().includes('placeholder') &&
                  !content.toLowerCase().includes('todo')) {
                
                examples[categoryName] = content.trim();
                break; // Solo tomar el primer ejemplo válido
              }
            } catch (fileError) {
              console.log(`Error leyendo ${file}:`, fileError);
            }
          }
        }
        
        // Si no se encontró contenido válido, usar ejemplo limpio funcional
        if (!examples[categoryName]) {
          const fallbackExamples: { [key: string]: string } = {
            vars: `fun main(): Int {
    var edad: Int = 25
    var altura: Float = 1.75f
    var activo: Int = 1
    println(edad)
    println(altura)
    println(activo)
    return 0
}`,
            exp: `fun main(): Int {
    var numero1: Int = 15
    var numero2: Int = 8
    var suma: Int = numero1 + numero2
    var producto: Int = numero1 * numero2
    var resultado: Int = (suma + producto) / 2
    println(suma)
    println(producto) 
    println(resultado)
    return 0
}`, 
            selectivas: `fun main(): Int {
    var puntuacion: Int = 85
    var resultado: Int = 0
    if (puntuacion >= 90) {
        resultado = 10
    } else if (puntuacion >= 70) {
        resultado = 8
    } else {
        resultado = 5
    }
    println(resultado)
    return 0
}`,
            funciones: `fun calcular(valor1: Int, valor2: Int): Int {
    var temp: Int = valor1 + valor2 * 2
    return temp
}

fun main(): Int {
    var numero: Int = 10
    var resultado: Int = calcular(numero, 5)
    println(resultado)
    return 0
}`,
            floats: `fun main(): Int {
    var temperatura: Float = 23.5f
    var factor: Float = 1.8f
    var fahrenheit: Float = temperatura * factor + 32.0f
    var entero: Int = 100
    println(temperatura)
    println(fahrenheit)
    println(entero)
    return 0
}`,
            arrays: `fun main(): Int {
    var numeros: Array<Int> = [10, 20, 30, 40, 50]
    var suma: Int = 0
    var i: Int = 0
    while (i < 5) {
        suma = suma + numeros[i]
        i = i + 1
    }
    var promedio: Int = suma / 5
    println(suma)
    println(promedio)
    return 0
}`
          };
          
          examples[categoryName] = fallbackExamples[categoryName] || `fun main(): Int {
    var ejemplo: Int = 42
    println(ejemplo)
    return 0
}`;
        }
        
      } catch (categoryError) {
        console.log(`Error procesando categoría ${categoryName}:`, categoryError);
        // Fallback si la carpeta no existe
        examples[categoryName] = `// Categoría: ${categoryName}\n// (Archivos no disponibles actualmente)`;
      }
    }
    
    return NextResponse.json(examples);
    
  } catch (error) {
    console.error('Error cargando ejemplos dinámicos:', error);
    return NextResponse.json({ error: 'Error cargando ejemplos' }, { status: 500 });
  }
}

// ✨ EJECUTAR ASSEMBLY en Linux/Unix
export async function PUT(request: NextRequest) {
  try {
    const { assemblyCode } = await request.json();
    
    // Solo permitir en sistemas Unix/Linux
    const isWindows = process.platform === 'win32';
    if (isWindows) {
      return NextResponse.json({
        success: false,
        error: 'La ejecución de assembly solo está disponible en Linux/Unix'
      }, { status: 400 });
    }
    
    // Crear directorio web si no existe
    const webDir = path.join(process.cwd(), '..', 'inputs', 'web');
    await mkdir(webDir, { recursive: true });
    
    // Crear archivo assembly temporal
    const assemblyFilename = `exec_${Date.now()}.s`;
    const assemblyPath = path.join(webDir, assemblyFilename);
    await writeFile(assemblyPath, assemblyCode, 'utf8');
    
    // Compilar con gcc
    const executableName = `exec_${Date.now()}`;
    const executablePath = path.join(webDir, executableName);
    const compileCmd = `gcc "${assemblyPath}" -o "${executablePath}"`;
    
    console.log(`Compilando assembly: ${compileCmd}`);
    
    try {
      const { stdout: compileStdout, stderr: compileStderr } = await execAsync(compileCmd, {
        cwd: path.join(process.cwd(), '..'),
        timeout: 15000
      });
      
      if (compileStderr && compileStderr.includes('error')) {
        return NextResponse.json({
          success: false,
          error: `Error de compilación: ${compileStderr}`,
          output: null
        }, { status: 400 });
      }
      
      // Ejecutar el binario compilado
      const executeCmd = `"${executablePath}"`;
      console.log(`Ejecutando assembly: ${executeCmd}`);
      
      const { stdout: execStdout, stderr: execStderr } = await execAsync(executeCmd, {
        cwd: path.join(process.cwd(), '..'),
        timeout: 10000
      });
      
      // Limpiar archivos temporales
      try {
        await unlink(assemblyPath);
        await unlink(executablePath);
      } catch (cleanupError) {
        console.log('Error limpiando archivos:', cleanupError);
      }
      
      return NextResponse.json({
        success: true,
        output: `${execStdout || '(sin salida)'}\n\n🚀 Assembly ejecutado correctamente`,
        error: execStderr || null,
        compileOutput: compileStdout
      });
      
    } catch (error: unknown) {
      // Limpiar archivos en caso de error
      try {
        await unlink(assemblyPath);
        await unlink(executablePath);
      } catch {
        // Ignorar errores de limpieza
      }
      
      const errorMessage = error instanceof Error ? error.message : 'Error desconocido';
      
      return NextResponse.json({
        success: false,
        error: `Error ejecutando assembly: ${errorMessage}`,
        output: null
      }, { status: 500 });
    }
    
  } catch (error: unknown) {
    console.error('Error en ejecución de assembly:', error);
    
    const errorMessage = error instanceof Error ? error.message : 'Error desconocido';
    
    return NextResponse.json({
      success: false,
      error: errorMessage,
      output: null
    }, { status: 500 });
  }
}
