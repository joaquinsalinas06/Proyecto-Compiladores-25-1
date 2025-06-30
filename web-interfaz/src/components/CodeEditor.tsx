"use client"

import { useState, useEffect } from "react"
import dynamic from "next/dynamic"
import { Button } from "@/components/ui/button"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Tabs, TabsList, TabsTrigger, TabsContent } from "@/components/ui/tabs"
import { Badge } from "@/components/ui/badge"
import { Play, Download, Code2, Terminal, Cpu, Loader2, Zap, Copy, Check, Rocket } from "lucide-react"

// Estilos CSS personalizados para animaciones fluidas
const customStyles = `
  @keyframes fadeIn {
    from { opacity: 0; transform: translateY(10px); }
    to { opacity: 1; transform: translateY(0); }
  }
  
  @keyframes gentleFloat {
    0%, 100% { transform: translateY(0px) translateX(0px); }
    25% { transform: translateY(-5px) translateX(2px); }
    50% { transform: translateY(-8px) translateX(-1px); }
    75% { transform: translateY(-3px) translateX(1px); }
  }
  
  @keyframes slowRotate {
    from { transform: rotate(0deg); }
    to { transform: rotate(360deg); }
  }
  
  @keyframes gradientShift {
    0% { background-position: 0% 50%; }
    50% { background-position: 100% 50%; }
    100% { background-position: 0% 50%; }
  }
  
  @keyframes particleFloat {
    0%, 100% { transform: translateY(0px) translateX(0px) scale(1); opacity: 0.3; }
    33% { transform: translateY(-20px) translateX(10px) scale(1.1); opacity: 0.6; }
    66% { transform: translateY(-10px) translateX(-5px) scale(0.9); opacity: 0.4; }
  }
  
  @keyframes waveMovement {
    0%, 100% { transform: translateX(0px) translateY(0px); }
    25% { transform: translateX(20px) translateY(-10px); }
    50% { transform: translateX(-15px) translateY(-20px); }
    75% { transform: translateX(10px) translateY(-5px); }
  }
  
  @keyframes typewriter {
    from { width: 0; }
    to { width: 100%; }
  }
  
  @keyframes blinkCursor {
    0%, 50% { border-right-color: transparent; }
    51%, 100% { border-right-color: currentColor; }
  }
  
  @keyframes pulse-cursor {
    0%, 100% { opacity: 1; }
    50% { opacity: 0.3; }
  }
  
  .animate-fadeIn {
    animation: fadeIn 0.5s ease-out;
  }
  
  .typewriter-effect {
    overflow: hidden;
    white-space: nowrap;
    border-right: 2px solid;
    animation: typewriter 2s steps(40) forwards, blinkCursor 1s infinite;
  }
  
  .typewriter-line {
    overflow: hidden;
    border-right: 1px solid currentColor;
    animation: blinkCursor 1s infinite;
  }
  
  .typing-cursor {
    animation: pulse-cursor 1s ease-in-out infinite;
  }
  
  .animate-gentle-float {
    animation: gentleFloat 6s ease-in-out infinite;
  }
  
  .animate-slow-rotate {
    animation: slowRotate 30s linear infinite;
  }
  
  .animate-gradient-shift {
    background-size: 200% 200%;
    animation: gradientShift 8s ease-in-out infinite;
  }
  
  .animate-particle-float {
    animation: particleFloat 12s ease-in-out infinite;
  }
  
  .animate-wave-movement {
    animation: waveMovement 15s ease-in-out infinite;
  }
  
  .glass-effect {
    backdrop-filter: blur(20px);
    background: rgba(255, 255, 255, 0.05);
  }
  
  .mesh-gradient {
    background: linear-gradient(-45deg, 
      rgba(59, 130, 246, 0.15),
      rgba(147, 51, 234, 0.15),
      rgba(236, 72, 153, 0.15),
      rgba(14, 165, 233, 0.15),
      rgba(168, 85, 247, 0.15)
    );
    background-size: 400% 400%;
  }
`

// Importar Monaco Editor dinámicamente
const MonacoEditor = dynamic(() => import("@monaco-editor/react"), {
  ssr: false,
  loading: () => (
    <div className="h-full bg-gradient-to-br from-slate-900 to-slate-800 rounded-lg flex items-center justify-center">
      <div className="flex items-center gap-3 text-slate-300">
        <Loader2 className="w-6 h-6 animate-spin" />
        <span className="text-lg font-medium">Cargando editor...</span>
      </div>
    </div>
  ),
})

interface CompileResult {
  success: boolean
  output?: string
  errors?: string
  assembly?: string
  error?: string
  canExecuteAssembly?: boolean
  assemblyFilename?: string
}

export default function KotlinCompiler() {
  const [code, setCode] = useState(`// Variable Declaration and Assignment
// Global variables with initialization
var x : Int = 10
var y : Int = 20

fun main(){
    println(x)  // Print global variable x
    println(y)  // Print global variable y
    println(x + y)  // Print sum of variables
}`)

  const [output, setOutput] = useState("")
  const [assembly, setAssembly] = useState("")
  const [loading, setLoading] = useState(false)
  const [executingAssembly, setExecutingAssembly] = useState(false)
  const [canExecuteAssembly, setCanExecuteAssembly] = useState(false)
  const [assemblyFilename, setAssemblyFilename] = useState("")
  const [mode, setMode] = useState<"eval" | "assembly">("eval")
  const [copied, setCopied] = useState(false)
  const [executionTime, setExecutionTime] = useState<number | null>(null)
  const [dynamicExamples, setDynamicExamples] = useState<{[key: string]: string}>({})
  const [loadingExamples, setLoadingExamples] = useState(true)
  
  // Estados para el efecto de escritura
  const [displayedOutput, setDisplayedOutput] = useState("")
  const [isTyping, setIsTyping] = useState(false)
  const [typingComplete, setTypingComplete] = useState(false)

  // Cargar ejemplos dinámicos al montar el componente
  useEffect(() => {
    const loadDynamicExamples = async () => {
      try {
        const response = await fetch("/api/compile")
        if (response.ok) {
          const examples = await response.json()
          setDynamicExamples(examples)
          
          // Cargar automáticamente el ejemplo de variables si está disponible
          if (examples.vars) {
            setCode(examples.vars)
          }
        }
      } catch (error) {
        console.error("Error cargando ejemplos dinámicos:", error)
      } finally {
        setLoadingExamples(false)
      }
    }
    
    loadDynamicExamples()
  }, [])

  // Efecto de escritura tipo máquina de escribir
  useEffect(() => {
    if (!output) {
      setDisplayedOutput("")
      setIsTyping(false)
      setTypingComplete(false)
      return
    }
    
    // Si ya está mostrando el mismo contenido, no hacer nada
    if (output === displayedOutput && typingComplete) return
    
    // Solo iniciar si no está ya escribiendo el mismo contenido
    if (isTyping) return
    
    setIsTyping(true)
    setTypingComplete(false)
    setDisplayedOutput("")
    
    let currentIndex = 0
    
    const typeText = () => {
      if (currentIndex <= output.length) {
        setDisplayedOutput(output.slice(0, currentIndex))
        currentIndex++
        
        // Velocidad consistente y natural
        const char = output[currentIndex - 1]
        let delay = 25 // Velocidad base 
        
        // Pequeñas variaciones según el carácter
        if (char === ' ') {
          delay = 40
        } else if (char === '\n') {
          delay = 80
        } else if (char === '.' || char === '!' || char === '?') {
          delay = 120
        } else if (/[0-9]/.test(char)) {
          delay = 20 // Números un poco más rápidos
        }
        
        setTimeout(typeText, delay)
      } else {
        setIsTyping(false)
        setTypingComplete(true)
      }
    }
    
    // Iniciar después de un pequeño delay
    const startDelay = setTimeout(typeText, 300)
    
    return () => {
      clearTimeout(startDelay)
    }
  }, [output]) // Solo depende de output para evitar bucle infinito

  // Categorías de ejemplos con descripciones 
  const exampleCategories = {
    vars: { name: "Variables", description: "Variable declaration and assignment", icon: "📝" },
    exp: { name: "Expressions", description: "Mathematical expressions and operators", icon: "🧮" },
    selectivas: { name: "Conditionals", description: "If-else statements and logic", icon: "🔀" },
    funciones: { name: "Functions", description: "Function definitions and calls", icon: "⚙️" },
    floats: { name: "Floats", description: "Decimal numbers and operations", icon: "🔢" },
    arrays: { name: "Arrays", description: "Array creation and manipulation", icon: "📋" }
  }

  const handleCompile = async () => {
    setLoading(true)
    setOutput("")
    setAssembly("")
    setExecutionTime(null)
    
    // Limpiar completamente el estado de escritura
    setDisplayedOutput("")
    setIsTyping(false)
    setTypingComplete(false)

    const startTime = Date.now()

    try {
      const response = await fetch("/api/compile", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ code, mode }),
      })

      const result: CompileResult = await response.json()
      const endTime = Date.now()
      setExecutionTime(endTime - startTime)

      if (result.success) {
        setOutput(result.output || "")
        console.log(result)
        if (result.assembly) {
          setAssembly(result.assembly)
        }
        setCanExecuteAssembly(result.canExecuteAssembly || false)
        setAssemblyFilename(result.assemblyFilename || "")
      } else {
        setOutput(`❌ Error: ${result.error}\n${result.errors || ""}`)
        setCanExecuteAssembly(false)
        setAssemblyFilename("")
      }
    } catch (error) {
      setOutput(`❌ Error de conexión: ${error}`)
      setExecutionTime(Date.now() - startTime)
    }
    setLoading(false)
  }

  const handleModeChange = (value: string) => {
    const newMode = value as "eval" | "assembly"
    setMode(newMode)
    // Limpiar estados cuando se cambia de modo
    setCanExecuteAssembly(false)
    setAssemblyFilename("")
    if (newMode === "eval") {
      setAssembly("")
    }
  }

  const executeAssembly = async () => {
    if (!assembly || !assemblyFilename) return

    setExecutingAssembly(true)
    setDisplayedOutput("")
    setIsTyping(false)
    setTypingComplete(false)
    
    const startTime = Date.now()

    try {
      const response = await fetch("/api/compile", {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ 
          assemblyCode: assembly, 
          filename: assemblyFilename 
        }),
      })

      const result = await response.json()
      const endTime = Date.now()
      
      if (result.success) {
        setOutput(result.output || "")
        setExecutionTime(endTime - startTime)
      } else {
        setOutput(`❌ Error ejecutando assembly: ${result.error}`)
        setExecutionTime(endTime - startTime)
      }
    } catch (error) {
      setOutput(`❌ Error de conexión: ${error}`)
      setExecutionTime(Date.now() - startTime)
    }
    setExecutingAssembly(false)
  }

  const downloadAssembly = () => {
    if (!assembly) return

    const blob = new Blob([assembly], { type: "text/plain" })
    const url = URL.createObjectURL(blob)
    const a = document.createElement("a")
    a.href = url
    a.download = "program.s"
    a.click()
    URL.revokeObjectURL(url)
  }

  const loadExample = (exampleKey: string) => {
    // Cargar ejemplo dinámico desde la API
    if (dynamicExamples[exampleKey]) {
      setCode(dynamicExamples[exampleKey])
      
      // Limpiar el output al cambiar de ejemplo
      setOutput("")
      setAssembly("")
      setDisplayedOutput("")
      setIsTyping(false)
      setTypingComplete(false)
      setExecutionTime(null)
    }
  }

  const copyToClipboard = async (text: string) => {
    try {
      await navigator.clipboard.writeText(text)
      setCopied(true)
      setTimeout(() => setCopied(false), 2000)
    } catch (err) {
      console.error("Error al copiar:", err)
    }
  }

  return (
    <>
      <style jsx>{customStyles}</style>
      <div className="min-h-screen bg-gradient-to-br from-slate-900 via-purple-900 to-indigo-900 relative overflow-hidden">
      {/* Fondo animado fluido y elegante */}
      <div className="absolute inset-0 overflow-hidden pointer-events-none">
        {/* Gradiente de fondo principal con movimiento suave */}
        <div className="absolute inset-0 mesh-gradient animate-gradient-shift opacity-60"></div>
        
        {/* Ondas de fondo grandes con movimiento lento */}
        <div className="absolute -top-1/2 -right-1/2 w-full h-full bg-gradient-radial from-blue-500/10 via-transparent to-transparent rounded-full animate-slow-rotate"></div>
        <div className="absolute -bottom-1/2 -left-1/2 w-full h-full bg-gradient-radial from-purple-500/10 via-transparent to-transparent rounded-full animate-slow-rotate" style={{animationDelay: '10s', animationDirection: 'reverse'}}></div>
        <div className="absolute top-1/3 left-1/3 w-3/4 h-3/4 bg-gradient-radial from-cyan-500/5 via-transparent to-transparent rounded-full animate-slow-rotate" style={{animationDelay: '20s'}}></div>
        
        {/* Partículas flotantes sutiles */}
        <div className="absolute inset-0">
          {[...Array(35)].map((_, i) => (
            <div
              key={i}
              className="absolute w-1 h-1 bg-cyan-400/30 rounded-full animate-particle-float"
              style={{
                left: `${Math.random() * 100}%`,
                top: `${Math.random() * 100}%`,
                animationDelay: `${Math.random() * 12}s`,
              }}
            />
          ))}
        </div>
        
        {/* Efectos de ondas sutiles */}
        <div className="absolute inset-0">
          {[...Array(5)].map((_, i) => (
            <div
              key={i}
              className="absolute opacity-10"
              style={{
                left: `${20 + (i * 20)}%`,
                top: `${20 + (i * 15)}%`,
                width: `${50 + (i * 20)}px`,
                height: `${50 + (i * 20)}px`,
                background: `radial-gradient(circle, ${['cyan', 'purple', 'blue', 'indigo', 'pink'][i]}/20 0%, transparent 70%)`,
                borderRadius: '50%',
                animation: `waveMovement ${10 + (i * 2)}s ease-in-out infinite`,
                animationDelay: `${i * 2}s`
              }}
            />
          ))}
        </div>
        
        {/* Líneas de código flotantes muy sutiles */}
        <div className="absolute inset-0 opacity-5">
          {[...Array(8)].map((_, i) => (
            <div
              key={i}
              className="absolute text-cyan-300 font-mono text-xs animate-gentle-float"
              style={{
                left: `${10 + Math.random() * 80}%`,
                top: `${10 + Math.random() * 80}%`,
                animationDelay: `${Math.random() * 6}s`,
              }}
            >
              {['fun main()', 'var x: Int', 'return 0', 'println()', 'if (x > y)', '{ code }', '// comment', 'val result'][i]}
            </div>
          ))}
        </div>
        
        {/* Brillo sutil en los bordes */}
        <div className="absolute top-0 left-0 w-full h-px bg-gradient-to-r from-transparent via-cyan-400/20 to-transparent"></div>
        <div className="absolute bottom-0 left-0 w-full h-px bg-gradient-to-r from-transparent via-purple-400/20 to-transparent"></div>
        <div className="absolute left-0 top-0 w-px h-full bg-gradient-to-b from-transparent via-blue-400/20 to-transparent"></div>
        <div className="absolute right-0 top-0 w-px h-full bg-gradient-to-b from-transparent via-indigo-400/20 to-transparent"></div>
      </div>
      <header className="bg-gray-900/70 backdrop-blur-xl border-b border-purple-500/20 sticky top-0 z-50">
        <div className="container mx-auto px-6 py-4">
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-4">
              <div className="flex items-center gap-3">
                <div className="p-2 bg-gradient-to-br from-cyan-500 to-blue-600 rounded-xl shadow-lg shadow-cyan-500/25 transform hover:scale-110 transition-all duration-300">
                  <Code2 className="w-6 h-6 text-white" />
                </div>
                <div>
                  <h1 className="text-2xl font-bold bg-gradient-to-r from-cyan-400 to-purple-400 bg-clip-text text-transparent">
                    Kotlin Compiler
                  </h1>
                  <p className="text-sm text-gray-300">Compila y ejecuta código Kotlin en tiempo real</p>
                </div>
              </div>
            </div>

            <div className="flex items-center gap-4">
              {/* Selector de ejemplos dinámicos */}
              <Select onValueChange={loadExample} disabled={loadingExamples}>
                <SelectTrigger className="w-48 bg-gray-800/50 border border-purple-500/30 text-gray-100 backdrop-blur-sm hover:bg-gray-700/50 transition-all duration-200 hover:border-purple-400/50 focus:border-cyan-400/50 focus:ring-2 focus:ring-cyan-400/25">
                  <SelectValue placeholder={loadingExamples ? "Cargando..." : "Cargar ejemplo..."} />
                </SelectTrigger>
                <SelectContent className="bg-gray-800 border border-purple-500/30 backdrop-blur-xl">
                  {/* Ejemplos dinámicos por categoría */}
                  {Object.entries(exampleCategories).map(([key, category]) => {
                    const hasExample = dynamicExamples[key]
                    if (!hasExample) return null
                    
                    return (
                      <SelectItem key={key} value={key} className="text-gray-100 hover:bg-purple-500/20 focus:bg-purple-500/20 cursor-pointer">
                        <div className="flex items-center gap-3 py-1">
                          <span className="text-lg">{category.icon}</span>
                          <div className="flex flex-col">
                            <span className="font-medium">{category.name}</span>
                            <span className="text-xs text-gray-400">{category.description}</span>
                          </div>
                          <Badge variant="outline" className="ml-auto text-xs text-green-400 border-green-400/30">
                            Live
                          </Badge>
                        </div>
                      </SelectItem>
                    )
                  })}
                </SelectContent>
              </Select>

              {/* Selector de modo */}
              <Tabs value={mode} onValueChange={handleModeChange}>
                <TabsList className="grid w-full grid-cols-2 bg-gray-800/30 border border-purple-500/20 p-1 rounded-xl backdrop-blur-sm h-12">
                  <TabsTrigger
                    value="eval"
                    className="flex items-center justify-center gap-2 h-full px-4 rounded-lg transition-all duration-300 data-[state=active]:bg-gradient-to-r data-[state=active]:from-cyan-500 data-[state=active]:to-blue-500 data-[state=active]:text-white data-[state=active]:shadow-lg data-[state=active]:shadow-cyan-500/25 data-[state=active]:transform data-[state=active]:scale-105 text-gray-400 hover:text-gray-200 hover:bg-gray-700/30"
                  >
                    <Terminal className="w-4 h-4 flex-shrink-0" />
                    <span className="font-medium">Evaluar</span>
                  </TabsTrigger>
                  <TabsTrigger
                    value="assembly"
                    className="flex items-center justify-center gap-2 h-full px-4 rounded-lg transition-all duration-300 data-[state=active]:bg-gradient-to-r data-[state=active]:from-purple-500 data-[state=active]:to-pink-500 data-[state=active]:text-white data-[state=active]:shadow-lg data-[state=active]:shadow-purple-500/25 data-[state=active]:transform data-[state=active]:scale-105 text-gray-400 hover:text-gray-200 hover:bg-gray-700/30"
                  >
                    <Cpu className="w-4 h-4 flex-shrink-0" />
                    <span className="font-medium">Assembly</span>
                  </TabsTrigger>
                </TabsList>
              </Tabs>

              {/* Botón de compilar */}
              <Button
                onClick={handleCompile}
                disabled={loading}
                size="lg"
                className="bg-gradient-to-r from-pink-500 to-violet-600 hover:from-pink-600 hover:to-violet-700 text-white shadow-lg shadow-pink-500/25 hover:shadow-xl hover:shadow-pink-500/30 transition-all duration-300 transform hover:scale-105"
              >
                {loading ? (
                  <>
                    <Loader2 className="w-4 h-4 mr-2 animate-spin" />
                    Compilando...
                  </>
                ) : (
                  <>
                    <Play className="w-4 h-4 mr-2" />
                    Compilar
                  </>
                )}
              </Button>
            </div>
          </div>
        </div>
      </header>

      {/* Contenido principal */}
      <div className="container mx-auto px-6 py-8 relative z-10">
        <div className="grid lg:grid-cols-2 gap-8 h-[calc(100vh-200px)]">
          {/* Panel del editor */}
          <Card className="bg-gray-900/80 backdrop-blur-xl border border-cyan-500/20 shadow-2xl shadow-cyan-500/10 transform transition-all duration-500 hover:shadow-cyan-500/20 hover:border-cyan-400/30">
            <CardHeader className="pb-4">
              <div className="flex items-center justify-between">
                <CardTitle className="flex items-center gap-2 text-gray-100">
                  <div className="p-1.5 bg-gradient-to-br from-cyan-500 to-teal-600 rounded-lg shadow-lg shadow-cyan-500/25 transform hover:scale-110 transition-all duration-300">
                    <Code2 className="w-4 h-4 text-white" />
                  </div>
                  Editor de Código
                </CardTitle>
                <div className="flex items-center gap-2">
                  <Badge variant="secondary" className="bg-gradient-to-r from-purple-500/20 to-pink-500/20 text-purple-300 border border-purple-500/30 shadow-lg hover:shadow-purple-500/30 transition-all duration-300">
                    Kotlin
                  </Badge>
                  {mode === "assembly" && (
                    <Badge className="bg-gradient-to-r from-orange-500/20 to-red-500/20 text-orange-300 border border-orange-500/30 shadow-lg hover:shadow-orange-500/30 transition-all duration-300">
                      Assembly Mode
                    </Badge>
                  )}
                </div>
              </div>
            </CardHeader>
            <CardContent className="p-0 h-full">
              <div className="h-[calc(100%-80px)] rounded-lg overflow-hidden border border-slate-700/50 shadow-inner">
                <MonacoEditor
                  height="100%"
                  defaultLanguage="kotlin"
                  value={code}
                  onChange={(value) => setCode(value || "")}
                  theme="vs-dark"
                  options={{
                    minimap: { enabled: false },
                    fontSize: 14,
                    lineNumbers: "on",
                    roundedSelection: false,
                    scrollBeyondLastLine: false,
                    automaticLayout: true,
                    fontFamily: "JetBrains Mono, Fira Code, monospace",
                    fontLigatures: true,
                    cursorBlinking: "smooth",
                    smoothScrolling: true,
                    wordWrap: "on",
                    glyphMargin: true,
                    folding: true,
                    renderLineHighlight: "all",
                  }}
                />
              </div>
            </CardContent>
          </Card>

          <Card className="bg-gray-900/80 backdrop-blur-xl border border-cyan-500/20 shadow-2xl shadow-cyan-500/10 transform transition-all duration-500 hover:shadow-emerald-500/20 hover:border-emerald-400/30">
            <CardHeader className="pb-4">
              <div className="flex items-center justify-between">
                <CardTitle className="flex items-center gap-2 text-gray-100">
                  <div className="p-1.5 bg-gradient-to-br from-emerald-500 to-teal-600 rounded-lg shadow-lg shadow-emerald-500/25 transform hover:scale-110 transition-all duration-300">
                    <Terminal className="w-4 h-4 text-white" />
                  </div>
                  Resultados
                  {loading && (
                    <div className="flex items-center gap-2 ml-2">
                      <Loader2 className="w-4 h-4 animate-spin text-cyan-400" />
                      <span className="text-sm text-cyan-400">Procesando...</span>
                    </div>
                  )}
                  {isTyping && !loading && (
                    <div className="flex items-center gap-2 ml-2">
                      <div className="flex gap-1">
                        {[...Array(3)].map((_, i) => (
                          <div
                            key={i}
                            className="w-1.5 h-1.5 bg-emerald-400 rounded-full animate-bounce"
                            style={{ animationDelay: `${i * 0.2}s` }}
                          />
                        ))}
                      </div>
                      <span className="text-sm text-emerald-400">Escribiendo...</span>
                    </div>
                  )}
                </CardTitle>
                <div className="flex items-center gap-2">
                  {executionTime && (
                    <Badge
                      variant="outline"
                      className="flex items-center gap-1 border-yellow-500/30 text-yellow-400 bg-yellow-500/10 hover:bg-yellow-500/20 transition-all duration-300"
                    >
                      <Zap className="w-3 h-3" />
                      {executionTime}ms
                    </Badge>
                  )}
                  {output && (
                    <Button variant="ghost" size="sm" onClick={() => copyToClipboard(output)} className="h-8 w-8 p-0 text-gray-400 hover:text-white hover:scale-110 transition-all duration-200">
                      {copied ? <Check className="w-4 h-4 text-green-500" /> : <Copy className="w-4 h-4" />}
                    </Button>
                  )}
                </div>
              </div>
            </CardHeader>
            <CardContent>
              <Tabs defaultValue="output" className="w-full">
                <TabsList className="grid w-full grid-cols-2 bg-gray-800/30 border border-purple-500/20 p-1 rounded-xl backdrop-blur-sm h-14">
                  <TabsTrigger 
                    value="output" 
                    className="flex items-center justify-center gap-2 h-full px-4 rounded-lg transition-all duration-300 data-[state=active]:bg-gradient-to-r data-[state=active]:from-emerald-500 data-[state=active]:to-teal-500 data-[state=active]:text-white data-[state=active]:shadow-lg data-[state=active]:shadow-emerald-500/25 data-[state=active]:transform data-[state=active]:scale-105 text-gray-400 hover:text-gray-200 hover:bg-gray-700/30"
                  >
                    <Terminal className="w-4 h-4 flex-shrink-0" />
                    <span className="font-medium">Salida</span>
                  </TabsTrigger>
                  <TabsTrigger 
                    value="assembly" 
                    className="flex items-center justify-center gap-2 h-full px-4 rounded-lg transition-all duration-300 data-[state=active]:bg-gradient-to-r data-[state=active]:from-orange-500 data-[state=active]:to-red-500 data-[state=active]:text-white data-[state=active]:shadow-lg data-[state=active]:shadow-orange-500/25 data-[state=active]:transform data-[state=active]:scale-105 text-gray-400 hover:text-gray-200 hover:bg-gray-700/30"
                  >
                    <Cpu className="w-4 h-4 flex-shrink-0" />
                    <span className="font-medium">Assembly</span>
                  </TabsTrigger>
                </TabsList>
                
                <TabsContent value="output" className="mt-6">
                  <div className="bg-gradient-to-br from-gray-950 to-gray-900 text-emerald-400 p-6 rounded-xl font-mono text-sm h-80 overflow-auto border border-emerald-500/20 shadow-inner shadow-emerald-500/5 transition-all duration-500">
                    {loading ? (
                      <div className="flex items-center justify-center h-full">
                        <div className="flex flex-col items-center gap-4 text-emerald-300">                            <div className="relative">
                              <Loader2 className="w-8 h-8 animate-spin" />
                              <div className="absolute inset-0 w-8 h-8 border-2 border-emerald-400/20 rounded-full animate-ping opacity-50"></div>
                            </div>
                            <span className="text-base">Ejecutando código...</span>
                            <div className="flex gap-1">
                              {[...Array(5)].map((_, i) => (
                                <div
                                  key={i}
                                  className="w-2 h-2 bg-emerald-400 rounded-full animate-bounce"
                                  style={{ animationDelay: `${i * 0.05}s` }}
                                />
                              ))}
                            </div>
                        </div>
                      </div>
                    ) : (
                      <div className="relative">
                        <pre className="whitespace-pre-wrap leading-relaxed animate-fadeIn">
                          {displayedOutput || (
                            <div className="flex items-center justify-center h-full text-gray-500">
                              <div className="text-center">
                                <Terminal className="w-12 h-12 mx-auto mb-3 text-gray-600 animate-gentle-float" />
                                <p className="text-base">Ejecuta el compilador para ver la salida</p>
                                <p className="text-sm mt-1">Los resultados aparecerán aquí</p>
                              </div>
                            </div>
                          )}
                          {isTyping && (
                            <span className="inline-block w-0.5 h-5 bg-emerald-400 ml-1 typing-cursor"></span>
                          )}
                        </pre>
                      </div>
                    )}
                  </div>
                </TabsContent>
                
                <TabsContent value="assembly" className="mt-6">
                  <div className="h-80 flex flex-col">
                    {assembly ? (
                      <>
                        <div className="flex items-center justify-between p-4 bg-gray-800/30 rounded-lg border border-orange-500/20 mb-4 flex-shrink-0 backdrop-blur-sm transition-all duration-300 hover:border-orange-400/40">
                          <div className="flex items-center gap-3">
                            <div className="p-2 bg-orange-500/20 rounded-lg transform hover:scale-110 transition-all duration-300">
                              <Cpu className="w-4 h-4 text-orange-400" />
                            </div>
                            <div>
                              <div className="text-sm font-medium text-gray-200">Código Assembly</div>
                              <div className="text-xs text-gray-400">Generado exitosamente • {assembly.split('\n').length} líneas</div>
                            </div>
                          </div>
                          <div className="flex items-center gap-2">
                            <Button
                              onClick={downloadAssembly}
                              size="sm"
                              variant="outline"
                              className="bg-gradient-to-r from-indigo-500/10 to-purple-600/10 hover:from-indigo-500/20 hover:to-purple-600/20 text-indigo-300 border-indigo-500/30 hover:border-indigo-400/50 shadow-lg shadow-indigo-500/10 transition-all duration-300 hover:scale-105"
                            >
                              <Download className="w-4 h-4 mr-2" />
                              Descargar
                            </Button>
                            {canExecuteAssembly && (
                              <Button
                                onClick={executeAssembly}
                                disabled={executingAssembly}
                                size="sm"
                                className="bg-gradient-to-r from-green-500 to-emerald-600 hover:from-green-600 hover:to-emerald-700 text-white shadow-lg shadow-green-500/25 transition-all duration-300 hover:scale-105 relative overflow-hidden"
                              >
                                {executingAssembly && (
                                  <div className="absolute inset-0 bg-gradient-to-r from-transparent via-white/20 to-transparent animate-pulse"></div>
                                )}
                                {executingAssembly ? (
                                  <>
                                    <Loader2 className="w-4 h-4 mr-2 animate-spin" />
                                    Ejecutando...
                                  </>
                                ) : (
                                  <>
                                    <Rocket className="w-4 h-4 mr-2" />
                                    Ejecutar
                                  </>
                                )}
                              </Button>
                            )}
                          </div>
                        </div>
                        <div className="bg-gradient-to-br from-gray-950 to-gray-900 text-amber-400 p-6 rounded-xl font-mono text-sm overflow-auto border border-amber-500/20 shadow-inner shadow-amber-500/5 flex-1 transition-all duration-500">
                          <pre className="whitespace-pre-wrap leading-relaxed animate-fadeIn">{assembly}</pre>
                        </div>
                      </>
                    ) : (
                      <div className="bg-gradient-to-br from-gray-950 to-gray-900 text-gray-500 p-6 rounded-xl font-mono text-sm h-full overflow-auto border border-gray-500/20 shadow-inner flex items-center justify-center transition-all duration-500">
                        <div className="text-center">
                          <Cpu className="w-12 h-12 mx-auto mb-3 text-gray-600 animate-gentle-float" />
                          <p className="text-base mb-2">Código Assembly no disponible</p>
                          <p className="text-sm text-gray-600">Cambia a modo &quot;Assembly&quot; y compila para generar código</p>
                          {mode !== "assembly" && (
                            <div className="mt-4">
                              <Button
                                onClick={() => setMode("assembly")}
                                size="sm"
                                variant="outline"
                                className="text-purple-400 border-purple-500/30 hover:bg-purple-500/10 transition-all duration-300 hover:scale-105"
                              >
                                <Cpu className="w-4 h-4 mr-2" />
                                Cambiar a Assembly
                              </Button>
                            </div>
                          )}
                        </div>
                      </div>
                    )}
                  </div>
                </TabsContent>
              </Tabs>
            </CardContent>
          </Card>
        </div>
      </div>
    </div>
    </>
  )
}
