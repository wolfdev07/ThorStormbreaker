cmake_minimum_required(VERSION 3.16)

project("ThorStormbreaker" LANGUAGES C CXX RC)

# ============================================
# MSVC Hot Reload (si aplica)
# ============================================
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()

# ============================================
# Configuración general
# ============================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Icono Windows
set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_SOURCE_DIR}/resources/app.rc)

# ============================================
# Load .env (opcional)
# ============================================
include(LoadEnvCMake OPTIONAL)

# ============================================
# libzkfp
# ============================================
set(LIBZKFP_DIR "${CMAKE_SOURCE_DIR}/external/libzkfp")
include_directories(${LIBZKFP_DIR}/include)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LIBZKFP_LIB_DIR "${LIBZKFP_DIR}/x64lib")
    message(STATUS "Usando libzkfp x64")
else()
    set(LIBZKFP_LIB_DIR "${LIBZKFP_DIR}/x86lib")
    message(STATUS "Usando libzkfp x86")
endif()

find_library(LIBZKFP_LIBRARY
        NAMES libzkfp
        PATHS ${LIBZKFP_LIB_DIR}
        NO_DEFAULT_PATH
)

if (NOT LIBZKFP_LIBRARY)
    message(FATAL_ERROR "No se encontró libzkfp")
endif()

# ============================================
# FetchContent
# ============================================
include(FetchContent)

# ---- webview (0.12.0) ----
FetchContent_Declare(
        webview
        GIT_REPOSITORY https://github.com/webview/webview
        GIT_TAG 0.12.0
)
FetchContent_MakeAvailable(webview)

# ---- nlohmann_json ----
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# ============================================
# SQLite3 (amalgamation)
# ============================================
add_library(sqlite3 STATIC
        external/sqlite3/sqlite3.c
)
target_include_directories(sqlite3 PUBLIC external/sqlite3)

# ============================================
# Ejecutable
# ============================================
add_executable(ThorStormbreaker WIN32
        ThorStormbreaker.cpp
        ThorStormbreaker.h
        resources/app.rc

        app/App.h
        app/App.cpp

        ui/WebViewUI.h
        ui/WebViewUi.cpp
        ui/WebViewDispatcher.h

        ui/WebBindServices/IFingerEnrollBind.h
        ui/WebBindServices/FingerEnrollBind.h
        ui/WebBindServices/FingerEnrollBind.cpp

        services/IFingerprintService.h
        services/FingerprintDeviceManager.h
        services/implement/FingerprintDeviceManager.cpp
        services/AccessBackgroundService.h
        services/implement/AccessBackgroundService.cpp
        services/implement/FingerprintServiceImpl.cpp
        services/implement/FingerEnrollServiceImpl.cpp

        config/SecureString.h
        config/Cipher.h
        config/ConfigManager.h
        config/ConfigEnvironment.h
        config/ConfigGenerator.h

        repository/Respository.h
)

# ============================================
# Includes
# ============================================
target_include_directories(ThorStormbreaker PRIVATE
        ${webview_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/ui
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_SOURCE_DIR}/repository
        ${CMAKE_SOURCE_DIR}/services
        ${LIBZKFP_DIR}/include
)

# ============================================
# Linkeo
# ============================================
target_link_libraries(ThorStormbreaker PRIVATE
        webview::core
        nlohmann_json::nlohmann_json
        sqlite3
        ${LIBZKFP_LIBRARY}
        -mwindows
)

# ============================================
# Post-build: recursos
# ============================================
add_custom_command(TARGET ThorStormbreaker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:ThorStormbreaker>/ui
)

add_custom_command(TARGET ThorStormbreaker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/ui/banner.html
        $<TARGET_FILE_DIR:ThorStormbreaker>/ui/banner.html
        COMMENT "Copying banner.html"
)

file(GLOB ZKFP_DLLS "${LIBZKFP_LIB_DIR}/*.dll")
foreach(DLL_FILE ${ZKFP_DLLS})
    add_custom_command(TARGET ThorStormbreaker POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_FILE}"
            $<TARGET_FILE_DIR:ThorStormbreaker>
    )
endforeach()