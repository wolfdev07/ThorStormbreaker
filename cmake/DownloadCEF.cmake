# DownloadCEF.cmake - Descarga automática de Chromium Embedded Framework
# 
# Este módulo descarga y configura CEF automáticamente basado en la arquitectura del sistema

set(CEF_VERSION "143.0.14+gdd46a37+chromium-143.0.7499.193")
set(CEF_CHROMIUM_VERSION "143.0.7499.193")

# Determinar arquitectura y URL de descarga
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
    set(CEF_PLATFORM "windows64")
    set(CEF_DOWNLOAD_URL "https://cef-builds.spotifycdn.com/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}.tar.bz2")
    message(STATUS "CEF: Detectada arquitectura x64")
else()
    # 32-bit
    set(CEF_PLATFORM "windows32")
    set(CEF_DOWNLOAD_URL "https://cef-builds.spotifycdn.com/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}.tar.bz2")
    message(STATUS "CEF: Detectada arquitectura x86")
endif()

# Directorio donde se descargará CEF
set(CEF_ROOT "${CMAKE_SOURCE_DIR}/external/cef")
set(CEF_BINARY_DIR "${CEF_ROOT}/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}")
set(CEF_ARCHIVE "${CEF_ROOT}/cef_binary.tar.bz2")

# Función para descargar CEF si no existe
function(download_cef)
    if(NOT EXISTS "${CEF_BINARY_DIR}")
        message(STATUS "CEF no encontrado. Descargando CEF ${CEF_VERSION}...")
        message(STATUS "URL: ${CEF_DOWNLOAD_URL}")
        message(STATUS "Destino: ${CEF_ARCHIVE}")
        message(STATUS "")
        message(STATUS "NOTA: Esta descarga es de ~304 MB y puede tardar varios minutos")
        message(STATUS "      dependiendo de su conexión a internet.")
        message(STATUS "")
        
        # Crear directorio si no existe
        file(MAKE_DIRECTORY "${CEF_ROOT}")
        
        # Descargar archivo
        file(DOWNLOAD
            "${CEF_DOWNLOAD_URL}"
            "${CEF_ARCHIVE}"
            SHOW_PROGRESS
            STATUS DOWNLOAD_STATUS
            TIMEOUT 3600  # 1 hora de timeout
        )
        
        # Verificar descarga
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
        
        if(NOT STATUS_CODE EQUAL 0)
            message(FATAL_ERROR "Error al descargar CEF: ${ERROR_MESSAGE}")
        endif()
        
        message(STATUS "Descarga completada. Extrayendo archivos...")
        
        # Extraer archivo
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xjf "${CEF_ARCHIVE}"
            WORKING_DIRECTORY "${CEF_ROOT}"
            RESULT_VARIABLE EXTRACT_RESULT
        )
        
        if(NOT EXTRACT_RESULT EQUAL 0)
            message(FATAL_ERROR "Error al extraer CEF")
        endif()
        
        # Eliminar archivo comprimido para ahorrar espacio
        file(REMOVE "${CEF_ARCHIVE}")
        
        message(STATUS "CEF extraído exitosamente en: ${CEF_BINARY_DIR}")
    else()
        message(STATUS "CEF ya está descargado: ${CEF_BINARY_DIR}")
    endif()
endfunction()

# Función para configurar CEF en el proyecto
function(setup_cef TARGET_NAME)
    # Asegurar que CEF está descargado
    download_cef()
    
    # Verificar que el directorio existe
    if(NOT EXISTS "${CEF_BINARY_DIR}")
        message(FATAL_ERROR "CEF no encontrado en: ${CEF_BINARY_DIR}")
    endif()
    
    # Buscar directorios de include y librerías
    set(CEF_INCLUDE_PATH "${CEF_BINARY_DIR}")
    if(EXISTS "${CEF_BINARY_DIR}/include")
        set(CEF_INCLUDE_PATH "${CEF_BINARY_DIR}/include")
    endif()
    
    message(STATUS "CEF encontrado:")
    message(STATUS "  Versión: ${CEF_VERSION}")
    message(STATUS "  Directorio: ${CEF_BINARY_DIR}")
    message(STATUS "  Include: ${CEF_INCLUDE_PATH}")
    
    # Agregar directorios de include
    target_include_directories(${TARGET_NAME} PRIVATE
        ${CEF_INCLUDE_PATH}
    )
    
    # Buscar y vincular librerías CEF
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CEF_LIB_DIR "${CEF_BINARY_DIR}/Debug")
    else()
        set(CEF_LIB_DIR "${CEF_BINARY_DIR}/Release")
    endif()
    
    # Librerías principales
    if(EXISTS "${CEF_LIB_DIR}/libcef.lib")
        target_link_libraries(${TARGET_NAME} PRIVATE "${CEF_LIB_DIR}/libcef.lib")
    endif()
    
    if(EXISTS "${CEF_LIB_DIR}/libcef_dll_wrapper.lib")
        target_link_libraries(${TARGET_NAME} PRIVATE "${CEF_LIB_DIR}/libcef_dll_wrapper.lib")
    endif()
    
    # Librerías estándar de Windows requeridas por CEF
    target_link_libraries(${TARGET_NAME} PRIVATE
        comctl32.lib
        rpcrt4.lib
        shlwapi.lib
        ws2_32.lib
    )
    
    # Configurar opciones de compilación
    if(MSVC)
        # Opciones específicas para Visual Studio
        target_compile_options(${TARGET_NAME} PRIVATE
            /MP  # Compilación multiproceso
            /W3  # Nivel de advertencias
        )
        
        # Configurar subsistema Windows
        set_target_properties(${TARGET_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
    
    # Copiar binarios de CEF al directorio de salida
    if(EXISTS "${CEF_LIB_DIR}")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_LIB_DIR}"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            COMMENT "Copiando binarios de CEF..."
        )
    endif()
    
    # Copiar recursos de CEF
    if(EXISTS "${CEF_BINARY_DIR}/Resources")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Resources"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            COMMENT "Copiando recursos de CEF..."
        )
    endif()
    
    message(STATUS "CEF configurado para ${TARGET_NAME}")
endfunction()

# Información adicional
message(STATUS "")
message(STATUS "========================================")
message(STATUS "Chromium Embedded Framework (CEF)")
message(STATUS "========================================")
message(STATUS "Versión: ${CEF_VERSION}")
message(STATUS "Chromium: ${CEF_CHROMIUM_VERSION}")
message(STATUS "Plataforma: ${CEF_PLATFORM}")
message(STATUS "")
message(STATUS "Características soportadas:")
message(STATUS "  JavaScript ES2023+")
message(STATUS "  CSS3 + Flexbox + Grid")
message(STATUS "  React / Vue / Angular")
message(STATUS "  Tailwind CSS")
message(STATUS "  IndexedDB")
message(STATUS "  WebSockets")
message(STATUS "  WebGL / Canvas")
message(STATUS "  Fetch API")
message(STATUS "  Service Workers")
message(STATUS "========================================")
message(STATUS "")
