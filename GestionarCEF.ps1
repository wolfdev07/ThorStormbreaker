# GestionarCEF.ps1 - Script para gestionar Chromium Embedded Framework
# Uso: .\GestionarCEF.ps1 [comando]

param(
    [Parameter(Position=0)]
    [ValidateSet("info", "descargar", "limpiar", "verificar", "ayuda")]
    [string]$Comando = "info"
)

$CEF_VERSION = "143.0.14+gdd46a37+chromium-143.0.7499.193"
$CEF_DIR = "external\cef"

function Mostrar-Banner {
    Write-Host "??????????????????????????????????????????????????" -ForegroundColor Cyan
    Write-Host "?   Gestión de Chromium Embedded Framework      ?" -ForegroundColor Cyan
    Write-Host "?   ThorStormbreaker                            ?" -ForegroundColor Cyan
    Write-Host "??????????????????????????????????????????????????" -ForegroundColor Cyan
    Write-Host ""
}

function Mostrar-Info {
    Mostrar-Banner
    
    Write-Host "VERSIÓN DE CEF" -ForegroundColor Yellow
    Write-Host "  CEF: $CEF_VERSION" -ForegroundColor White
    Write-Host "  Chromium: 143.0.7499.193" -ForegroundColor White
    Write-Host ""
    
    Write-Host "ARQUITECTURA DEL SISTEMA" -ForegroundColor Yellow
    if ([Environment]::Is64BitOperatingSystem) {
        Write-Host "  ? Windows 64-bit detectado" -ForegroundColor Green
        $plataforma = "windows64"
    } else {
        Write-Host "  ? Windows 32-bit detectado" -ForegroundColor Green
        $plataforma = "windows32"
    }
    Write-Host "  Binarios requeridos: cef_binary_..._$plataforma.tar.bz2" -ForegroundColor Gray
    Write-Host ""
    
    Write-Host "ESTADO DE LA INSTALACIÓN" -ForegroundColor Yellow
    $cefPath = Join-Path $PSScriptRoot "$CEF_DIR\cef_binary_${CEF_VERSION}_${plataforma}"
    
    if (Test-Path $cefPath) {
        Write-Host "  ? CEF está instalado" -ForegroundColor Green
        Write-Host "  Ubicación: $cefPath" -ForegroundColor Gray
        
        # Calcular tamaño
        $size = (Get-ChildItem -Path $cefPath -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB
        Write-Host "  Tamaño: $([math]::Round($size, 2)) MB" -ForegroundColor Gray
        
        # Verificar archivos clave
        Write-Host ""
        Write-Host "  ARCHIVOS CLAVE:" -ForegroundColor Cyan
        
        $archivos = @{
            "libcef.dll" = "Release\libcef.dll"
            "libcef_dll_wrapper" = "libcef_dll_wrapper\Release\libcef_dll_wrapper.lib"
            "resources" = "Resources\icudtl.dat"
            "cmake" = "cmake\cef_variables.cmake"
        }
        
        foreach ($nombre in $archivos.Keys) {
            $rutaArchivo = Join-Path $cefPath $archivos[$nombre]
            if (Test-Path $rutaArchivo) {
                Write-Host "    ? $nombre" -ForegroundColor Green
            } else {
                Write-Host "    ? $nombre (faltante)" -ForegroundColor Red
            }
        }
    } else {
        Write-Host "  ? CEF NO está instalado" -ForegroundColor Red
        Write-Host "  Ejecute: .\GestionarCEF.ps1 descargar" -ForegroundColor Yellow
    }
    
    Write-Host ""
    Write-Host "CARACTERÍSTICAS SOPORTADAS" -ForegroundColor Yellow
    Write-Host "  ? JavaScript ES2023+" -ForegroundColor Green
    Write-Host "  ? React, Vue, Angular, Svelte" -ForegroundColor Green
    Write-Host "  ? Tailwind CSS, Bootstrap" -ForegroundColor Green
    Write-Host "  ? IndexedDB, WebSockets" -ForegroundColor Green
    Write-Host "  ? Canvas, WebGL, WebGL2" -ForegroundColor Green
    Write-Host "  ? Service Workers, PWA" -ForegroundColor Green
    Write-Host ""
}

function Descargar-CEF {
    Mostrar-Banner
    
    Write-Host "DESCARGA DE CEF" -ForegroundColor Yellow
    Write-Host ""
    
    # Detectar arquitectura
    if ([Environment]::Is64BitOperatingSystem) {
        $plataforma = "windows64"
    } else {
        $plataforma = "windows32"
    }
    
    $nombreArchivo = "cef_binary_${CEF_VERSION}_${plataforma}.tar.bz2"
    $url = "https://cef-builds.spotifycdn.com/$nombreArchivo"
    $cefPath = Join-Path $PSScriptRoot "$CEF_DIR"
    $archivoDestino = Join-Path $cefPath $nombreArchivo
    
    Write-Host "Plataforma: $plataforma" -ForegroundColor Cyan
    Write-Host "Archivo: $nombreArchivo" -ForegroundColor Cyan
    Write-Host "URL: $url" -ForegroundColor Gray
    Write-Host ""
    
    # Verificar si ya existe
    $directorioFinal = Join-Path $cefPath "cef_binary_${CEF_VERSION}_${plataforma}"
    if (Test-Path $directorioFinal) {
        Write-Host "CEF ya está descargado en:" -ForegroundColor Green
        Write-Host $directorioFinal -ForegroundColor Gray
        Write-Host ""
        Write-Host "¿Desea volver a descargar? (S/N): " -NoNewline -ForegroundColor Yellow
        $respuesta = Read-Host
        if ($respuesta -ne "S" -and $respuesta -ne "s") {
            Write-Host "Operación cancelada" -ForegroundColor Yellow
            return
        }
        
        Write-Host "Eliminando instalación anterior..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $directorioFinal
    }
    
    # Crear directorio si no existe
    if (-not (Test-Path $cefPath)) {
        New-Item -ItemType Directory -Path $cefPath | Out-Null
    }
    
    Write-Host "? ADVERTENCIA: Esta descarga es de ~304 MB" -ForegroundColor Yellow
    Write-Host "  Puede tardar 5-15 minutos dependiendo de su conexión" -ForegroundColor Gray
    Write-Host ""
    Write-Host "¿Continuar con la descarga? (S/N): " -NoNewline -ForegroundColor Yellow
    $respuesta = Read-Host
    
    if ($respuesta -ne "S" -and $respuesta -ne "s") {
        Write-Host "Descarga cancelada" -ForegroundColor Yellow
        return
    }
    
    Write-Host ""
    Write-Host "Descargando CEF..." -ForegroundColor Green
    Write-Host "Esto puede tardar varios minutos..." -ForegroundColor Gray
    Write-Host ""
    
    try {
        # Descargar con barra de progreso
        $ProgressPreference = 'Continue'
        Invoke-WebRequest -Uri $url -OutFile $archivoDestino -UseBasicParsing
        
        Write-Host ""
        Write-Host "? Descarga completada" -ForegroundColor Green
        Write-Host ""
        
        # Extraer archivo
        Write-Host "Extrayendo archivo..." -ForegroundColor Green
        Write-Host "Esto puede tardar 1-2 minutos..." -ForegroundColor Gray
        
        # Usar 7zip si está disponible, sino tar de Windows
        $7zipPath = "C:\Program Files\7-Zip\7z.exe"
        if (Test-Path $7zipPath) {
            & $7zipPath x $archivoDestino -o"$cefPath" -y | Out-Null
        } else {
            # Windows 10+ tiene tar integrado
            tar -xjf $archivoDestino -C $cefPath
        }
        
        Write-Host ""
        Write-Host "? Extracción completada" -ForegroundColor Green
        
        # Eliminar archivo comprimido
        Write-Host ""
        Write-Host "Limpiando archivos temporales..." -ForegroundColor Yellow
        Remove-Item $archivoDestino
        
        Write-Host ""
        Write-Host "??????????????????????????????????????????????????" -ForegroundColor Green
        Write-Host "?   ? CEF INSTALADO CORRECTAMENTE               ?" -ForegroundColor Green
        Write-Host "??????????????????????????????????????????????????" -ForegroundColor Green
        Write-Host ""
        Write-Host "Ubicación: $directorioFinal" -ForegroundColor Gray
        Write-Host ""
        Write-Host "Próximos pasos:" -ForegroundColor Yellow
        Write-Host "  1. Configurar proyecto: cmake -B build" -ForegroundColor White
        Write-Host "  2. Compilar: cmake --build build" -ForegroundColor White
        Write-Host ""
        
    } catch {
        Write-Host ""
        Write-Host "? Error durante la descarga:" -ForegroundColor Red
        Write-Host $_.Exception.Message -ForegroundColor Red
        Write-Host ""
        Write-Host "Posibles soluciones:" -ForegroundColor Yellow
        Write-Host "  - Verifique su conexión a internet" -ForegroundColor White
        Write-Host "  - Intente nuevamente más tarde" -ForegroundColor White
        Write-Host "  - Descargue manualmente desde: https://cef-builds.spotifycdn.com/" -ForegroundColor White
    }
}

function Limpiar-CEF {
    Mostrar-Banner
    
    Write-Host "LIMPIAR INSTALACIÓN DE CEF" -ForegroundColor Yellow
    Write-Host ""
    
    $cefPath = Join-Path $PSScriptRoot $CEF_DIR
    
    if (-not (Test-Path $cefPath)) {
        Write-Host "No hay instalación de CEF para limpiar" -ForegroundColor Gray
        return
    }
    
    # Calcular tamaño a liberar
    $size = (Get-ChildItem -Path $cefPath -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB
    
    Write-Host "Se liberarán aproximadamente $([math]::Round($size, 2)) MB" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "¿Está seguro de eliminar CEF? (S/N): " -NoNewline -ForegroundColor Yellow
    $respuesta = Read-Host
    
    if ($respuesta -eq "S" -or $respuesta -eq "s") {
        Write-Host ""
        Write-Host "Eliminando CEF..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $cefPath
        Write-Host "? CEF eliminado correctamente" -ForegroundColor Green
        Write-Host ""
        Write-Host "Para reinstalar, ejecute: .\GestionarCEF.ps1 descargar" -ForegroundColor Cyan
    } else {
        Write-Host ""
        Write-Host "Operación cancelada" -ForegroundColor Yellow
    }
    Write-Host ""
}

function Verificar-CEF {
    Mostrar-Banner
    
    Write-Host "VERIFICACIÓN DE INTEGRIDAD" -ForegroundColor Yellow
    Write-Host ""
    
    if ([Environment]::Is64BitOperatingSystem) {
        $plataforma = "windows64"
    } else {
        $plataforma = "windows32"
    }
    
    $cefPath = Join-Path $PSScriptRoot "$CEF_DIR\cef_binary_${CEF_VERSION}_${plataforma}"
    
    if (-not (Test-Path $cefPath)) {
        Write-Host "? CEF no está instalado" -ForegroundColor Red
        Write-Host ""
        return $false
    }
    
    Write-Host "Verificando archivos esenciales..." -ForegroundColor Cyan
    Write-Host ""
    
    $archivosEsenciales = @(
        "Release\libcef.dll",
        "Release\chrome_elf.dll",
        "Release\d3dcompiler_47.dll",
        "Release\libEGL.dll",
        "Release\libGLESv2.dll",
        "Resources\icudtl.dat",
        "Resources\chrome_100_percent.pak",
        "Resources\resources.pak",
        "include\cef_app.h",
        "include\cef_browser.h"
    )
    
    $todosPresentes = $true
    foreach ($archivo in $archivosEsenciales) {
        $rutaCompleta = Join-Path $cefPath $archivo
        if (Test-Path $rutaCompleta) {
            Write-Host "  ? $archivo" -ForegroundColor Green
        } else {
            Write-Host "  ? $archivo (FALTANTE)" -ForegroundColor Red
            $todosPresentes = $false
        }
    }
    
    Write-Host ""
    if ($todosPresentes) {
        Write-Host "? Todos los archivos esenciales están presentes" -ForegroundColor Green
        Write-Host "  CEF está listo para usar" -ForegroundColor Green
    } else {
        Write-Host "? Faltan archivos esenciales" -ForegroundColor Red
        Write-Host "  Recomendación: Volver a descargar CEF" -ForegroundColor Yellow
        Write-Host "  Ejecute: .\GestionarCEF.ps1 descargar" -ForegroundColor Yellow
    }
    Write-Host ""
    
    return $todosPresentes
}

function Mostrar-Ayuda {
    Mostrar-Banner
    
    Write-Host "USO: .\GestionarCEF.ps1 [comando]" -ForegroundColor White
    Write-Host ""
    Write-Host "COMANDOS DISPONIBLES:" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "  info       " -NoNewline -ForegroundColor Cyan
    Write-Host "- Muestra información de CEF y estado de instalación (por defecto)"
    Write-Host "  descargar  " -NoNewline -ForegroundColor Cyan
    Write-Host "- Descarga e instala CEF automáticamente"
    Write-Host "  verificar  " -NoNewline -ForegroundColor Cyan
    Write-Host "- Verifica la integridad de la instalación"
    Write-Host "  limpiar    " -NoNewline -ForegroundColor Cyan
    Write-Host "- Elimina la instalación de CEF (libera ~300 MB)"
    Write-Host "  ayuda      " -NoNewline -ForegroundColor Cyan
    Write-Host "- Muestra esta ayuda"
    Write-Host ""
    Write-Host "EJEMPLOS:" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "  .\GestionarCEF.ps1" -ForegroundColor Gray
    Write-Host "  .\GestionarCEF.ps1 info" -ForegroundColor Gray
    Write-Host "  .\GestionarCEF.ps1 descargar" -ForegroundColor Gray
    Write-Host "  .\GestionarCEF.ps1 verificar" -ForegroundColor Gray
    Write-Host ""
}

# Ejecutar comando
switch ($Comando) {
    "info" { Mostrar-Info }
    "descargar" { Descargar-CEF }
    "limpiar" { Limpiar-CEF }
    "verificar" { Verificar-CEF }
    "ayuda" { Mostrar-Ayuda }
    default { Mostrar-Info }
}
