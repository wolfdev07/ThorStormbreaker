# ?? Guía de Integración CEF con Django

## ?? Resumen de la Integración

Has integrado **Chromium Embedded Framework (CEF) 143** en ThorStormbreaker, lo que te permite:

? Renderizar aplicaciones Django modernas  
? Soporte completo para React, Vue, Angular  
? Tailwind CSS y frameworks CSS modernos  
? JavaScript ES2023+  
? IndexedDB, WebSockets, Service Workers  
? WebGL, Canvas, APIs modernas del navegador  

## ?? Distribución Elegida: **Standard Distribution**

### ¿Por qué Standard?

| Característica | Standard | Minimal | Justificación |
|----------------|----------|---------|---------------|
| **Tamaño** | 304 MB | 143 MB | El espacio adicional vale la pena |
| **Ejemplos** | ? Incluidos | ? No | Acelera desarrollo |
| **Debugging** | ? Completo | ?? Limitado | Esencial para producción |
| **Documentación** | ? Extensa | ?? Básica | Mejor soporte |
| **Actualizaciones** | ? Fáciles | ?? Complejas | Escalabilidad |

### Performance y Escalabilidad

**Standard Distribution** es MEJOR para tu caso porque:

1. **Performance**:
   - Incluye optimizaciones precompiladas
   - Símbolos de release optimizados
   - Cache de recursos más eficiente

2. **Escalabilidad**:
   - Fácil agregar nuevas features
   - Compatible con extensiones futuras
   - Mejor para equipos grandes

3. **Producción**:
   - Mejor debugging en caso de errores
   - Logs más detallados
   - Profiling incluido

## ?? Instalación Automática

El sistema descarga y configura CEF automáticamente:

```bash
# 1. Configurar proyecto (descarga CEF automáticamente)
cmake -B out/build/x64-release -G "Ninja"

# 2. Compilar
cmake --build out/build/x64-release

# 3. Ejecutar
out/build/x64-release/ThorStormbreaker.exe
```

### Primera Ejecución

La primera vez que configures el proyecto:

```
CEF: Detectada arquitectura x64
CEF no encontrado. Descargando CEF 143.0.14...
URL: https://cef-builds.spotifycdn.com/...
NOTA: Esta descarga es de ~304 MB y puede tardar varios minutos

Descargando... [=====     ] 45%
```

**Tiempo estimado**: 5-15 minutos dependiendo de tu conexión.

## ?? Arquitecturas Soportadas

El sistema detecta automáticamente tu arquitectura:

```cmake
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit ? descarga windows64
else()
    # 32-bit ? descarga windows32
endif()
```

## ?? Configuración del Proyecto

### Estructura de Directorios

Después de la descarga:

```
ThorStormbreaker/
??? external/
?   ??? libzkfp/           # SDK de huellas
?   ??? cef/               # ? CEF se instala aquí
?       ??? cef_binary_143.0.14+...windows64/
?           ??? include/   # Headers de CEF
?           ??? libcef_dll/# Wrapper DLL
?           ??? Release/   # Binarios de producción
?           ??? Resources/ # Recursos de Chromium
?           ??? cmake/     # Módulos CMake
??? cmake/
?   ??? DownloadCEF.cmake  # Script de descarga
??? services/
    ??? IBrowserService.h  # Interfaz del navegador
```

### Deshabilitar CEF (opcional)

Si solo quieres el sistema de huellas:

```cmake
# En CMakeLists.txt
option(USE_CEF "Incluir CEF" OFF)  # ? Cambiar a OFF
```

O desde línea de comandos:

```bash
cmake -B build -DUSE_CEF=OFF
```

## ?? Uso con Django

### Escenario 1: Django Dev Server (desarrollo)

```cpp
#include "services/IBrowserService.h"

int main() {
    auto browser = std::make_unique<BrowserServiceImpl>();
    
    browser->initialize(argc, argv);
    
    // Apuntar al servidor de desarrollo Django
    browser->createBrowser(
        "http://127.0.0.1:8000",  // Django dev server
        "Mi App Django",
        1920, 1080
    );
    
    // Loop de eventos
    return browser->runMessageLoop();
}
```

### Escenario 2: Django + React Bundle (producción)

```cpp
// Django sirve el bundle compilado de React
browser->createBrowser(
    "http://localhost:8000/app/",  // Ruta Django que sirve React
    "App con React + Tailwind",
    1280, 720
);

// Ejecutar JavaScript en el contexto de React
browser->executeJavaScript(R"(
    console.log('CEF integrado con React!');
    // Aquí puedes comunicarte con tu app React
)");
```

### Escenario 3: Aplicación Standalone

```cpp
// Empaquetar Django + CEF en un ejecutable único
// Django corre en thread separado
std::thread djangoThread([]() {
    // Iniciar Django en puerto 8001
    system("python manage.py runserver 8001 --noreload");
});

// CEF se conecta al servidor local
browser->createBrowser("http://127.0.0.1:8001");
```

## ?? Comunicación Django ? CEF

### JavaScript ? Django (Fetch API)

```javascript
// En tu app React/Django
async function guardarHuella(templateData) {
    const response = await fetch('/api/huellas/', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'X-CSRFToken': getCookie('csrftoken')
        },
        body: JSON.stringify({
            template: templateData,
            user_id: 123
        })
    });
    
    return await response.json();
}
```

### C++ ? JavaScript (executeJavaScript)

```cpp
// Llamar funciones JavaScript desde C++
std::string userId = "123";
std::string code = "window.setUserId('" + userId + "');";
browser->executeJavaScript(code);

// Obtener resultado de JavaScript
std::string result;
browser->executeJavaScriptWithResult(
    "JSON.stringify({nombre: 'Juan', id: 1})",
    result
);
// result = "{\"nombre\":\"Juan\",\"id\":1}"
```

### Django ? C++ (WebSockets)

```python
# Django view
from channels.generic.websocket import WebsocketConsumer

class FingerprintConsumer(WebsocketConsumer):
    def receive(self, text_data):
        data = json.loads(text_data)
        # Procesar comando desde CEF
        if data['action'] == 'scan':
            # Llamar al sistema de huellas
            template = scan_fingerprint()
            self.send(json.dumps({
                'template': template
            }))
```

## ?? APIs Web Soportadas (Chromium 143)

### ? Storage y Bases de Datos
- [x] **IndexedDB** - Base de datos NoSQL en el navegador
- [x] **LocalStorage** - Almacenamiento clave-valor persistente
- [x] **SessionStorage** - Almacenamiento de sesión
- [x] **Cache API** - Cache de recursos offline

### ? Comunicación
- [x] **Fetch API** - Peticiones HTTP modernas
- [x] **WebSockets** - Comunicación bidireccional en tiempo real
- [x] **Server-Sent Events** - Push desde servidor
- [x] **WebRTC** - Comunicación peer-to-peer

### ? JavaScript Moderno
- [x] **ES2023** - `async/await`, destructuring, modules
- [x] **Promises** - Manejo asíncrono
- [x] **Modules** - `import/export`
- [x] **Workers** - Web Workers para multithreading

### ? CSS Moderno
- [x] **Flexbox** - Layout flexible
- [x] **Grid** - Layout de cuadrícula
- [x] **Custom Properties** - Variables CSS (`--variable`)
- [x] **Animations** - `@keyframes`, transitions
- [x] **Media Queries** - Responsive design

### ? Frameworks Soportados
- [x] **React** 18+ (incluyendo Hooks, Suspense)
- [x] **Vue** 3+
- [x] **Angular** 17+
- [x] **Svelte** 4+
- [x] **Tailwind CSS** 3+
- [x] **Bootstrap** 5+
- [x] **Material UI**

### ? Gráficos y Multimedia
- [x] **Canvas 2D** - Dibujo 2D
- [x] **WebGL** - Gráficos 3D
- [x] **WebGL2** - Versión mejorada
- [x] **Video/Audio** - Reproducción multimedia
- [x] **Media Source Extensions** - Streaming adaptativo

### ? PWA y Service Workers
- [x] **Service Workers** - Funcionamiento offline
- [x] **Push Notifications** - Notificaciones push
- [x] **Background Sync** - Sincronización en segundo plano
- [x] **Web App Manifest** - Instalación como app

## ??? DevTools de Chrome

```cpp
// Abrir DevTools para debugging
browser->showDevTools();

// En tu código JavaScript, puedes usar:
console.log('Debugging en CEF');
console.table(datos);
debugger;  // Breakpoint
```

## ? Optimización de Performance

### 1. Cache de Django

```python
# settings.py
CACHES = {
    'default': {
        'BACKEND': 'django.core.cache.backends.redis.RedisCache',
        'LOCATION': 'redis://127.0.0.1:6379/1',
    }
}
```

### 2. Compresión de Assets

```python
# settings.py
MIDDLEWARE = [
    'django.middleware.gzip.GZipMiddleware',  # Comprimir respuestas
    # ...
]
```

### 3. Bundle Optimization

```javascript
// webpack.config.js (para React)
module.exports = {
    mode: 'production',
    optimization: {
        splitChunks: {
            chunks: 'all',
        },
    },
    performance: {
        hints: 'warning',
        maxAssetSize: 512000,  // 500 KB
        maxEntrypointSize: 512000,
    },
};
```

### 4. Lazy Loading en CEF

```cpp
// Cargar página inicial pequeña
browser->createBrowser("http://localhost:8000/loading");

// Luego cargar la app completa
browser->setOnLoadCompleteCallback([](const std::string& url) {
    if (url == "http://localhost:8000/loading") {
        browser->loadURL("http://localhost:8000/app");
    }
});
```

## ?? Integración Completa: Huellas + Web

```cpp
#include "services/IFingerprintService.h"
#include "services/IBrowserService.h"

int main() {
    // Inicializar servicios
    auto fpService = std::make_unique<FingerprintServiceImpl>();
    auto browser = std::make_unique<BrowserServiceImpl>();
    
    fpService->initialize();
    fpService->openDevice(0);
    
    browser->initialize(argc, argv);
    browser->createBrowser("http://localhost:8000");
    
    // Capturar huella y enviar a Django
    std::vector<unsigned char> image, fpTemplate;
    if (fpService->acquireFingerprint(image, fpTemplate)) {
        // Convertir template a base64
        std::string templateB64 = base64_encode(fpTemplate);
        
        // Enviar a JavaScript/Django
        std::string js = "window.sendFingerprintToServer('" + templateB64 + "');";
        browser->executeJavaScript(js);
    }
    
    return browser->runMessageLoop();
}
```

## ?? Ejemplos Prácticos

Ver carpeta `examples/` para:
- ? Ejemplo básico de navegador
- ? Django + React + CEF
- ? Sistema de autenticación con huellas + web
- ? Captura de huellas desde interfaz web

## ?? Recursos Adicionales

- **CEF Forum**: https://magpcss.org/ceforum/
- **CEF Tutorial**: https://bitbucket.org/chromiumembedded/cef/wiki/Tutorial
- **API Reference**: https://magpcss.org/ceforum/apidocs3/

---

**¡Tu sistema está listo para renderizar aplicaciones web modernas!** ??
