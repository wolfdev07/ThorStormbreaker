#pragma once

#include "ConfigManager.h"
#include <iostream>
#include <map>

/**
 * @brief Utilidad para generar archivo config.dat cifrado
 * 
 * Esta clase ayuda a crear el archivo de configuración inicial
 * de forma interactiva o programática.
 */
class ConfigGenerator {
public:
    /**
     * @brief Genera un archivo config.dat con valores por defecto
     * @param password Contraseña para cifrar el archivo
     * @param outputPath Ruta del archivo de salida
     */
    static void generateDefault(const std::string& password, 
                                const std::string& outputPath = "config.dat") {
        
        std::map<std::string, std::string> defaultConfig = {
            // Base de datos
            {"DB_PATH", "fingerprints.db"},
            {"DB_PASSWORD", ""},
            
            // Configuración del lector de huellas
            {"FP_DEVICE_INDEX", "0"},
            {"FP_TIMEOUT", "5000"},
            {"FP_MATCH_THRESHOLD", "70"},
            
            // Seguridad
            {"SESSION_TIMEOUT", "900"},  // 15 minutos
            {"MAX_LOGIN_ATTEMPTS", "3"},
            {"LOCK_MEMORY", "true"},
            
            // Logging
            {"LOG_LEVEL", "INFO"},
            {"LOG_PATH", "logs/app.log"},
            {"LOG_MAX_SIZE", "10485760"},  // 10 MB
            
            // Aplicación
            {"APP_NAME", "ThorStormbreaker"},
            {"APP_VERSION", "1.0.0"},
            {"DEBUG_MODE", "false"}
        };

        ConfigManager config(outputPath);
        config.create(password, defaultConfig);
        
        std::cout << "✓ Archivo de configuración creado: " << outputPath << std::endl;
        std::cout << "✓ Configuración cifrada con AES-256" << std::endl;
        std::cout << "⚠️  Guarda la contraseña en un lugar seguro!" << std::endl;
    }

    /**
     * @brief Genera config.dat de forma interactiva
     */
    static void generateInteractive() {
        std::cout << "=== Generador de Configuración ThorStormbreaker ===" << std::endl;
        std::cout << std::endl;

        // Solicitar contraseña
        std::cout << "Ingrese una contraseña maestra para cifrar el archivo:" << std::endl;
        std::cout << "(Mínimo 8 caracteres, recomendado: 16+)" << std::endl;
        std::cout << "Contraseña: ";
        
        std::string password;
        std::getline(std::cin, password);

        if (password.length() < 8) {
            std::cerr << "❌ La contraseña debe tener al menos 8 caracteres" << std::endl;
            return;
        }

        std::cout << std::endl;
        std::cout << "Confirme la contraseña: ";
        std::string passwordConfirm;
        std::getline(std::cin, passwordConfirm);

        if (password != passwordConfirm) {
            std::cerr << "❌ Las contraseñas no coinciden" << std::endl;
            SecureZeroMemory((void*)password.data(), password.size());
            SecureZeroMemory((void*)passwordConfirm.data(), passwordConfirm.size());
            return;
        }

        // Configuración personalizada
        std::map<std::string, std::string> config;

        auto askValue = [](const std::string& prompt, const std::string& defaultValue) {
            std::cout << prompt << " [" << defaultValue << "]: ";
            std::string value;
            std::getline(std::cin, value);
            return value.empty() ? defaultValue : value;
        };

        std::cout << std::endl;
        std::cout << "=== Configuración de Base de Datos ===" << std::endl;
        config["DB_PATH"] = askValue("Ruta de la base de datos", "fingerprints.db");
        config["DB_PASSWORD"] = askValue("Contraseña de la BD (dejar vacío para ninguna)", "");

        std::cout << std::endl;
        std::cout << "=== Configuración del Lector de Huellas ===" << std::endl;
        config["FP_DEVICE_INDEX"] = askValue("Índice del dispositivo", "0");
        config["FP_TIMEOUT"] = askValue("Timeout de captura (ms)", "5000");
        config["FP_MATCH_THRESHOLD"] = askValue("Umbral de coincidencia (0-100)", "70");

        std::cout << std::endl;
        std::cout << "=== Configuración de Seguridad ===" << std::endl;
        config["SESSION_TIMEOUT"] = askValue("Timeout de sesión (segundos)", "900");
        config["MAX_LOGIN_ATTEMPTS"] = askValue("Intentos máximos de login", "3");
        config["LOCK_MEMORY"] = askValue("Bloquear memoria en RAM (true/false)", "true");

        std::cout << std::endl;
        std::cout << "=== Configuración de Logging ===" << std::endl;
        config["LOG_LEVEL"] = askValue("Nivel de log (DEBUG/INFO/WARN/ERROR)", "INFO");
        config["LOG_PATH"] = askValue("Ruta del archivo de log", "logs/app.log");

        std::cout << std::endl;
        std::cout << "=== Información de la Aplicación ===" << std::endl;
        config["APP_NAME"] = "ThorStormbreaker";
        config["APP_VERSION"] = "1.0.0";
        config["DEBUG_MODE"] = askValue("Modo debug (true/false)", "false");

        // Crear archivo
        std::cout << std::endl;
        std::cout << "Generando archivo config.dat..." << std::endl;

        try {
            ConfigManager configManager("config.dat");
            configManager.create(password, config);

            std::cout << std::endl;
            std::cout << "✓ ¡Configuración creada exitosamente!" << std::endl;
            std::cout << "✓ Archivo: config.dat" << std::endl;
            std::cout << "✓ Cifrado: AES-256" << std::endl;
            std::cout << std::endl;
            std::cout << "⚠️  IMPORTANTE:" << std::endl;
            std::cout << "   - Guarda la contraseña maestra en un lugar seguro" << std::endl;
            std::cout << "   - No compartas el archivo config.dat sin cifrarlo" << std::endl;
            std::cout << "   - Haz backups regulares del archivo" << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "❌ Error al crear configuración: " << e.what() << std::endl;
        }

        // Limpiar contraseñas de memoria
        SecureZeroMemory((void*)password.data(), password.size());
        SecureZeroMemory((void*)passwordConfirm.data(), passwordConfirm.size());
    }

    /**
     * @brief Muestra el contenido de un archivo config.dat
     * @param password Contraseña para descifrar
     * @param configPath Ruta del archivo
     */
    static void displayConfig(const std::string& password, 
                             const std::string& configPath = "config.dat") {
        try {
            ConfigManager config(configPath);
            config.load(password);

            std::cout << "=== Contenido de " << configPath << " ===" << std::endl;
            std::cout << std::endl;

            auto keys = config.getKeys();
            for (const auto& key : keys) {
                std::string value = config.get(key);
                // Ocultar valores sensibles
                if (key.find("PASSWORD") != std::string::npos || 
                    key.find("SECRET") != std::string::npos ||
                    key.find("KEY") != std::string::npos) {
                    value = "********";
                }
                std::cout << key << " = " << value << std::endl;
            }

            std::cout << std::endl;
            std::cout << "Total de claves: " << keys.size() << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "❌ Error al leer configuración: " << e.what() << std::endl;
        }
    }

    /**
     * @brief Genera una contraseña maestra aleatoria
     */
    static void generateMasterPassword() {
        auto password = ConfigManager::generateMasterPassword();
        
        std::cout << "=== Contraseña Maestra Generada ===" << std::endl;
        std::cout << std::endl;
        std::cout << password.getString() << std::endl;
        std::cout << std::endl;
        std::cout << "⚠️  GUARDA ESTA CONTRASEÑA EN UN LUGAR SEGURO!" << std::endl;
        std::cout << "   No podrás recuperarla después." << std::endl;
        std::cout << std::endl;
        std::cout << "Longitud: " << password.size() << " caracteres" << std::endl;
        std::cout << "Entropía: ~512 bits" << std::endl;
    }
};
