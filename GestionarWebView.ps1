# GestionarWebView.ps1 - Script para gestionar WebView en ThorStormbreaker
# Uso: .\GestionarWebView.ps1 [comando]

param(
    [Parameter(Position=0)]
    [ValidateSet('compilar', 'ejecutar', 'servidor', 'todo', 'limpiar', 'ayuda')]
    [string]$Comando = 'ayuda'
)

$ErrorActionPreference = "Stop"

# Colores
function Write-Success { Write-Host $args -ForegroundColor Green }
function Write-Error { Write-Host $args -ForegroundColor Red }
function Write-Info { Write-Host $args -ForegroundColor Cyan }
function Write-Warning { Write-Host $args -ForegroundColor Yellow }

# Banner
function Show-Banner {
    Write-Host ""
    Write-Host "??????????????????????????????????????????????????" -ForegroundColor Magenta
    Write-Host "?    ThorStormbreaker - WebView Manager          ?" -ForegroundColor Magenta
    Write-Host "??????????????????????????????????????????????????" -ForegroundColor Magenta
    Write-Host ""
}

# Ayuda
function Show-Help {
    Show-Banner
    Write-Host "Uso: .\GestionarWebView.ps1 [comando]" -ForegroundColor White
    Write-Host ""
    Write-Host "Comandos disponibles:" -ForegroundColor Yellow
    Write-Host "  compilar   - Compila el proyecto con WebView" -ForegroundColor Gray
    Write-Host "  ejecutar   - Ejecuta la aplicación" -ForegroundColor Gray
    Write-Host "  servidor   - Inicia servidor web en localhost:8000" -ForegroundColor Gray
    Write-Host "  todo       - Compila y ejecuta" -ForegroundColor Gray
    Write-Host "  limpiar    - Limpia archivos de build" -ForegroundColor Gray
    Write-Host "  ayuda      - Muestra esta ayuda" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Ejemplos:" -ForegroundColor Yellow
    Write-Host "  .\GestionarWebView.ps1 todo" -ForegroundColor Gray
    Write-Host "  .\GestionarWebView.ps1 servidor" -ForegroundColor Gray
    Write-Host ""
}

# Compilar
function Invoke-Build {
    Write-Info "[1/2] Compilando proyecto..."
    
    $buildDir = "out/build/x64-debug"
    
    # Configurar si no existe
    if (-not (Test-Path "$buildDir/build.ninja")) {
        Write-Info "Configurando CMake..."
        & cmake -S . -B $buildDir -G Ninja -DCMAKE_BUILD_TYPE=Debug
        
        if ($LASTEXITCODE -ne 0) {
            Write-Error "? Error en configuración"
            exit 1
        }
    }
    
    # Compilar
    & cmake --build $buildDir
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "? Compilación completada"
    } else {
        Write-Error "? Error en compilación"
        exit 1
    }
}

# Ejecutar
function Invoke-Run {
    Write-Info "[2/2] Ejecutando aplicación..."
    
    $exePath = "out/build/x64-debug/ThorStormbreaker.exe"
    
    if (-not (Test-Path $exePath)) {
        Write-Error "? Ejecutable no encontrado. Compile primero."
        exit 1
    }
    
    Write-Host ""
    Write-Success "????????????????????????????????????????????????"
    Write-Success "  Iniciando ThorStormbreaker con WebView"
    Write-Success "????????????????????????????????????????????????"
    Write-Host ""
    
    & $exePath
}

# Servidor web
function Invoke-Server {
    Write-Info "Iniciando servidor web..."
    
    # Crear directorio www si no existe
    if (-not (Test-Path "www")) {
        Write-Info "Creando directorio www..."
        New-Item -ItemType Directory -Path "www" | Out-Null
    }
    
    # Crear index.html de ejemplo si no existe
    if (-not (Test-Path "www/index.html")) {
        Write-Info "Creando index.html de ejemplo..."
        
        @"
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ThorStormbreaker - WebView</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        
        .container {
            max-width: 800px;
            width: 100%;
        }
        
        .card {
            background: rgba(255, 255, 255, 0.95);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
            backdrop-filter: blur(10px);
        }
        
        h1 {
            color: #667eea;
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        
        .subtitle {
            color: #666;
            font-size: 1.2em;
            margin-bottom: 30px;
        }
        
        .status-box {
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border-radius: 10px;
            margin-bottom: 20px;
        }
        
        .status-box h2 {
            margin-bottom: 10px;
        }
        
        .buttons {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-top: 30px;
        }
        
        button {
            padding: 15px 30px;
            font-size: 16px;
            border: none;
            border-radius: 10px;
            cursor: pointer;
            transition: all 0.3s ease;
            font-weight: 600;
        }
        
        .btn-primary {
            background: #667eea;
            color: white;
        }
        
        .btn-secondary {
            background: #764ba2;
            color: white;
        }
        
        .btn-success {
            background: #28a745;
            color: white;
        }
        
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(0, 0, 0, 0.2);
        }
        
        button:active {
            transform: translateY(0);
        }
        
        #log {
            margin-top: 20px;
            padding: 20px;
            background: #f5f5f5;
            border-radius: 10px;
            max-height: 200px;
            overflow-y: auto;
            font-family: 'Courier New', monospace;
            font-size: 14px;
        }
        
        .log-entry {
            padding: 5px;
            border-bottom: 1px solid #ddd;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="card">
            <h1>?? ThorStormbreaker</h1>
            <p class="subtitle">Sistema de Huellas Dactilares v1.0</p>
            
            <div class="status-box">
                <h2>Estado del Sistema</h2>
                <p id="status">? WebView conectado correctamente</p>
                <p id="time"></p>
            </div>
            
            <div class="buttons">
                <button class="btn-primary" onclick="testFunction()">
                    Probar Función
                </button>
                <button class="btn-secondary" onclick="sendCommand('SCAN')">
                    Escanear Huella
                </button>
                <button class="btn-success" onclick="clearLog()">
                    Limpiar Log
                </button>
            </div>
            
            <div id="log">
                <div class="log-entry">Sistema iniciado...</div>
            </div>
        </div>
    </div>
    
    <script>
        // Actualizar reloj
        function updateTime() {
            const now = new Date();
            document.getElementById('time').textContent = 
                'Hora: ' + now.toLocaleTimeString();
        }
        
        setInterval(updateTime, 1000);
        updateTime();
        
        // Función de prueba
        function testFunction() {
            log('? Función ejecutada correctamente');
            document.getElementById('status').textContent = 
                '? Última acción: ' + new Date().toLocaleTimeString();
        }
        
        // Enviar comando (simulado)
        function sendCommand(cmd) {
            log('? Comando enviado: ' + cmd);
            console.log('[CMD]' + cmd);
            
            // Simular respuesta después de 1 segundo
            setTimeout(() => {
                log('? Respuesta recibida para: ' + cmd);
            }, 1000);
        }
        
        // Sistema de logging
        function log(message) {
            const logDiv = document.getElementById('log');
            const entry = document.createElement('div');
            entry.className = 'log-entry';
            entry.textContent = '[' + new Date().toLocaleTimeString() + '] ' + message;
            logDiv.appendChild(entry);
            logDiv.scrollTop = logDiv.scrollHeight;
        }
        
        function clearLog() {
            document.getElementById('log').innerHTML = 
                '<div class="log-entry">Log limpiado...</div>';
        }
        
        // Callback para C++ (si se usa loadUrl en vez de loadHTML)
        window.onCppMessage = function(data) {
            log('? Mensaje de C++: ' + JSON.stringify(data));
        };
        
        // Inicialización
        console.log('WebView UI cargada correctamente');
        log('? Interfaz web inicializada');
    </script>
</body>
</html>
"@ | Out-File -FilePath "www/index.html" -Encoding UTF8
        
        Write-Success "? index.html creado"
    }
    
    Write-Info "Servidor HTTP en http://localhost:8000"
    Write-Warning "Presione Ctrl+C para detener"
    Write-Host ""
    
    try {
        Set-Location www
        python -m http.server 8000
    } catch {
        Write-Error "? Error al iniciar servidor. ¿Python instalado?"
        Write-Info "Alternativa: npx http-server -p 8000"
    } finally {
        Set-Location ..
    }
}

# Limpiar
function Invoke-Clean {
    Write-Info "Limpiando archivos de build..."
    
    $buildDir = "out/build/x64-debug"
    
    if (Test-Path $buildDir) {
        Remove-Item -Recurse -Force $buildDir
        Write-Success "? Build limpiado"
    } else {
        Write-Warning "No hay nada que limpiar"
    }
}

# Todo
function Invoke-All {
    Show-Banner
    
    Invoke-Build
    Write-Host ""
    
    Write-Success "????????????????????????????????????????????????"
    Write-Success "? Compilación exitosa"
    Write-Success "????????????????????????????????????????????????"
    Write-Host ""
    
    Write-Info "Opciones:"
    Write-Host "  A. Ejecutar con HTML inline (actual): " -NoNewline
    Write-Host ".\GestionarWebView.ps1 ejecutar" -ForegroundColor Yellow
    Write-Host "  B. Usar servidor local:" -ForegroundColor White
    Write-Host "     1. Terminal 1: " -NoNewline
    Write-Host ".\GestionarWebView.ps1 servidor" -ForegroundColor Yellow
    Write-Host "     2. Terminal 2: Cambiar loadHTML() por loadUrl() en main()" -ForegroundColor Gray
    Write-Host "     3. Terminal 2: " -NoNewline
    Write-Host ".\GestionarWebView.ps1 ejecutar" -ForegroundColor Yellow
    Write-Host ""
}

# Main
Show-Banner

switch ($Comando) {
    'compilar' { Invoke-Build }
    'ejecutar' { Invoke-Run }
    'servidor' { Invoke-Server }
    'todo' { Invoke-All }
    'limpiar' { Invoke-Clean }
    'ayuda' { Show-Help }
    default { Show-Help }
}
