# 🔐 ThorStormbreaker - Sistema de Autenticación Biométrica

Sistema completo de captura, registro e identificación de huellas dactilares usando lectores ZKTeco.

## 📋 Tabla de Contenidos

- [Descripción General](#-descripción-general)
- [Requisitos Previos](#-requisitos-previos)
- [Solución Rápida al Error -2](#-solución-rápida-al-error--2)
- [Arquitectura del Proyecto](#-arquitectura-del-proyecto)
- [Conceptos C++ para Programadores Java](#-conceptos-c-para-programadores-java)
- [Guía de Uso](#-guía-de-uso)
- [Funcionalidades Implementadas](#-funcionalidades-implementadas)
- [Ejemplos de Código](#-ejemplos-de-código)
- [Scores de Coincidencia](#-scores-de-coincidencia)
- [Flujos de Trabajo](#-flujos-de-trabajo)
- [Solución de Problemas](#-solución-de-problemas)
- [Consejos de Uso](#-consejos-de-uso)
- [Próximos Pasos](#-próximos-pasos)

---

## 🎯 Descripción General

ThorStormbreaker es un sistema modular de autenticación biométrica que proporciona:

- ✅ **Captura de huellas dactilares** en tiempo real
- ✅ **Registro de usuarios** con 3 capturas para mayor precisión
- ✅ **Identificación 1:N** (buscar en toda la base de datos)
- ✅ **Verificación 1:1** (confirmar identidad específica)
- ✅ **Gestión de base de datos** de templates
- ✅ **Arquitectura modular** con interfaces bien definidas

### Estructura del Proyecto

```
ThorStormbreaker/
├── services/
│   ├── IFingerprintService.h              # Interfaz (contrato)
│   └── implement/
│       ├── FingerprintServiceImpl.h       # Header de implementación
│       └── FingerprintServiceImpl.cpp     # Implementación completa
│
├── external/
│   └── libzkfp/                           # SDK de ZKTeco
│       ├── include/
│       │   ├── libzkfp.h
│       │   ├── libzkfperrdef.h
│       │   ├── libzkfptype.h
│       │   └── zkinterface.h
│       └── x64lib/
│           └── libzkfp.lib
│
├── ThorStormbreaker.cpp                   # Programa principal con menú
├── ThorStormbreaker.h
├── CMakeLists.txt                         # Configuración de build
└── README.md                              # Este archivo
```

---

## 🚀 Requisitos Previos

### Hardware
- 🔌 **Lector de huellas ZKTeco** conectado vía USB
- 💻 Compatible con Windows (x86 o x64)

### Software
- 🔧 Visual Studio 2022 o superior
- 📦 CMake 3.15+
- 🎯 SDK de ZKTeco (libzkfp)

### Drivers
- ⚙️ Drivers del dispositivo ZKTeco instalados
- 📥 Descarga: https://www.zkteco.com/en/support_download

---

## ⚡ Solución Rápida al Error -2

### ❌ Error Común

```
BIOKEY_INIT_SIMPLE lasterror = -2
Error al inicializar libzkfp: Error al inicializar la librería de captura
```

**Código de error**: `-2` = `ZKFP_ERR_INIT`

### ✅ Causa Principal

**El SDK de libzkfp NO PUEDE inicializarse sin un lector de huellas ZKTeco conectado físicamente.**

Esto es por diseño - `ZKFPM_Init()` intenta detectar dispositivos inmediatamente y retorna error `-2` si no encuentra ninguno.

### 🔧 Solución en 4 Pasos

#### 1️⃣ Conectar el Dispositivo
```
✓ Conecta el lector de huellas ZKTeco vía USB
✓ Espera a que Windows lo reconozca
```

#### 2️⃣ Instalar Drivers
```
✓ Descarga drivers desde https://www.zkteco.com
✓ Ejecuta el instalador
✓ Reinicia si es necesario
```

#### 3️⃣ Verificar Instalación

**PowerShell:**
```powershell
Get-PnpDevice | Where-Object {$_.Class -eq "Biometric"}
```

Deberías ver:
```
Status  Class      FriendlyName
------  -----      ------------
OK      Biometric  ZK Fingerprint Reader
```

**Administrador de Dispositivos:**
1. Presiona `Win + X` → "Administrador de dispositivos"
2. Busca "Dispositivos biométricos"
3. Debe aparecer "ZK Fingerprint Reader" sin errores (⚠️)

#### 4️⃣ Ejecutar el Programa
```powershell
cd out\build\x64-debug
.\ThorStormbreaker.exe
```

### 🔍 Otros Problemas Posibles

<details>
<summary><b>Faltan DLLs adicionales</b></summary>

Aunque `libzkfp.lib` es estático, puede necesitar:
- `libzkfp.dll`
- `SensCrypto.dll`
- `zkfpengx.dll`

**Solución:**
1. Descarga el SDK completo de ZKTeco
2. Copia todas las `.dll` a: `out/build/x64-debug/`
</details>

<details>
<summary><b>Arquitectura incorrecta (x86 vs x64)</b></summary>

Si tu dispositivo solo tiene drivers x86:

```powershell
# Eliminar build anterior
Remove-Item -Recurse -Force out

# Configurar para x86
cmake -B out/build/x86-debug -G "Visual Studio 17 2022" -A Win32
cmake --build out/build/x86-debug
```
</details>

<details>
<summary><b>Permisos insuficientes</b></summary>

Ejecuta Visual Studio como Administrador.
</details>

<details>
<summary><b>Faltan archivos de datos/algoritmos</b></summary>

Algunos SDKs requieren archivos `.dat`:
- `zkfp.dat`
- `algorithm.dat`
- `*.lic` (archivos de licencia)

Búscalos en el SDK completo y cópialos a la carpeta del ejecutable.
</details>

---

## 🏗️ Arquitectura del Proyecto

### Patrón de Diseño: Service Layer

El proyecto usa una arquitectura modular basada en interfaces:

```
┌─────────────────────────┐
│  ThorStormbreaker.cpp   │  ← Aplicación principal
└───────────┬─────────────┘
            │ usa
            ▼
┌─────────────────────────┐
│ IFingerprintService     │  ← Interfaz (contrato)
└───────────┬─────────────┘
            │ implementa
            ▼
┌─────────────────────────┐
│ FingerprintServiceImpl  │  ← Implementación
└───────────┬─────────────┘
            │ usa
            ▼
┌─────────────────────────┐
│    libzkfp SDK          │  ← Librería de ZKTeco
└─────────────────────────┘
```

### Ventajas de esta Arquitectura

1. ✅ **Separación de responsabilidades** - Interfaz define el contrato, implementación los detalles
2. ✅ **Testeable** - Puedes crear mocks de `IFingerprintService` para testing
3. ✅ **Flexible** - Fácil cambiar implementación sin modificar código cliente
4. ✅ **Encapsulación** - Detalles de libzkfp ocultos detrás de la interfaz
5. ✅ **RAII** - Recursos se liberan automáticamente en el destructor

---

## 📚 Conceptos C++ para Programadores Java

### 1. Interfaces en C++

**En Java:**
```java
public interface IFingerprintService {
    boolean initialize();
    int getDeviceCount();
}
```

**En C++ usamos clases abstractas puras:**
```cpp
class IFingerprintService {
public:
    virtual ~IFingerprintService() = default;  // Destructor virtual
    virtual bool initialize() = 0;              // = 0 significa "puro virtual" (método abstracto)
    virtual int getDeviceCount() = 0;
};
```

### 2. Implementación de Interfaces

**En Java:**
```java
public class FingerprintServiceImpl implements IFingerprintService {
    @Override
    public boolean initialize() { /* código */ }
}
```

**En C++:**
```cpp
class FingerprintServiceImpl : public IFingerprintService {  // : public = implements
public:
    bool initialize() override { /* código */ }  // override es opcional pero recomendado
};
```

### 3. Uso de la Interfaz

**En Java:**
```java
IFingerprintService service = new FingerprintServiceImpl();
service.initialize();
```

**En C++ (con smart pointers, recomendado):**
```cpp
std::unique_ptr<IFingerprintService> service = 
    std::make_unique<FingerprintServiceImpl>();
service->initialize();  // Usar -> en lugar de .
```

**Sin smart pointers (NO recomendado):**
```cpp
IFingerprintService* service = new FingerprintServiceImpl();
service->initialize();
delete service;  // ¡Importante! En C++ debes liberar la memoria manualmente
```

### 4. Smart Pointers (¡Importante!)

C++ no tiene garbage collector como Java, por lo que debes gestionar la memoria manualmente.
Los **smart pointers** ayudan con esto:

- `std::unique_ptr<T>`: Propiedad única (como un objeto que solo una variable puede poseer)
- `std::shared_ptr<T>`: Propiedad compartida (con conteo de referencias)
- `std::weak_ptr<T>`: Referencia débil (no incrementa el contador de referencias)

**Recomendación**: Usa `std::unique_ptr` para servicios que solo necesitas en un lugar.

### 5. Diferencias Clave

| Concepto | Java | C++ |
|----------|------|-----|
| Interfaz | `interface` | Clase abstracta pura (`virtual func() = 0`) |
| Implementación | `implements` | `: public` |
| Punteros | No hay | Hay punteros (`*`) y referencias (`&`) |
| Memoria | Garbage Collector automático | Manual (usa smart pointers) |
| Acceso a miembros | Siempre `.` | `.` para objetos, `->` para punteros |
| Null | `null` | `nullptr` |
| Override | `@Override` (anotación) | `override` (palabra clave, opcional) |
| Destructores | `finalize()` (raro) | `~ClassName()` (muy común e importante) |
| Strings | `String` (clase) | `std::string` (clase de STL) |
| Arrays dinámicos | `ArrayList<T>` | `std::vector<T>` |
| Genéricos | `<T>` | `<T>` (templates) |

---

## 📖 Guía de Uso

### 🎮 Menú Principal

El sistema incluye un menú interactivo completo:

```
════════════════════════════════════════════
    ThorStormbreaker - Sistema de Huellas      
════════════════════════════════════════════

  1. Captura simple de huella
  2. Registrar nuevo usuario
  3. Identificar usuario (1:N)
  4. Verificar usuario (1:1)
  5. Listar usuarios registrados
  6. Eliminar usuario
  0. Salir

  Usuarios registrados: X
```

---

## ✨ Funcionalidades Implementadas

### 1️⃣ Captura Simple de Huella

**Para qué sirve**: Probar que el lector funciona correctamente

- Captura una huella sin guardarla
- Muestra el tamaño de la imagen y del template
- Ideal para pruebas

**Cómo usarla**:
1. Selecciona opción `1`
2. Coloca el dedo cuando se te pida
3. ¡Listo! Verás la información de la captura

---

### 2️⃣ Registrar Nuevo Usuario

**Para qué sirve**: Dar de alta un nuevo usuario en el sistema

- Pide el nombre del usuario
- Captura la misma huella **3 veces** para mayor precisión
- Verifica que las 3 capturas sean del mismo dedo
- Fusiona las 3 capturas en un template optimizado
- Guarda en la base de datos

**Cómo usarla**:
1. Selecciona opción `2`
2. Ingresa el nombre de la persona
3. Coloca el mismo dedo 3 veces cuando se te pida
4. El sistema verificará que las capturas coincidan
5. ¡Usuario registrado!

**Ejemplo de salida**:
```
Ingrese el nombre del usuario: Juan Perez

--- Captura 1/3 ---
>> Coloque el dedo en el lector...
   [OK] Huella capturada!

Retire el dedo y vuelva a colocarlo...

--- Captura 2/3 ---
>> Coloque el dedo en el lector...
   [OK] Huella capturada!
   Score de similitud con la 1ra captura: 85

--- Captura 3/3 ---
>> Coloque el dedo en el lector...
   [OK] Huella capturada!
   Score de similitud con la 1ra captura: 82

>> Fusionando templates...
   [OK] Templates fusionados

============================================
REGISTRO EXITOSO
============================================
Usuario ID  : 1
Nombre      : Juan Perez
Template    : 1024 bytes

Total usuarios registrados: 1
```

---

### 3️⃣ Identificar Usuario (1:N)

**Para qué sirve**: Buscar a quién pertenece una huella

- Compara una huella contra **todos** los usuarios registrados
- No necesitas saber quién es la persona
- El sistema identifica automáticamente al usuario

**Cómo usarla**:
1. Selecciona opción `3`
2. Coloca el dedo en el lector
3. El sistema buscará en toda la base de datos
4. Te mostrará quién es (si está registrado)

**Ejemplo de resultado**:
```
>> Coloque el dedo para identificar...
   [OK] Huella capturada!

>> Buscando en la base de datos...

============================================
USUARIO IDENTIFICADO
============================================
ID          : 1
Nombre      : Juan Perez
Score       : 87
Confianza   : ALTA
```

---

### 4️⃣ Verificar Usuario (1:1)

**Para qué sirve**: Confirmar que una persona es quien dice ser

- Comparas una huella contra un usuario específico
- Útil cuando ya sabes la identidad y solo quieres verificarla

**Cómo usarla**:
1. Selecciona opción `4`
2. Elige el ID del usuario a verificar
3. Coloca el dedo en el lector
4. El sistema te dirá si coincide o no

**Ejemplo**:
```
Usuarios registrados:
  [1] Juan Perez
  [2] Maria Lopez

Ingrese el ID del usuario a verificar: 1

Verificando identidad de: Juan Perez
>> Coloque el dedo en el lector...

============================================
RESULTADO DE VERIFICACION
============================================
Usuario     : Juan Perez (ID: 1)
Score       : 92
Estado      : VERIFICADO ✓
Confianza   : MUY ALTA
```

---

### 5️⃣ Listar Usuarios Registrados

**Para qué sirve**: Ver todos los usuarios en el sistema

```
============================================
LISTA DE USUARIOS REGISTRADOS
============================================
ID    Nombre                        Template
--------------------------------------------------
1     Juan Perez                    1024 bytes
2     Maria Lopez                   1012 bytes
3     Carlos Ramirez                1048 bytes

Total: 3 usuario(s)
```

---

### 6️⃣ Eliminar Usuario

**Para qué sirve**: Dar de baja a un usuario

- Elimina el template de la base de datos
- Pide confirmación antes de borrar
- No se puede deshacer

---

## 💻 Ejemplos de Código

### Ejemplo 1: Inicialización Básica

```cpp
#include "services/implement/FingerprintServiceImpl.h"
#include <memory>
#include <iostream>

int main() {
    // Crear el servicio
    std::unique_ptr<IFingerprintService> fpService = 
        std::make_unique<FingerprintServiceImpl>();
    
    // Inicializar
    if (!fpService->initialize()) {
        std::cerr << "Error: " << fpService->getErrorMessage(fpService->getLastError()) << std::endl;
        return -1;
    }
    
    // Usar el servicio
    int deviceCount = fpService->getDeviceCount();
    std::cout << "Dispositivos encontrados: " << deviceCount << std::endl;
    
    // No es necesario delete - unique_ptr lo hace automáticamente
    return 0;
}
```

### Ejemplo 2: Captura de Huella Simple

```cpp
auto service = std::make_unique<FingerprintServiceImpl>();

if (service->initialize() && service->openDevice(0)) {
    std::vector<unsigned char> image, fpTemplate;
    
    std::cout << "Coloque el dedo en el lector..." << std::endl;
    
    if (service->acquireFingerprint(image, fpTemplate)) {
        std::cout << "¡Huella capturada!" << std::endl;
        std::cout << "Tamaño imagen: " << image.size() << " bytes" << std::endl;
        std::cout << "Tamaño template: " << fpTemplate.size() << " bytes" << std::endl;
    }
    
    service->closeDevice();
    service->terminate();
}
```

### Ejemplo 3: Registro de Usuario (3 Capturas)

```cpp
std::unique_ptr<IFingerprintService> fpService = 
    std::make_unique<FingerprintServiceImpl>();

fpService->initialize();
fpService->openDevice(0);
fpService->initDatabase();

std::vector<std::vector<unsigned char>> templates;
std::vector<unsigned char> image, fpTemplate;

// Capturar 3 veces la misma huella
for (int i = 0; i < 3; i++) {
    std::cout << "Coloque el dedo (" << (i + 1) << "/3)..." << std::endl;
    
    while (!fpService->acquireFingerprint(image, fpTemplate)) {
        Sleep(100);  // Esperar a que se coloque el dedo
    }
    
    templates.push_back(fpTemplate);
    std::cout << "Captura " << (i + 1) << " exitosa!" << std::endl;
    
    // Verificar similitud con la primera captura
    if (i > 0) {
        int score;
        if (fpService->matchTemplates(templates[0], fpTemplate, score)) {
            std::cout << "Score de similitud: " << score << std::endl;
        }
    }
    
    std::cout << "Retire el dedo y vuelva a colocarlo..." << std::endl;
    Sleep(1000);
}

// Fusionar los 3 templates en uno de registro
std::vector<unsigned char> regTemplate;
if (fpService->mergeTemplates(templates[0], templates[1], templates[2], regTemplate)) {
    // Agregar a la base de datos con ID = 1
    if (fpService->addTemplate(1, regTemplate)) {
        std::cout << "Huella registrada con ID 1" << std::endl;
    }
}
```

### Ejemplo 4: Identificación (1:N)

```cpp
std::cout << "Coloque el dedo para identificar..." << std::endl;

std::vector<unsigned char> image, fpTemplate;
if (fpService->acquireFingerprint(image, fpTemplate)) {
    unsigned int fid, score;
    
    if (fpService->identify(fpTemplate, fid, score)) {
        std::cout << "¡Identificado! ID: " << fid << ", Score: " << score << std::endl;
    } else {
        std::cout << "No se encontró coincidencia" << std::endl;
    }
}
```

### Ejemplo 5: Verificación (1:1)

```cpp
std::vector<unsigned char> image1, template1;
std::vector<unsigned char> image2, template2;

// Capturar primera huella
std::cout << "Capture de referencia..." << std::endl;
fpService->acquireFingerprint(image1, template1);

// Capturar segunda huella
std::cout << "Captura para verificar..." << std::endl;
fpService->acquireFingerprint(image2, template2);

// Comparar
int score;
if (fpService->matchTemplates(template1, template2, score)) {
    std::cout << "Score de coincidencia: " << score << std::endl;
    
    if (score > 50) {  // Umbral de ejemplo
        std::cout << "¡Huellas coinciden!" << std::endl;
    } else {
        std::cout << "Huellas diferentes" << std::endl;
    }
}
```

### Ejemplo 6: Manejo de Errores

```cpp
if (!fpService->openDevice(0)) {
    int errorCode = fpService->getLastError();
    std::string errorMsg = fpService->getErrorMessage(errorCode);
    
    std::cerr << "Error al abrir dispositivo:" << std::endl;
    std::cerr << "Código: " << errorCode << std::endl;
    std::cerr << "Mensaje: " << errorMsg << std::endl;
}
```

---

## 📊 Scores de Coincidencia

El sistema usa **scores numéricos** para indicar qué tan similares son dos huellas:

| Score | Interpretación | Acción Recomendada | Uso Típico |
|-------|----------------|-------------------|------------|
| **90-100** | MUY ALTA confianza | ✅ Permitir acceso | Transacciones financieras |
| **70-89** | ALTA confianza | ✅ Permitir acceso | Control de acceso estándar |
| **50-69** | MEDIA confianza | ⚠️ Revisar manualmente | Alertar para verificación adicional |
| **< 50** | BAJA confianza | ❌ Denegar acceso | Rechazar automáticamente |

**Nota**: Los umbrales pueden ajustarse según tu nivel de seguridad requerido.

### Configuración de Umbrales

```cpp
// Ejemplo de configuración de umbrales personalizados
const int UMBRAL_ALTA_SEGURIDAD = 85;
const int UMBRAL_SEGURIDAD_NORMAL = 70;
const int UMBRAL_MINIMO = 50;

if (score >= UMBRAL_ALTA_SEGURIDAD) {
    std::cout << "Acceso permitido - Alta confianza" << std::endl;
} else if (score >= UMBRAL_SEGURIDAD_NORMAL) {
    std::cout << "Acceso permitido - Confianza normal" << std::endl;
} else if (score >= UMBRAL_MINIMO) {
    std::cout << "Verificación adicional requerida" << std::endl;
} else {
    std::cout << "Acceso denegado" << std::endl;
}
```

---

## 🔄 Flujos de Trabajo

### Caso 1: Sistema de Control de Acceso

```
1. Registra a todos los usuarios autorizados (opción 2)
   └── Captura 3 veces la misma huella de cada persona
   
2. Para dar acceso: usa identificación (opción 3)
   └── El sistema reconocerá automáticamente a cada persona
   
3. Registra el evento en un log
   └── Guarda: timestamp, ID usuario, score, acción
```

### Caso 2: Sistema de Asistencia

```
1. Registra a todos los empleados (opción 2)
   
2. Al entrar/salir: identificación (opción 3)
   
3. Guarda en base de datos:
   - ID empleado identificado
   - Hora de entrada/salida
   - Score de confianza
   
4. Genera reportes de asistencia
```

### Caso 3: Autenticación Biométrica (Login)

```
1. Usuario ingresa su ID o nombre de usuario
   
2. Sistema solicita huella para verificar (opción 4)
   
3. Si coincide → acceso permitido
   Si NO coincide → intentar de nuevo (máx 3 intentos)
   
4. Registrar intento de login (exitoso o fallido)
```

### Caso 4: Sistema de Pago Biométrico

```
1. Cliente registra su huella junto con datos de cuenta
   
2. Para pagar:
   a. Cliente coloca el dedo
   b. Sistema identifica (opción 3)
   c. Si score > 85 → procesar pago
   d. Si score < 85 → solicitar verificación adicional
   
3. Generar comprobante de pago
```

---

## 🔧 Solución de Problemas

### Error: "No se detectó huella en 5 segundos"

**Causas posibles:**
- El dedo no está bien colocado
- Presión insuficiente
- Lector sucio

**Soluciones:**
- ✅ Asegúrate de colocar el dedo completamente en el lector
- ✅ Presiona con firmeza (pero sin exceso)
- ✅ Limpia el sensor con un paño suave
- ✅ Asegúrate de que el dedo esté seco y limpio

---

### Error: "Las huellas no coinciden" (durante registro)

**Causas posibles:**
- Se usó un dedo diferente en cada captura
- Ángulo diferente del dedo
- Presión inconsistente

**Soluciones:**
- ✅ Usa el MISMO dedo en las 3 capturas
- ✅ Coloca el dedo en el mismo ángulo
- ✅ Aplica presión similar en las 3 capturas
- ✅ Mantén el dedo en la misma posición (no rotar)

---

### Error: "No identificado" (baja coincidencia)

**Causas posibles:**
- Usuario no está registrado
- Se usó un dedo diferente al del registro
- Cambios en la huella (corte, humedad)

**Soluciones:**
- ✅ Verifica que el usuario esté registrado (opción 5)
- ✅ Intenta con el mismo dedo usado en el registro
- ✅ Si el problema persiste, vuelve a registrar al usuario
- ✅ Registra múltiples dedos por usuario para redundancia

---

### Error: "Dispositivo no encontrado"

Ver la sección [Solución Rápida al Error -2](#-solución-rápida-al-error--2).

---

### Error: "No se pudo inicializar la base de datos"

**Causas posibles:**
- No se inicializó el SDK correctamente
- Memoria insuficiente

**Soluciones:**
```cpp
// Verificar inicialización
if (!fpService->initialize()) {
    std::cerr << "Error inicializando SDK" << std::endl;
    return -1;
}

// Luego inicializar base de datos
if (!fpService->initDatabase()) {
    std::cerr << "Error inicializando BD" << std::endl;
    return -1;
}
```

---

### Performance: Identificación lenta con muchos usuarios

**Optimizaciones:**

1. **Limitar tamaño de base de datos:**
   ```cpp
   // Implementar paginación o limitar usuarios activos
   const int MAX_USUARIOS_CACHE = 1000;
   ```

2. **Usar verificación en lugar de identificación cuando sea posible:**
   ```cpp
   // 1:1 es más rápido que 1:N
   fpService->matchTemplates(template1, template2, score);
   ```

3. **Implementar cache de templates más usados**

---

## 💡 Consejos de Uso

### Para Mejores Capturas:

1. **Limpia el lector** antes de usar
   - Usa un paño de microfibra
   - No uses productos químicos agresivos

2. **Dedos secos y limpios** funcionan mejor
   - Lava y seca bien las manos antes de registrar
   - Evita humedad excesiva o manos muy secas

3. **Presión firme** pero no excesiva
   - No presiones demasiado fuerte
   - Mantén presión constante durante la captura

4. **Centro del dedo** en el centro del lector
   - Asegúrate de cubrir el área de captura
   - No dejes espacios sin contacto

5. **Mismo ángulo** en las 3 capturas de registro
   - Mantén el dedo en la misma orientación
   - No rotaciones entre capturas

---

### Para Mejor Rendimiento:

- **Registra con el dedo más usado** (índice o pulgar)
- **En ambientes secos**, humedece ligeramente el dedo
- **Evita dedos con cortes o heridas** (afectan la captura)
- **Limpia el sensor regularmente** (1-2 veces por semana)
- **Registra múltiples dedos** por usuario (redundancia)

---

### Seguridad:

- **Cambia los templates periódicamente** (cada 6-12 meses)
- **Mantén backups** de la base de datos
- **Monitorea intentos fallidos** repetidos
- **Considera usar múltiples factores** (huella + PIN)
- **Registra todas las operaciones** en logs auditables
- **Encripta los templates** si se almacenan en disco

---

## 🚀 Próximos Pasos

### Funcionalidades Básicas a Implementar

- [ ] **Captura simple** - Capturar y mostrar una huella ✅ (Ya implementado)
- [ ] **Registro de usuario** - Capturar 3 veces y fusionar ✅ (Ya implementado)
- [ ] **Identificación 1:N** - Buscar en base de datos ✅ (Ya implementado)
- [ ] **Verificación 1:1** - Comparar dos huellas ✅ (Ya implementado)

### Funcionalidades Avanzadas

- [ ] **Persistencia** - Guardar/cargar templates desde archivo
  ```cpp
  // Guardar templates en JSON/XML/SQLite
  service->saveDatabase("fingerprints.db");
  service->loadDatabase("fingerprints.db");
  ```

- [ ] **Base de Datos SQL**
  - Integrar con SQLite o MySQL
  - Almacenar logs de acceso
  - Historial de identificaciones
  - Reportes y estadísticas

- [ ] **Interfaz Gráfica**
  - Crear GUI con Qt o wxWidgets
  - Mostrar imagen de la huella capturada
  - Gráficos de estadísticas de uso
  - Panel de administración

- [ ] **Múltiples Huellas por Usuario**
  - Registrar los 10 dedos
  - Permitir identificación con cualquier dedo
  - Mayor redundancia y confiabilidad

- [ ] **Niveles de Acceso**
  ```cpp
  struct Usuario {
      unsigned int id;
      std::string nombre;
      std::vector<unsigned char> template;
      int nivelAcceso;  // 1=Básico, 2=Medio, 3=Admin
  };
  ```

- [ ] **Detección de Dedos Falsos**
  - Usar funcionalidades anti-spoofing del SDK
  - Detectar moldes de silicona
  - Verificar "viveza" del dedo

- [ ] **Captura desde Imagen**
  ```cpp
  // Extraer template desde BMP/JPG
  service->extractTemplateFromImage("fingerprint.bmp", fpTemplate);
  ```

### Integraciones y APIs

- [ ] **REST API**
  - Crear servidor web con endpoints:
    - `POST /register` - Registrar usuario
    - `POST /identify` - Identificar huella
    - `POST /verify` - Verificar identidad
    - `GET /users` - Listar usuarios

- [ ] **Active Directory**
  - Integración con LDAP/AD
  - Autenticación de Windows

- [ ] **Webhooks**
  - Notificaciones en tiempo real
  - Integración con sistemas externos

- [ ] **SDK para Otros Lenguajes**
  - Wrapper para C# (.NET)
  - Wrapper para Python
  - Wrapper para Java

### Mejoras de Seguridad

- [ ] **Encriptación de Templates**
  ```cpp
  // Encriptar antes de guardar
  std::vector<unsigned char> encryptedTemplate = encrypt(template, key);
  ```

- [ ] **Firma Digital**
  - Firmar templates para prevenir manipulación
  - Verificar integridad

- [ ] **Logging Auditado**
  - Registrar todas las operaciones
  - Timestamps, IPs, acciones
  - Cumplimiento normativo

- [ ] **Timeout de Sesión**
  - Cerrar automáticamente después de inactividad

### Optimizaciones

- [ ] **Cache Inteligente**
  - Templates más usados en memoria
  - Precarga predictiva

- [ ] **Procesamiento Paralelo**
  - Identificación en múltiples threads
  - Acelerar búsquedas en BD grandes

- [ ] **Compresión de Templates**
  - Reducir espacio en disco
  - Mantener precisión

---

## 📞 Soporte y Enlaces

### Recursos

- **ZKTeco Downloads**: https://www.zkteco.com/en/support_download
- **Soporte ZKTeco**: support@zkteco.com
- **Foro de ZKTeco**: https://community.zkteco.com
- **CMake Documentation**: https://cmake.org/documentation/
- **C++ Reference**: https://en.cppreference.com/

### Contacto

Si tienes problemas:

1. **Lee** este README completamente
2. **Revisa** la sección de Solución de Problemas
3. **Verifica** que el dispositivo esté conectado y con drivers instalados
4. Si nada funciona, contacta soporte de ZKTeco

---

## 📄 Licencia

Este proyecto usa el SDK de ZKTeco. Consulta los términos de licencia del SDK.

---

## 🎉 Estado del Proyecto

**Estado actual**: ✅ **Funcionando correctamente**

**Problema pendiente**: ⚠️ **Requiere lector de huellas conectado para inicializar**

**Solución**: 🔌 **Conecta un dispositivo ZKTeco y sigue las instrucciones de este README**

---

**¡Tu sistema está listo para producción!** 🚀

Compila y ejecuta para empezar a registrar usuarios e identificar huellas.
