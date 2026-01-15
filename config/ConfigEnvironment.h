#pragma once

#include "ConfigManager.h"
#include <string>
#include <optional>
#include <memory>
#include <stdexcept>
#include <vector>

/**
 * @brief Gestor de variables de entorno desde configuración cifrada
 * 
 * Proporciona acceso tipo singleton a la configuración de la aplicación
 * desde un archivo config.dat cifrado con AES-256.
 * 
 * Los valores se descifran en RAM y se limpian automáticamente de memoria
 * usando SecureString y SecureZeroMemory.
 * 
 * Uso:
 * @code
 * // Inicializar una vez al inicio
 * ConfigEnvironment::initialize("MasterPassword");
 * 
 * // Leer valores en cualquier parte de la app
 * std::string dbPath = ConfigEnvironment::get("DB_PATH");
 * int timeout = ConfigEnvironment::getInt("TIMEOUT", 5000);
 * 
 * // Limpiar al finalizar
 * ConfigEnvironment::shutdown();
 * @endcode
 */
class ConfigEnvironment {
private:
    static inline std::unique_ptr<ConfigManager> config_ = nullptr;
    static inline bool initialized_ = false;
    static inline std::string configPath_ = "config.dat";

    /**
     * @brief Verifica que el sistema esté inicializado
     * @throws std::runtime_error si no está inicializado
     */
    static void ensureInitialized() {
        if (!initialized_ || !config_) {
            throw std::runtime_error(
                "ConfigEnvironment no inicializado. "
                "Llama a ConfigEnvironment::initialize(password) primero."
            );
        }
    }

    // Constructor privado (clase estática)
    ConfigEnvironment() = delete;
    ~ConfigEnvironment() = delete;
    ConfigEnvironment(const ConfigEnvironment&) = delete;
    ConfigEnvironment& operator=(const ConfigEnvironment&) = delete;

public:
    /**
     * @brief Inicializa el sistema de configuración
     * @param password Contraseña maestra para descifrar config.dat
     * @param configPath Ruta al archivo config.dat (opcional)
     * @return true si se inicializó correctamente
     * 
     * @note DEBE llamarse antes de usar cualquier otro método
     * 
     * Ejemplo:
     * @code
     * if (!ConfigEnvironment::initialize("MiPasswordSegura")) {
     *     std::cerr << "Error al cargar configuración" << std::endl;
     *     return -1;
     * }
     * @endcode
     */
    static bool initialize(const std::string& password, 
                          const std::string& configPath = "config.dat") {
        try {
            configPath_ = configPath;
            config_ = std::make_unique<ConfigManager>(configPath);
            
            if (!config_->load(password)) {
                config_.reset();
                initialized_ = false;
                return false;
            }
            
            initialized_ = true;
            return true;
            
        } catch (const std::exception&) {
            config_.reset();
            initialized_ = false;
            return false;
        }
    }

    /**
     * @brief Verifica si el sistema está inicializado
     * @return true si está inicializado y listo para usar
     */
    static bool isInitialized() {
        return initialized_ && config_ && config_->isLoaded();
    }

    /**
     * @brief Limpia y cierra el sistema de configuración
     * 
     * Limpia toda la configuración de memoria de forma segura
     * usando SecureZeroMemory.
     */
    static void shutdown() {
        if (config_) {
            config_->clear();
            config_.reset();
        }
        initialized_ = false;
    }

    /**
     * @brief Recarga la configuración desde el archivo
     * @param password Contraseña maestra
     * @return true si se recargó correctamente
     */
    static bool reload(const std::string& password) {
        if (!config_) {
            return initialize(password, configPath_);
        }
        
        try {
            config_->clear();
            return config_->load(password);
        } catch (...) {
            return false;
        }
    }

    // ============================================================
    // Métodos de lectura de configuración
    // ============================================================

    /**
     * @brief Obtiene una variable de configuración como string
     * @param key Nombre de la clave
     * @param defaultValue Valor por defecto si no existe
     * @return Valor de la configuración o defaultValue
     */
    static std::string get(const std::string& key, const std::string& defaultValue = "") {
        ensureInitialized();
        return config_->get(key, defaultValue);
    }

    /**
     * @brief Obtiene una variable de configuración como int
     * @param key Nombre de la clave
     * @param defaultValue Valor por defecto si no existe o no es válido
     * @return Valor de la configuración o defaultValue
     */
    static int getInt(const std::string& key, int defaultValue = 0) {
        ensureInitialized();
        return config_->getInt(key, defaultValue);
    }

    /**
     * @brief Obtiene una variable de configuración como bool
     * @param key Nombre de la clave
     * @param defaultValue Valor por defecto si no existe
     * @return true si el valor es "true", "1", "yes", "on" (case-insensitive)
     */
    static bool getBool(const std::string& key, bool defaultValue = false) {
        ensureInitialized();
        return config_->getBool(key, defaultValue);
    }

    /**
     * @brief Obtiene una variable de configuración como double
     * @param key Nombre de la clave
     * @param defaultValue Valor por defecto si no existe o no es válido
     * @return Valor de la configuración o defaultValue
     */
    static double getDouble(const std::string& key, double defaultValue = 0.0) {
        ensureInitialized();
        return config_->getDouble(key, defaultValue);
    }

    /**
     * @brief Verifica si existe una clave de configuración
     * @param key Nombre de la clave
     * @return true si existe
     */
    static bool has(const std::string& key) {
        ensureInitialized();
        return config_->has(key);
    }

    /**
     * @brief Obtiene una variable de configuración como optional
     * @param key Nombre de la clave
     * @return std::optional con el valor o std::nullopt si no existe
     */
    static std::optional<std::string> getOptional(const std::string& key) {
        ensureInitialized();
        if (config_->has(key)) {
            return config_->get(key);
        }
        return std::nullopt;
    }

    /**
     * @brief Obtiene una variable requerida (lanza excepción si no existe)
     * @param key Nombre de la clave
     * @return Valor de la configuración
     * @throws std::runtime_error si la clave no existe
     */
    static std::string require(const std::string& key) {
        ensureInitialized();
        return config_->require(key);
    }

    // ============================================================
    // Métodos de escritura de configuración
    // ============================================================

    /**
     * @brief Establece una variable de configuración (solo en memoria)
     * @param key Nombre de la clave
     * @param value Valor a establecer
     * 
     * @note Los cambios solo afectan la instancia en memoria.
     *       Para persistir, llama a save()
     */
    static void set(const std::string& key, const std::string& value) {
        ensureInitialized();
        config_->set(key, value);
    }

    /**
     * @brief Guarda los cambios al archivo config.dat
     * @param password Contraseña maestra para cifrar
     * @return true si se guardó correctamente
     */
    static bool save(const std::string& password) {
        ensureInitialized();
        try {
            config_->save(password);
            return true;
        } catch (...) {
            return false;
        }
    }

    // ============================================================
    // Métodos de utilidad
    // ============================================================

    /**
     * @brief Obtiene todas las claves de configuración disponibles
     * @return Vector con todas las claves
     */
    static std::vector<std::string> getKeys() {
        ensureInitialized();
        return config_->getKeys();
    }

    /**
     * @brief Obtiene la ruta del archivo de configuración
     * @return Ruta al archivo config.dat
     */
    static std::string getConfigPath() {
        return configPath_;
    }

    /**
     * @brief Obtiene el número de claves de configuración cargadas
     * @return Cantidad de claves
     */
    static size_t getKeyCount() {
        if (!isInitialized()) return 0;
        return config_->getKeys().size();
    }

    /**
     * @brief Genera una contraseña maestra segura
     * @param length Longitud de la contraseña (por defecto 64)
     * @return SecureString con la contraseña generada
     */
    static SecureString generateMasterPassword(size_t length = 64) {
        return ConfigManager::generateMasterPassword();
    }

    // ============================================================
    // Métodos de diagnóstico y debug
    // ============================================================

    /**
     * @brief Lista todas las claves disponibles (para debug)
     * @param hideSecrets Si es true, oculta valores de claves sensibles
     */
    static void printConfig(bool hideSecrets = true) {
        ensureInitialized();
        
        auto keys = config_->getKeys();
        
        std::cout << "=== Configuración Cargada ===" << std::endl;
        std::cout << "Archivo: " << configPath_ << std::endl;
        std::cout << "Claves: " << keys.size() << std::endl;
        std::cout << std::endl;
        
        for (const auto& key : keys) {
            std::string value = config_->get(key);
            
            // Ocultar valores sensibles
            if (hideSecrets) {
                std::string keyUpper = key;
                std::transform(keyUpper.begin(), keyUpper.end(), keyUpper.begin(), ::toupper);
                
                if (keyUpper.find("PASSWORD") != std::string::npos ||
                    keyUpper.find("SECRET") != std::string::npos ||
                    keyUpper.find("KEY") != std::string::npos ||
                    keyUpper.find("TOKEN") != std::string::npos) {
                    value = "********";
                }
            }
            
            std::cout << "  " << key << " = " << value << std::endl;
        }
    }

    /**
     * @brief Verifica si una clave es sensible (contiene secretos)
     * @param key Nombre de la clave
     * @return true si es sensible
     */
    static bool isSensitiveKey(const std::string& key) {
        std::string keyUpper = key;
        std::transform(keyUpper.begin(), keyUpper.end(), keyUpper.begin(), ::toupper);
        
        return keyUpper.find("PASSWORD") != std::string::npos ||
               keyUpper.find("SECRET") != std::string::npos ||
               keyUpper.find("KEY") != std::string::npos ||
               keyUpper.find("TOKEN") != std::string::npos ||
               keyUpper.find("CREDENTIAL") != std::string::npos;
    }
};
