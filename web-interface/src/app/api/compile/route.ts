import { NextRequest, NextResponse } from 'next/server'
import { exec } from 'child_process'
import { promisify } from 'util'
import { writeFile, readFile, mkdir } from 'fs/promises'
import path from 'path'

const execAsync = promisify(exec)
function formatOutput(stdout: string, mode: string): string {
  const lines = stdout.split('\n')
  
  if (mode === 'eval') {
    let inExecutionSection = false
    let executionOutput: string[] = []
    
    for (const line of lines) {
      if (line.includes('EJECUTAR:')) {
        inExecutionSection = true
        continue
      }
      
      if (inExecutionSection) {
        if (line.includes('GENERANDO CODIGO ASSEMBLY:') || line.startsWith('Generando codigo ensamblador')) {
          break
        }
        if (line.trim() && !line.includes('TOKEN(') && !line.includes('Iniciando') && !line.includes('exitoso')) {
          executionOutput.push(line.trim())
        }
      }
    }
    
    if (executionOutput.length > 0) {
      const result = [
        '🎯 RESULTADO DE LA EJECUCIÓN',
        '━'.repeat(40),
        ...executionOutput,
        '━'.repeat(40),
        '✅ Programa ejecutado exitosamente'
      ]
      return result.join('\n')
    } else {
      return [
        '🎯 RESULTADO DE LA EJECUCIÓN',
        '━'.repeat(40),
        '✅ Programa ejecutado correctamente (sin salida)',
        '━'.repeat(40)
      ].join('\n')
    }
  } else if (mode === 'assembly') {
    let inExecutionSection = false
    let executionOutput: string[] = []
    let assemblyGenerated = false
    
    for (const line of lines) {
      if (line.includes('EJECUTAR:')) {
        inExecutionSection = true
        continue
      }
      
      if (line.includes('GENERANDO CODIGO ASSEMBLY:')) {
        inExecutionSection = false
        assemblyGenerated = true
        continue
      }
      
      if (inExecutionSection && line.trim() && !line.includes('TOKEN(') && !line.includes('Iniciando') && !line.includes('exitoso')) {
        executionOutput.push(line.trim())
      }
      
      if (line.includes('Codigo assembly generado exitosamente')) {
        assemblyGenerated = true
      }
    }
    
    const result = []
    
    result.push('🎯 RESULTADO DE LA EJECUCIÓN')
    result.push('━'.repeat(40))
    if (executionOutput.length > 0) {
      result.push(...executionOutput)
    } else {
      result.push('✅ Programa ejecutado correctamente (sin salida)')
    }
    result.push('━'.repeat(40))
    result.push('')
    
    if (assemblyGenerated) {
      result.push('⚙️ GENERACIÓN DE CÓDIGO ASSEMBLY')
      result.push('━'.repeat(40))
      result.push('✅ Código assembly generado exitosamente')
      result.push('📄 Archivo disponible para descarga')
      result.push('━'.repeat(40))
    }
    
    return result.join('\n')
  }
  
  return stdout
}

export async function POST(request: NextRequest) {
  let command = ''
  let compilerDir = ''
  
  try {
    const { code, mode = 'eval' } = await request.json()

    if (!code || code.trim() === '') {
      return NextResponse.json(
        { success: false, error: 'No code provided' },
        { status: 400 }
      )
    }

    // Generar nombre único para el archivo
    const timestamp = Date.now()
    const filename = `web_input_${timestamp}.txt`
    
    // Ruta al directorio del compilador (relativa desde web-interface)
    compilerDir = path.join(process.cwd(), '..')
    const inputPath = path.join(compilerDir, 'inputs', 'web', filename)
    
    // Crear el archivo de entrada
    await mkdir(path.dirname(inputPath), { recursive: true })
    await writeFile(inputPath, code, 'utf-8')

    // Determinar el nombre del ejecutable según el sistema operativo
    const isWindows = process.platform === 'win32'
    const mainExeName = isWindows ? 'main.exe' : 'main'
    const mainExePath = path.join(compilerDir, mainExeName)
    const inputRelativePath = `inputs/web/${filename}`
    
    // Verificar que el ejecutable existe
    try {
      await readFile(mainExePath)
    } catch (error) {
      const compileCmd = isWindows 
        ? 'python make.py' 
        : 'g++ -g main.cpp scanner.cpp parser.cpp token.cpp exp.cpp visitor.cpp codegen.cpp -o main && chmod +x main'
      
      return NextResponse.json(
        { 
          success: false, 
          error: `No se encontró el ejecutable ${mainExeName}. Asegúrate de compilar el proyecto primero.`,
          details: `Buscar en: ${mainExePath}`,
          compileInstructions: `Comando de compilación: ${compileCmd}`
        },
        { status: 404 }
      )
    }
    
    // Construir el comando según el sistema operativo
    if (isWindows) {
      command = `"${mainExePath}" "${inputRelativePath}"`
    } else {
      // En Linux, usar ruta relativa y asegurar permisos
      command = `./main "${inputRelativePath}"`
    }

    // Ejecutar el compilador
    const { stdout, stderr } = await execAsync(command, { 
      timeout: 15000,
      encoding: 'utf-8',
      cwd: compilerDir
    })

    let assemblyCode = null
    if (mode === 'assembly') {
      // Intentar leer el archivo .s generado
      const assemblyPath = inputPath.replace('.txt', '.s')
      try {
        assemblyCode = await readFile(assemblyPath, 'utf-8')
      } catch (error) {
        // No se pudo leer el archivo assembly
        console.log('No se generó archivo assembly:', error)
      }
    }

    const formattedOutput = formatOutput(stdout, mode)

    return NextResponse.json({
      success: true,
      output: formattedOutput,
      errors: stderr,
      assembly: assemblyCode,
      filename: filename.replace('.txt', '.s'),
      rawOutput: stdout
    })

  } catch (error: any) {
    console.error('Error en API compile:', error)
    console.error('Command:', command)
    console.error('Compiler dir:', compilerDir)
    
    // Manejar diferentes tipos de errores
    if (error.code === 'ETIMEDOUT') {
      return NextResponse.json(
        { success: false, error: 'Timeout: El código tardó demasiado en ejecutarse' },
        { status: 408 }
      )
    }

    if (error.code === 'ENOENT') {
      const expectedExe = process.platform === 'win32' ? 'main.exe' : 'main'
      const compileCmd = process.platform === 'win32' 
        ? 'python make.py' 
        : 'g++ -g main.cpp scanner.cpp parser.cpp token.cpp exp.cpp visitor.cpp codegen.cpp -o main && chmod +x main'
      
      return NextResponse.json(
        { 
          success: false, 
          error: `No se encontró el ejecutable ${expectedExe}. Asegúrate de compilar el proyecto primero.`,
          details: `Comando de compilación: ${compileCmd}`
        },
        { status: 404 }
      )
    }

    return NextResponse.json(
      { 
        success: false, 
        error: `Command failed: ${error.message}`,
        details: error.stderr || '',
        command: command
      },
      { status: 500 }
    )
  }
}
