"use client"

import { useState } from "react"
import dynamic from "next/dynamic"
import { Button } from "@/components/ui/button"
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Tabs, TabsList, TabsTrigger } from "@/components/ui/tabs"
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
  const [code, setCode] = useState(`var x : Int = 10
var y : Int = 5
var result : Int

fun main(): Int {
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
    basic: {
      name: "Suma Básica",
      code: `var x : Int = 10
var y : Int = 5

fun main(): Int {
    println(x + y)
    return 0
}`,
    },
    "if-else": {
      name: "Condicionales",
      code: `var age : Int = 20
var status : Int

fun main(): Int {
    if (age >= 18) {
        status = 1
        println("Mayor de edad")
    } else {
        status = 0
        println("Menor de edad")
    }
    return 0
}`,
    },
    while: {
      name: "Bucle While",
      code: `var counter : Int = 0
var sum : Int = 0

fun main(): Int {
    while (counter < 5) {
        sum += counter
        println(counter)
        counter += 1
    }
    println("Suma total: " + sum)
    return 0
}`,
    },
    for: {
      name: "Bucle For",
      code: `var total : Int = 0

fun main(): Int {
    for (i in 1..5) {
        total += i
        println("Número: " + i)
    }
    println("Total: " + total)
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
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-purple-900 to-violet-900">
      {/* Header mejorado */}
      <header className="bg-gray-900/90 backdrop-blur-xl border-b border-purple-500/20 sticky top-0 z-50">
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
                <SelectTrigger className="w-48 bg-gray-800/80 border-purple-500/30 text-gray-100 backdrop-blur-sm hover:bg-gray-700/80 transition-colors">
                  <SelectValue placeholder="Cargar ejemplo..." />
                </SelectTrigger>
                <SelectContent>
                  {Object.entries(examples).map(([key, example]) => (
                    <SelectItem key={key} value={key}>
                      <div className="flex items-center gap-2">
                        <FileCode className="w-4 h-4" />
                        {example.name}
                      </div>
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>

              {/* Selector de modo */}
              <Tabs value={mode} onValueChange={(value) => setMode(value as "eval" | "assembly")}>
                <TabsList className="bg-gray-800/80 border border-purple-500/30 backdrop-blur-sm">
                  <TabsTrigger
                    value="eval"
                    className="flex items-center gap-2 data-[state=active]:bg-cyan-500 data-[state=active]:text-gray-900"
                  >
                    <Terminal className="w-4 h-4" />
                    Evaluar
                  </TabsTrigger>
                  <TabsTrigger
                    value="assembly"
                    className="flex items-center gap-2 data-[state=active]:bg-purple-500 data-[state=active]:text-white"
                  >
                    <Cpu className="w-4 h-4" />
                    Assembly
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
      <div className="container mx-auto px-6 py-8">
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
                <Badge variant="secondary" className="bg-purple-500/20 text-purple-300 border-purple-500/30">
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

          {/* Panel de resultados */}
          <div className="flex flex-col gap-6">
            {/* Salida del programa */}
            <Card className="bg-gray-900/80 backdrop-blur-xl border border-cyan-500/20 shadow-2xl shadow-cyan-500/10 flex-1">
              <CardHeader className="pb-4">
                <div className="flex items-center justify-between">
                  <CardTitle className="flex items-center gap-2 text-gray-100">
                    <div className="p-1.5 bg-gradient-to-br from-emerald-500 to-teal-600 rounded-lg shadow-lg shadow-emerald-500/25">
                      <Terminal className="w-4 h-4 text-white" />
                    </div>
                    {mode === "eval" ? "Salida del Programa" : "Resultado de Compilación"}
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
                      <Button variant="ghost" size="sm" onClick={() => copyToClipboard(output)} className="h-8 w-8 p-0">
                        {copied ? <Check className="w-4 h-4 text-green-500" /> : <Copy className="w-4 h-4" />}
                      </Button>
                    )}
                  </div>
                </div>
              </CardHeader>
              <CardContent>
                <div className="bg-gray-950 text-emerald-400 p-4 rounded-lg font-mono text-sm h-64 overflow-auto border border-emerald-500/20 shadow-inner">
                  {loading ? (
                    <div className="flex items-center gap-2 text-slate-400">
                      <Loader2 className="w-4 h-4 animate-spin" />
                      Ejecutando...
                    </div>
                  ) : (
                    <pre className="whitespace-pre-wrap">{output || "Ejecuta el compilador para ver la salida..."}</pre>
                  )}
                </div>
              </CardContent>
            </Card>

            {/* Panel de Assembly (si está disponible) */}
            {assembly && (
              <Card className="bg-gray-900/80 backdrop-blur-xl border border-cyan-500/20 shadow-2xl shadow-cyan-500/10 flex-1">
                <CardHeader className="pb-4">
                  <div className="flex items-center justify-between">
                    <CardTitle className="flex items-center gap-2 text-gray-100">
                      <div className="p-1.5 bg-gradient-to-br from-orange-500 to-red-500 rounded-lg shadow-lg shadow-orange-500/25">
                        <Cpu className="w-4 h-4 text-white" />
                      </div>
                      Código Assembly
                    </CardTitle>
                    <Button
                      onClick={downloadAssembly}
                      size="sm"
                      className="bg-gradient-to-r from-indigo-500 to-purple-600 hover:from-indigo-600 hover:to-purple-700 text-white shadow-lg shadow-indigo-500/25"
                    >
                      <Download className="w-4 h-4 mr-2" />
                      Descargar
                    </Button>
                  </div>
                </CardHeader>
                <CardContent>
                  <div className="bg-gray-950 text-amber-400 p-4 rounded-lg font-mono text-sm h-64 overflow-auto border border-amber-500/20 shadow-inner">
                    <pre className="whitespace-pre-wrap">{assembly}</pre>
                  </div>
                </CardContent>
              </Card>
            )}
          </div>
        </div>
      </div>
    </div>
  )
}
