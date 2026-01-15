# LoadEnvCMake.cmake - Carga variables de entorno desde .env en tiempo de configuración

# Función para cargar archivo .env
function(load_env_file ENV_FILE)
    if(NOT EXISTS "${ENV_FILE}")
        message(STATUS "Archivo .env no encontrado: ${ENV_FILE}")
        message(STATUS "  (Opcional) Cree ${ENV_FILE} para configurar variables de entorno")
        return()
    endif()
    
    message(STATUS "")
    message(STATUS "Cargando variables de entorno desde: ${ENV_FILE}")
    message(STATUS "??????????????????????????????????????????????????")
    
    file(STRINGS "${ENV_FILE}" ENV_LINES)
    
    set(VAR_COUNT 0)
    foreach(LINE ${ENV_LINES})
        # Eliminar espacios en blanco
        string(STRIP "${LINE}" LINE)
        
        # Ignorar líneas vacías y comentarios
        if(LINE STREQUAL "" OR LINE MATCHES "^#")
            continue()
        endif()
        
        # Parsear VARIABLE=valor
        if(LINE MATCHES "^([A-Za-z_][A-Za-z0-9_]*)=(.*)$")
            set(VAR_NAME "${CMAKE_MATCH_1}")
            set(VAR_VALUE "${CMAKE_MATCH_2}")
            
            # Remover comillas si existen
            string(REGEX REPLACE "^[\"']|[\"']$" "" VAR_VALUE "${VAR_VALUE}")
            
            # Establecer variable de entorno para el proceso CMake
            set(ENV{${VAR_NAME}} "${VAR_VALUE}")
            
            # También establecer como variable de cache (opcional)
            set(${VAR_NAME} "${VAR_VALUE}" CACHE STRING "Variable de entorno desde .env" FORCE)
            
            # Mostrar mensaje (ocultar valores sensibles)
            if(VAR_NAME MATCHES "(PASSWORD|SECRET|KEY|TOKEN)")
                message(STATUS "  ? ${VAR_NAME} = ********")
            else()
                message(STATUS "  ? ${VAR_NAME} = ${VAR_VALUE}")
            endif()
            
            math(EXPR VAR_COUNT "${VAR_COUNT} + 1")
        else()
            message(STATUS "  ? Línea ignorada (formato inválido): ${LINE}")
        endif()
    endforeach()
    
    message(STATUS "??????????????????????????????????????????????????")
    message(STATUS "Variables de entorno cargadas: ${VAR_COUNT}")
    message(STATUS "")
endfunction()

# Cargar automáticamente si existe .env en el directorio raíz
set(ENV_FILE "${CMAKE_SOURCE_DIR}/.env")
if(EXISTS "${ENV_FILE}")
    load_env_file("${ENV_FILE}")
else()
    message(STATUS "")
    message(STATUS "Archivo .env no encontrado (opcional)")
    message(STATUS "  Para usar variables de entorno, copie .env.example a .env")
    message(STATUS "")
endif()
