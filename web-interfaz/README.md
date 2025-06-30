# Interfaz Web para el Compilador

Esta es la interfaz web para el compilador de lenguaje personalizado.

## Características

- ✅ Editor de código con sintaxis highlighting
- ✅ Compilación en tiempo real
- ✅ Visualización del output del programa
- ✅ Generación y descarga de código assembly
- ✅ Ejemplos predefinidos (básico, if-else, while, for)
- ✅ Dos modos: Evaluación y Assembly

## Estructura

```
web-interfaz/
├── src/
│   ├── app/
│   │   ├── api/compile/route.ts    # API backend para compilar
│   │   └── page.tsx                # Página principal
│   └── components/
│       └── CodeEditor.tsx          # Componente del editor
├── package.json
└── README.md
```

## Instalación y Uso

1. **Instalar dependencias:**
   ```bash
   cd web-interfaz
   npm install
   ```

2. **Ejecutar en modo desarrollo:**
   ```bash
   npm run dev
   ```

3. **Abrir en el navegador:**
   ```
   http://localhost:3000
   ```

## Cómo funciona

1. **Frontend:** Next.js con React y Tailwind CSS
2. **Editor:** Monaco Editor (mismo que VS Code)
3. **Backend:** API Routes de Next.js que ejecutan el compilador C++
4. **Comunicación:** 
   - Usuario escribe código → Frontend lo envía al API
   - API guarda el código en `inputs/web/`
   - API ejecuta `main.exe` con el archivo
   - API retorna el output y el archivo assembly (si aplica)
   - Frontend muestra los resultados

## Flujo de trabajo

1. El usuario escribe código en el editor web
2. Al hacer clic en "Compilar", se envía una petición POST a `/api/compile`
3. El API guarda el código en un archivo temporal en `inputs/web/`
4. Se ejecuta el compilador C++ (`main.exe`) con ese archivo
5. Se captura la salida estándar y se lee el archivo assembly generado
6. Los resultados se muestran en la interfaz web
7. El usuario puede descargar el archivo assembly

## Modos de ejecución

- **Evaluar:** Ejecuta el código y muestra la salida del programa
- **Assembly:** Además genera y muestra el código assembly, permitiendo descargarlo

## Ejemplos incluidos

- **Básico:** Operaciones simples con variables
- **If-Else:** Condicionales
- **While:** Bucles while
- **For:** Bucles for con rangos
