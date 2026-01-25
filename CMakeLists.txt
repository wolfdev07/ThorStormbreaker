# CMakeList.txt: proyecto de CMake para ThorStormbreaker, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required (VERSION 3.16)

# Habilite Recarga activa para los compiladores de MSVC si se admiten.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ThorStormbreaker")

# ============================================
# Configuración de módulos CMake personalizados
# ============================================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# ============================================
# Cargar variables de entorno desde .env (opcional)
# ============================================
include(LoadEnvCMake)

# ============================================
# Configuración de libzkfp
# ============================================

# Ruta a las librerías libzkfp
set(LIBZKFP_DIR "${CMAKE_SOURCE_DIR}/external/libzkfp")

# Incluir directorios de headers
include_directories(${LIBZKFP_DIR}/include)

# Seleccionar la librería según la arquitectura
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
    set(LIBZKFP_LIB_DIR "${LIBZKFP_DIR}/x64lib")
    message(STATUS "Usando libzkfp x64")
else()
    # 32-bit  
    set(LIBZKFP_LIB_DIR "${LIBZKFP_DIR}/x86lib")
    message(STATUS "Usando libzkfp x86")
endif()

# Encontrar la librería
find_library(LIBZKFP_LIBRARY 
    NAMES libzkfp
    PATHS ${LIBZKFP_LIB_DIR}
    NO_DEFAULT_PATH
)

if(NOT LIBZKFP_LIBRARY)
    message(FATAL_ERROR "No se encontró la librería libzkfp en ${LIBZKFP_LIB_DIR}")
else()
    message(STATUS "Librería libzkfp encontrada: ${LIBZKFP_LIBRARY}")
endif()

# ============================================
# Crear ejecutable
# ============================================

# Agregue un origen al ejecutable de este proyecto.
add_executable(ThorStormbreaker 
        ThorStormbreaker.cpp
        ThorStormbreaker.h

        services/IFingerprintService.h
        services/implement/FingerprintServiceImpl.cpp
        services/implement/FingerprintServiceImpl.h
        services/implement/FingerEnrollServiceImpl.cpp

        config/SecureString.h
        config/Cipher.h
        config/ConfigManager.h
        config/ConfigEnvironment.h
        config/ConfigGenerator.h

        ui/WebViewUi.cpp
        ui/WebViewUI.h

        ui/WebBindServices/IFingerEnrollBind.h
        ui/WebBindServices/FingerEnrollBind.h
        ui/WebBindServices/FingerEnrollBind.cpp

        repository/Respository.h
        ui/WebViewDispatcher.h
        ui/WebViewDispatcher.h
        app/App.h
        app/App.cpp
        services/AccessBackgroundService.h
        services/implement/AccessBackgroundService.cpp
        services/implement/AccessBackgroundService.cpp
        services/FingerprintDeviceManager.h
        services/implement/FingerprintDeviceManager.cpp
)

# Establecer estándar C++ 20
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ThorStormbreaker PROPERTY CXX_STANDARD 20)
endif()

# Vincular la librería libzkfp (enlace estático, no requiere DLL)
target_link_libraries(ThorStormbreaker PRIVATE
        sqlite3
        ${LIBZKFP_LIBRARY}
        webview::core
        nlohmann_json::nlohmann_json
)

# Incluir directorios para el target
target_include_directories(ThorStormbreaker PRIVATE
        ${CMAKE_SOURCE_DIR}/app
        ${LIBZKFP_DIR}/include
        ${CMAKE_SOURCE_DIR}/ui
        ${CMAKE_SOURCE_DIR}/services
        ${CMAKE_SOURCE_DIR}/repository
)

# ============================================
# Copiar DLLs y archivos de datos (si existen)
# ============================================

# Buscar y copiar DLLs del SDK (pueden existir en versiones completas)
file(GLOB ZKFP_DLLS "${LIBZKFP_LIB_DIR}/*.dll")
if(ZKFP_DLLS)
    message(STATUS "Encontradas DLLs de libzkfp, se copiarán al directorio de salida")
    foreach(DLL_FILE ${ZKFP_DLLS})
        add_custom_command(TARGET ThorStormbreaker POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_FILE}"
            $<TARGET_FILE_DIR:ThorStormbreaker>
            COMMENT "Copiando ${DLL_FILE}..."
        )
    endforeach()
else()
    message(STATUS "No se encontraron DLLs adicionales (enlace estático)")
endif()

# Buscar y copiar archivos de datos (.dat, .lic)
file(GLOB ZKFP_DATA_FILES "${LIBZKFP_DIR}/*.dat" "${LIBZKFP_DIR}/*.lic")
if(ZKFP_DATA_FILES)
    message(STATUS "Encontrados archivos de datos, se copiarán al directorio de salida")
    foreach(DATA_FILE ${ZKFP_DATA_FILES})
        add_custom_command(TARGET ThorStormbreaker POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DATA_FILE}"
            $<TARGET_FILE_DIR:ThorStormbreaker>
            COMMENT "Copiando ${DATA_FILE}..."
        )
    endforeach()
else()
    message(STATUS "No se encontraron archivos de datos (.dat, .lic)")
endif()

# ============================================
# Webview Integration (opcional)
# ===========================================
include(FetchContent)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 0.12.0
)
FetchContent_MakeAvailable(webview)

# ============================================
# SQLite3 (amalgamation)
# ============================================

add_library(sqlite3 STATIC
        ${CMAKE_SOURCE_DIR}/external/sqlite3/sqlite3.c
)

target_include_directories(sqlite3 PUBLIC
        ${CMAKE_SOURCE_DIR}/external/sqlite3
)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(nlohmann_json)

# TODO: Agregue pruebas y destinos de instalación si es necesario.
