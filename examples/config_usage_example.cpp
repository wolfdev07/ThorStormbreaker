/**
 * @file example_config_usage.cpp
 * @brief Ejemplo completo de uso del sistema de configuración segura
 * 
 * Este archivo muestra cómo migrar desde variables de entorno (.env)
 * al sistema de configuración cifrada (config.dat)
 */

#include "config/ConfigManager.h"
#include "config/ConfigEnvironment.h"
#include "config/ConfigGenerator.h"
#include "config/SecureString.h"
#include "services/Environment.h"
#include <iostream>
#include <filesystem>

// ============================================================
// PASO 1: Crear el archivo config.dat (solo primera vez)
// ============================================================

void createConfigFile() {
    std::cout << "=== Creando archivo config.dat ===" << std::endl;
    std::cout << std::endl;

    // Opción A: Crear con valores por defecto
    std::cout << "Opción A: Valores por defecto" << std::endl;
    ConfigGenerator::generateDefault("MiPasswordSegura123!");
    std::cout << std::endl;

    // Opción B: Crear con valores personalizados
    std::cout << "Opción B: Valores personalizados" << std::endl;
    std::map<std::string, std::string> customConfig = {
        {"DB_PATH", "fingerprints.db"},
        {"DB_PASSWORD", "SecretDBPassword"},
        {"FP_DEVICE_INDEX", "0"},
        {"FP_TIMEOUT", "5000"},
        {"FP_MATCH_THRESHOLD", "75"},
        {"API_KEY", "sk_live_abc123xyz789"},
        {"LOG_LEVEL", "INFO"}
    };

    ConfigManager config("config.dat");
    config.create("MiPasswordSegura123!", customConfig);
    std::cout << "✓ Configuración creada" << std::endl;
    std::cout << std::endl;

    // Opción C: Generador interactivo
    // ConfigGenerator::generateInteractive();

    // Opción D: Generar contraseña maestra aleatoria
    std::cout << "Opción D: Contraseña maestra aleatoria" << std::endl;
    ConfigGenerator::generateMasterPassword();
}

// ============================================================
// PASO 2: Usar ConfigManager directamente
// ============================================================

void useConfigManagerDirect() {
    std::cout << "\n=== Usando ConfigManager directamente ===" << std::endl;
    std::cout << std::endl;

    try {
        // Cargar configuración
        ConfigManager config("config.dat");
        if (!config.load("MiPasswordSegura123!")) {
            std::cerr << "❌ Error al cargar configuración" << std::endl;
            return;
        }

        std::cout << "✓ Configuración cargada" << std::endl;
        std::cout << std::endl;

        // Leer valores
        std::string dbPath = config.get("DB_PATH", "default.db");
        int timeout = config.getInt("FP_TIMEOUT", 5000);
        bool debugMode = config.getBool("DEBUG_MODE", false);
        double threshold = config.getDouble("FP_MATCH_THRESHOLD", 70.0);

        std::cout << "DB Path: " << dbPath << std::endl;
        std::cout << "Timeout: " << timeout << " ms" << std::endl;
        std::cout << "Debug: " << (debugMode ? "ON" : "OFF") << std::endl;
        std::cout << "Threshold: " << threshold << std::endl;
        std::cout << std::endl;

        // Valor requerido (lanza excepción si no existe)
        try {
            std::string apiKey = config.require("API_KEY");
            std::cout << "API Key encontrada (ocultada por seguridad)" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "API Key no encontrada: " << e.what() << std::endl;
        }
        std::cout << std::endl;

        // Modificar y guardar
        config.set("LAST_ACCESS", "2026-01-14");
        config.set("ACCESS_COUNT", "42");
        config.save("MiPasswordSegura123!");
        std::cout << "✓ Cambios guardados" << std::endl;

        // Listar todas las claves
        std::cout << "\nClaves disponibles:" << std::endl;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            std::cout << "  - " << key << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
    }
}

// ============================================================
// PASO 3: Usar ConfigEnvironment directamente (RECOMENDADO)
// ============================================================

void useConfigEnvironmentDirect() {
    std::cout << "\n=== Usando ConfigEnvironment directamente (RECOMENDADO) ===" << std::endl;
    std::cout << std::endl;

    // IMPORTANTE: Inicializar primero
    if (!ConfigEnvironment::initialize("MiPasswordSegura123!")) {
        std::cerr << "❌ Error al inicializar ConfigEnvironment" << std::endl;
        return;
    }

    std::cout << "✓ ConfigEnvironment inicializado" << std::endl;
    std::cout << std::endl;

    // Usar la API
    std::string dbPath = ConfigEnvironment::get("DB_PATH", "default.db");
    int timeout = ConfigEnvironment::getInt("FP_TIMEOUT", 5000);
    bool debugMode = ConfigEnvironment::getBool("DEBUG_MODE", false);

    std::cout << "DB Path: " << dbPath << std::endl;
    std::cout << "Timeout: " << timeout << " ms" << std::endl;
    std::cout << "Debug: " << (debugMode ? "ON" : "OFF") << std::endl;
    std::cout << std::endl;

    // Verificar existencia
    if (ConfigEnvironment::has("API_KEY")) {
        std::cout << "✓ API_KEY configurada" << std::endl;
    }

    // Obtener con optional
    auto optionalValue = ConfigEnvironment::getOptional("SOME_KEY");
    if (optionalValue) {
        std::cout << "SOME_KEY = " << *optionalValue << std::endl;
    } else {
        std::cout << "SOME_KEY no configurada" << std::endl;
    }
    std::cout << std::endl;

    // Modificar (en memoria)
    ConfigEnvironment::set("SESSION_ID", "abc123");
    
    // Guardar cambios
    if (ConfigEnvironment::save("MiPasswordSegura123!")) {
        std::cout << "✓ Configuración guardada" << std::endl;
    }

    // Información de diagnóstico
    std::cout << "\nNúmero de claves: " << ConfigEnvironment::getKeyCount() << std::endl;
    std::cout << "Archivo config: " << ConfigEnvironment::getConfigPath() << std::endl;

    // Imprimir configuración (ocultando secretos)
    std::cout << std::endl;
    ConfigEnvironment::printConfig(true);

    // Limpiar al finalizar
    ConfigEnvironment::shutdown();
    std::cout << "\n✓ ConfigEnvironment cerrado y memoria limpiada" << std::endl;
}

// ============================================================
// PASO 4: Usar Environment (wrapper de compatibilidad)
// ============================================================

void useEnvironmentWrapper() {
    std::cout << "\n=== Usando Environment (wrapper de compatibilidad) ===" << std::endl;
    std::cout << std::endl;

    // IMPORTANTE: Inicializar primero
    if (!Environment::initialize("MiPasswordSegura123!")) {
        std::cerr << "❌ Error al inicializar Environment" << std::endl;
        return;
    }

    std::cout << "✓ Environment inicializado" << std::endl;
    std::cout << std::endl;

    // Usar como antes (API compatible)
    std::string dbPath = Environment::get("DB_PATH", "default.db");
    int timeout = Environment::getInt("FP_TIMEOUT", 5000);
    bool debugMode = Environment::getBool("DEBUG_MODE", false);

    std::cout << "DB Path: " << dbPath << std::endl;
    std::cout << "Timeout: " << timeout << " ms" << std::endl;
    std::cout << "Debug: " << (debugMode ? "ON" : "OFF") << std::endl;
    std::cout << std::endl;

    // Verificar existencia
    if (Environment::has("API_KEY")) {
        std::cout << "✓ API_KEY configurada" << std::endl;
    }

    // Obtener con optional
    auto optionalValue = Environment::getOptional("SOME_KEY");
    if (optionalValue) {
        std::cout << "SOME_KEY = " << *optionalValue << std::endl;
    } else {
        std::cout << "SOME_KEY no configurada" << std::endl;
    }
    std::cout << std::endl;

    // Modificar (en memoria)
    Environment::set("SESSION_ID", "abc123");
    
    // Guardar cambios
    if (Environment::save("MiPasswordSegura123!")) {
        std::cout << "✓ Configuración guardada" << std::endl;
    }

    // Limpiar al finalizar
    Environment::shutdown();
    std::cout << "✓ Environment cerrado y memoria limpiada" << std::endl;
    std::cout << "\nNOTA: Environment es solo un wrapper. Se recomienda usar ConfigEnvironment directamente." << std::endl;
}

// ============================================================
// PASO 5: Usar SecureString para datos sensibles
// ============================================================

void useSecureString() {
    std::cout << "\n=== Usando SecureString ===" << std::endl;
    std::cout << std::endl;

    {
        // Crear SecureString
        SecureString password("MiContraseñaSecreto123!");
        
        // Bloquear en RAM (evita swap a disco)
        if (password.lockMemory()) {
            std::cout << "✓ Memoria bloqueada en RAM" << std::endl;
        }

        // Usar el password
        std::cout << "Longitud del password: " << password.size() << " caracteres" << std::endl;
        
        // La memoria se limpia automáticamente al salir del scope
        std::cout << "Password se limpiará automáticamente..." << std::endl;
    }

    std::cout << "✓ Memoria limpiada con SecureZeroMemory" << std::endl;
    std::cout << std::endl;

    // Generar password aleatorio
    auto randomPassword = ConfigManager::generateMasterPassword();
    std::cout << "Password aleatorio generado:" << std::endl;
    std::cout << randomPassword.getString() << std::endl;
    std::cout << "Longitud: " << randomPassword.size() << " caracteres" << std::endl;
}

// ============================================================
// PASO 6: Migración desde .env
// ============================================================

void migrationExample() {
    std::cout << "\n=== Migración desde .env a config.dat ===" << std::endl;
    std::cout << std::endl;

    // ANTES (con .env):
    // ------------------------------
    // DB_PATH=fingerprints.db
    // DB_PASSWORD=secret123
    // API_KEY=sk_live_abc123
    //
    // Código:
    // std::string dbPath = Environment::get("DB_PATH");
    // std::string apiKey = Environment::get("API_KEY");

    std::cout << "ANTES (código antiguo con .env):" << std::endl;
    std::cout << "  std::string dbPath = Environment::get(\"DB_PATH\");" << std::endl;
    std::cout << "  // Lee desde variables de entorno" << std::endl;
    std::cout << std::endl;

    // AHORA (con config.dat):
    // ------------------------------
    std::cout << "AHORA (código nuevo con config.dat):" << std::endl;
    std::cout << std::endl;

    // 1. Crear config.dat con los mismos valores
    std::map<std::string, std::string> envValues = {
        {"DB_PATH", "fingerprints.db"},
        {"DB_PASSWORD", "secret123"},
        {"API_KEY", "sk_live_abc123"}
    };

    ConfigManager config("migrated_config.dat");
    config.create("NuevaPasswordSegura123!", envValues);
    std::cout << "✓ config.dat creado desde valores .env" << std::endl;

    // 2. Inicializar Environment
    if (Environment::initialize("NuevaPasswordSegura123!", "migrated_config.dat")) {
        std::cout << "✓ Environment inicializado con config.dat" << std::endl;
    }

    // 3. Usar el MISMO código que antes (API compatible)
    std::string dbPath = Environment::get("DB_PATH");
    std::string apiKey = Environment::get("API_KEY");

    std::cout << "DB Path: " << dbPath << std::endl;
    std::cout << "API Key: " << apiKey.substr(0, 10) << "... (oculto)" << std::endl;
    std::cout << std::endl;

    std::cout << "✓ Migración completada!" << std::endl;
    std::cout << "  - config.dat cifrado con AES-256" << std::endl;
    std::cout << "  - Valores descifrados en RAM" << std::endl;
    std::cout << "  - Memoria limpiada automáticamente" << std::endl;
    std::cout << "  - API compatible con código existente" << std::endl;

    Environment::shutdown();
}

// ============================================================
// PASO 7: Ejemplo completo de aplicación
// ============================================================

void fullApplicationExample() {
    std::cout << "\n=== Ejemplo de Aplicación Completa ===" << std::endl;
    std::cout << std::endl;

    try {
        // 1. Inicializar configuración usando ConfigEnvironment
        std::cout << "[1] Inicializando configuración..." << std::endl;
        if (!ConfigEnvironment::initialize("MiPasswordSegura123!")) {
            throw std::runtime_error("No se pudo cargar config.dat");
        }

        // 2. Leer configuración de BD
        std::cout << "[2] Configurando base de datos..." << std::endl;
        std::string dbPath = ConfigEnvironment::require("DB_PATH");
        std::string dbPassword = ConfigEnvironment::get("DB_PASSWORD", "");
        
        std::cout << "    Base de datos: " << dbPath << std::endl;
        std::cout << "    Con contraseña: " << (!dbPassword.empty() ? "Sí" : "No") << std::endl;

        // 3. Leer configuración del lector de huellas
        std::cout << "[3] Configurando lector de huellas..." << std::endl;
        int deviceIndex = ConfigEnvironment::getInt("FP_DEVICE_INDEX", 0);
        int timeout = ConfigEnvironment::getInt("FP_TIMEOUT", 5000);
        int threshold = ConfigEnvironment::getInt("FP_MATCH_THRESHOLD", 70);

        std::cout << "    Dispositivo: " << deviceIndex << std::endl;
        std::cout << "    Timeout: " << timeout << " ms" << std::endl;
        std::cout << "    Umbral: " << threshold << std::endl;

        // 4. Configuración de logging
        std::cout << "[4] Configurando logging..." << std::endl;
        std::string logLevel = ConfigEnvironment::get("LOG_LEVEL", "INFO");
        std::string logPath = ConfigEnvironment::get("LOG_PATH", "app.log");

        std::cout << "    Nivel: " << logLevel << std::endl;
        std::cout << "    Archivo: " << logPath << std::endl;

        // 5. Verificar modo debug
        std::cout << "[5] Configuración de desarrollo..." << std::endl;
        bool debugMode = ConfigEnvironment::getBool("DEBUG_MODE", false);
        std::cout << "    Modo debug: " << (debugMode ? "ACTIVADO" : "DESACTIVADO") << std::endl;

        std::cout << std::endl;
        std::cout << "✓ Aplicación configurada correctamente" << std::endl;
        std::cout << "✓ Todos los secretos descifrados en RAM" << std::endl;

        // 6. Limpiar al finalizar
        ConfigEnvironment::shutdown();
        std::cout << "✓ Memoria limpiada de forma segura" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
    }
}

// ============================================================
// MAIN
// ============================================================

int main() {
    std::cout << "╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   Ejemplo de Sistema de Configuración Segura        ║" << std::endl;
    std::cout << "║   ThorStormbreaker - Migración de .env a config.dat ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    int opcion = 0;
    
    do {
        std::cout << "\n=== MENÚ ===" << std::endl;
        std::cout << "1. Crear archivo config.dat" << std::endl;
        std::cout << "2. Usar ConfigManager directamente" << std::endl;
        std::cout << "3. Usar ConfigEnvironment (RECOMENDADO)" << std::endl;
        std::cout << "4. Usar Environment (wrapper de compatibilidad)" << std::endl;
        std::cout << "5. Ejemplo de SecureString" << std::endl;
        std::cout << "6. Ejemplo de migración desde .env" << std::endl;
        std::cout << "7. Ejemplo de aplicación completa" << std::endl;
        std::cout << "0. Salir" << std::endl;
        std::cout << std::endl;
        std::cout << "Seleccione opción: ";
        
        std::cin >> opcion;
        std::cin.ignore();

        switch (opcion) {
            case 1:
                createConfigFile();
                break;
            case 2:
                useConfigManagerDirect();
                break;
            case 3:
                useConfigEnvironmentDirect();
                break;
            case 4:
                useEnvironmentWrapper();
                break;
            case 5:
                useSecureString();
                break;
            case 6:
                migrationExample();
                break;
            case 7:
                fullApplicationExample();
                break;
            case 0:
                std::cout << "\n¡Hasta luego!" << std::endl;
                break;
            default:
                std::cout << "Opción inválida" << std::endl;
        }

    } while (opcion != 0);

    return 0;
}
