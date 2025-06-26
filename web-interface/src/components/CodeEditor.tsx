"use client"

import { useState } from "react"
import dynamic from "next/dynamic"
import { Button } from "@/components/ui/button"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Tabs, TabsList, TabsTrigger, TabsContent } from "@/components/ui/tabs"
import { Badge } from "@/components/ui/badge"
import { Play, Download, Code2, Terminal, Cpu, Loader2, FileCode, Zap, Copy, Check } from "lucide-react"

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
}

export default function KotlinCompiler() {
  const [code, setCode] = useState(`fun main(): Int {
    var x : Int = 10
    var y : Int = 5
    var result : Int
    result = x + y
    println(result)
    return 0
}`)

  const [output, setOutput] = useState("")
  const [assembly, setAssembly] = useState("")
  const [loading, setLoading] = useState(false)
  const [mode, setMode] = useState<"eval" | "assembly">("eval")
  const [copied, setCopied] = useState(false)
  const [executionTime, setExecutionTime] = useState<number | null>(null)

  const examples = {
    variables: {
      name: "Variables",
      code: `fun main(): Int {
    var x : Int = 10
    var y : Int = 20
    var result : Int
    result = x + y
    println(result)
    return 0
}`,
    },
    expressions: {
      name: "Expresiones",
      code: `fun main(): Int {
    var x : Int = 5
    var y : Int = 3
    var result : Int
    result = x + y * 2
    println(result)
    result = (x + y) * 2
    println(result)
    return 0
}`,
    },
    conditionals: {
      name: "Condicionales",
      code: `fun main(): Int {
    var x : Int = 15
    var y : Int = 10
    var result : Int
    if (x > y) {
        result = x + y
        println(result)
    } else {
        result = x - y
        println(result)
    }
    return 0
}`,
    },
    floats: {
      name: "Números Decimales",
      code: `fun main(): Int {
    var x : Float = 10.5f
    var y : Float = 3.2f
    var result : Float
    result = x + y
    println(result)
    result = x * y
    println(result)
    return 0
}`,
    },
    functions: {
      name: "Funciones",
      code: `fun calculate(): Int {
    var temp : Int = 42
    println(temp)
    return temp
}

fun main(): Int {
    var result : Int
    result = calculate()
    println(result)
    return 0
}`,
    },
  }

  const handleCompile = async () => {
    setLoading(true)
    setOutput("")
    setAssembly("")
    setExecutionTime(null)

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
        if (result.assembly) {
          setAssembly(result.assembly)
        }
      } else {
        setOutput(`❌ Error: ${result.error}\n${result.errors || ""}`)
      }
    } catch (error) {
      setOutput(`❌ Error de conexión: ${error}`)
      setExecutionTime(Date.now() - startTime)
    }
    setLoading(false)
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
    const example = examples[exampleKey as keyof typeof examples]
    if (example) {
      setCode(example.code)
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
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-purple-900 to-violet-900 relative overflow-hidden">
      <div className="absolute inset-0 overflow-hidden pointer-events-none">
        <div className="absolute -top-40 -right-40 w-80 h-80 bg-purple-500/10 rounded-full blur-3xl animate-pulse"></div>
        <div className="absolute -bottom-40 -left-40 w-80 h-80 bg-cyan-500/10 rounded-full blur-3xl animate-pulse" style={{animationDelay: '1s'}}></div>
        <div className="absolute top-1/3 left-1/4 w-60 h-60 bg-pink-500/5 rounded-full blur-3xl animate-pulse" style={{animationDelay: '2s'}}></div>
      </div>
      <header className="bg-gray-900/70 backdrop-blur-xl border-b border-purple-500/20 sticky top-0 z-50">
        <div className="container mx-auto px-6 py-4">
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-4">
              <div className="flex items-center gap-3">
                <div className="p-2 bg-gradient-to-br from-cyan-500 to-blue-600 rounded-xl shadow-lg shadow-cyan-500/25">
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
              {/* Selector de ejemplos */}
              <Select onValueChange={loadExample}>
                <SelectTrigger className="w-48 bg-gray-800/50 border border-purple-500/30 text-gray-100 backdrop-blur-sm hover:bg-gray-700/50 transition-all duration-200 hover:border-purple-400/50 focus:border-cyan-400/50 focus:ring-2 focus:ring-cyan-400/25">
                  <SelectValue placeholder="Cargar ejemplo..." />
                </SelectTrigger>
                <SelectContent className="bg-gray-800 border border-purple-500/30 backdrop-blur-xl">
                  {Object.entries(examples).map(([key, example]) => (
                    <SelectItem key={key} value={key} className="text-gray-100 hover:bg-purple-500/20 focus:bg-purple-500/20 cursor-pointer">
                      <div className="flex items-center gap-2">
                        <FileCode className="w-4 h-4 text-cyan-400" />
                        {example.name}
                      </div>
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>

              {/* Selector de modo */}
              <Tabs value={mode} onValueChange={(value) => setMode(value as "eval" | "assembly")}>
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
          <Card className="bg-gray-900/80 backdrop-blur-xl border border-cyan-500/20 shadow-2xl shadow-cyan-500/10">
            <CardHeader className="pb-4">
              <div className="flex items-center justify-between">
                <CardTitle className="flex items-center gap-2 text-gray-100">
                  <div className="p-1.5 bg-gradient-to-br from-cyan-500 to-teal-600 rounded-lg shadow-lg shadow-cyan-500/25">
                    <Code2 className="w-4 h-4 text-white" />
                  </div>
                  Editor de Código
                </CardTitle>
                <Badge variant="secondary" className="bg-gradient-to-r from-purple-500/20 to-pink-500/20 text-purple-300 border border-purple-500/30 shadow-lg">
                  Kotlin
                </Badge>
              </div>
            </CardHeader>
            <CardContent className="p-0 h-full">
              <div className="h-[calc(100%-80px)] rounded-lg overflow-hidden border border-slate-200 dark:border-slate-700">
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
                  }}
                />
              </div>
            </CardContent>
          </Card>

          <Card className="bg-gray-900/80 backdrop-blur-xl border border-cyan-500/20 shadow-2xl shadow-cyan-500/10">
            <CardHeader className="pb-4">
              <div className="flex items-center justify-between">
                <CardTitle className="flex items-center gap-2 text-gray-100">
                  <div className="p-1.5 bg-gradient-to-br from-emerald-500 to-teal-600 rounded-lg shadow-lg shadow-emerald-500/25">
                    <Terminal className="w-4 h-4 text-white" />
                  </div>
                  Resultados
                </CardTitle>
                <div className="flex items-center gap-2">
                  {executionTime && (
                    <Badge
                      variant="outline"
                      className="flex items-center gap-1 border-yellow-500/30 text-yellow-400 bg-yellow-500/10"
                    >
                      <Zap className="w-3 h-3" />
                      {executionTime}ms
                    </Badge>
                  )}
                  {output && (
                    <Button variant="ghost" size="sm" onClick={() => copyToClipboard(output)} className="h-8 w-8 p-0 text-gray-400 hover:text-white">
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
                  <div className="bg-gradient-to-br from-gray-950 to-gray-900 text-emerald-400 p-6 rounded-xl font-mono text-sm h-80 overflow-auto border border-emerald-500/20 shadow-inner shadow-emerald-500/5">
                    {loading ? (
                      <div className="flex items-center justify-center h-full">
                        <div className="flex items-center gap-3 text-emerald-300">
                          <Loader2 className="w-5 h-5 animate-spin" />
                          <span className="text-base">Ejecutando código...</span>
                        </div>
                      </div>
                    ) : (
                      <pre className="whitespace-pre-wrap leading-relaxed">{output || (
                        <div className="flex items-center justify-center h-full text-gray-500">
                          <div className="text-center">
                            <Terminal className="w-12 h-12 mx-auto mb-3 text-gray-600" />
                            <p className="text-base">Ejecuta el compilador para ver la salida</p>
                            <p className="text-sm mt-1">Los resultados aparecerán aquí</p>
                          </div>
                        </div>
                      )}</pre>
                    )}
                  </div>
                </TabsContent>
                
                <TabsContent value="assembly" className="mt-6">
                  <div className="h-80 flex flex-col">
                    {assembly ? (
                      <>
                        <div className="flex items-center justify-between p-4 bg-gray-800/30 rounded-lg border border-orange-500/20 mb-4 flex-shrink-0">
                          <div className="flex items-center gap-3">
                            <div className="p-2 bg-orange-500/20 rounded-lg">
                              <Cpu className="w-4 h-4 text-orange-400" />
                            </div>
                            <div>
                              <div className="text-sm font-medium text-gray-200">Código Assembly</div>
                              <div className="text-xs text-gray-400">Generado exitosamente</div>
                            </div>
                          </div>
                          <Button
                            onClick={downloadAssembly}
                            size="sm"
                            className="bg-gradient-to-r from-indigo-500 to-purple-600 hover:from-indigo-600 hover:to-purple-700 text-white shadow-lg shadow-indigo-500/25 transition-all duration-300 hover:scale-105"
                          >
                            <Download className="w-4 h-4 mr-2" />
                            Descargar
                          </Button>
                        </div>
                        <div className="bg-gradient-to-br from-gray-950 to-gray-900 text-amber-400 p-6 rounded-xl font-mono text-sm overflow-auto border border-amber-500/20 shadow-inner shadow-amber-500/5 flex-1">
                          <pre className="whitespace-pre-wrap leading-relaxed">{assembly}</pre>
                        </div>
                      </>
                    ) : (
                      <div className="bg-gradient-to-br from-gray-950 to-gray-900 text-gray-500 p-6 rounded-xl font-mono text-sm h-full overflow-auto border border-gray-500/20 shadow-inner flex items-center justify-center">
                        <div className="text-center">
                          <Cpu className="w-12 h-12 mx-auto mb-3 text-gray-600" />
                          <p className="text-base mb-2">Código Assembly no disponible</p>
                          <p className="text-sm text-gray-600">Cambia a modo "Assembly" y compila para generar código</p>
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
  )
}
