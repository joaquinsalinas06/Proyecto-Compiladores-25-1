import { NextRequest, NextResponse } from 'next/server'
import { exec } from 'child_process'
import { promisify } from 'util'
import { writeFile, readFile, mkdir } from 'fs/promises'
import path from 'path'

const execAsync = promisify(exec)

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

    // Determinar qué pasos ejecutar según el modo
    const mainExePath = path.join(compilerDir, 'main.exe')
    const inputRelativePath = `inputs/web/${filename}`
    
    switch (mode) {
      case 'eval':
        // Solo ejecutar hasta eval visitor (pasos 1-4)
        command = `"${mainExePath}" "${inputRelativePath}"`
        break
      case 'assembly':
        // Ejecutar todos los pasos incluyendo generación de assembly
        command = `"${mainExePath}" "${inputRelativePath}"`
        break
      default:
        command = `"${mainExePath}" "${inputRelativePath}"`
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

    // Limpiar el archivo temporal (opcional)
    // await unlink(inputPath)

    return NextResponse.json({
      success: true,
      output: stdout,
      errors: stderr,
      assembly: assemblyCode,
      filename: filename.replace('.txt', '.s')
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
      return NextResponse.json(
        { success: false, error: 'No se encontró el archivo main.exe. Asegúrate de que el compilador esté compilado.' },
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
