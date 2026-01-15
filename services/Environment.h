#pragma once

#include "../config/ConfigEnvironment.h"
#include <string>
#include <optional>
#include <vector>

/**
 * @brief Wrapper de compatibilidad para ConfigEnvironment
 * 
 * Esta clase mantiene compatibilidad con código existente que usa Environment.
 * Internamente delega todas las llamadas a ConfigEnvironment.
 * 
 * MIGRADO desde variables de entorno (.env) a archivo cifrado (config.dat)
 * 
 * @deprecated Se recomienda usar ConfigEnvironment directamente en código nuevo
 * 
 * @note IMPORTANTE: Debes llamar a Environment::initialize(password) 
 *       antes de usar cualquier otro método.
 */
class Environment {
public:
class Environment {
public:
    /**
     * @brief Inicializa el sistema de configuración
     * @param password Contraseña maestra para descifrar config.dat
     * @param configPath Ruta al archivo config.dat
     * @return true si se inicializó correctamente
     */
    static bool initialize(const std::string& password, 
                          const std::string& configPath = "config.dat") {
        return ConfigEnvironment::initialize(password, configPath);
    }

    /**
     * @brief Verifica si el sistema está inicializado
     */
    static bool isInitialized() {
        return ConfigEnvironment::isInitialized();
    }

    /**
     * @brief Limpia y cierra el sistema de configuración
     */
    static void shutdown() {
        ConfigEnvironment::shutdown();
    }

    /**
     * @brief Recarga la configuración desde el archivo
     */
    static bool reload(const std::string& password) {
        return ConfigEnvironment::reload(password);
    }

    /**
     * @brief Obtiene una variable de configuración como string
     */
    static std::string get(const std::string& key, const std::string& defaultValue = "") {
        return ConfigEnvironment::get(key, defaultValue);
    }

    /**
     * @brief Obtiene una variable de configuración como int
     */
    static int getInt(const std::string& key, int defaultValue = 0) {
        return ConfigEnvironment::getInt(key, defaultValue);
    }

    /**
     * @brief Obtiene una variable de configuración como bool
     */
    static bool getBool(const std::string& key, bool defaultValue = false) {
        return ConfigEnvironment::getBool(key, defaultValue);
    }

    /**
     * @brief Obtiene una variable de configuración como double
     */
    static double getDouble(const std::string& key, double defaultValue = 0.0) {
        return ConfigEnvironment::getDouble(key, defaultValue);
    }

    /**
     * @brief Verifica si existe una clave de configuración
     */
    static bool has(const std::string& key) {
        return ConfigEnvironment::has(key);
    }

    /**
     * @brief Obtiene una variable de configuración como optional
     */
    static std::optional<std::string> getOptional(const std::string& key) {
        return ConfigEnvironment::getOptional(key);
    }

    /**
     * @brief Obtiene una variable requerida (lanza excepción si no existe)
     */
    static std::string require(const std::string& key) {
        return ConfigEnvironment::require(key);
    }

    /**
     * @brief Establece una variable de configuración (solo en memoria)
     */
    static void set(const std::string& key, const std::string& value) {
        ConfigEnvironment::set(key, value);
    }

    /**
     * @brief Guarda los cambios al archivo config.dat
     */
    static bool save(const std::string& password) {
        return ConfigEnvironment::save(password);
    }

    /**
     * @brief Obtiene todas las claves de configuración disponibles
     */
    static std::vector<std::string> getKeys() {
        return ConfigEnvironment::getKeys();
    }

    /**
     * @brief Lista todas las claves disponibles (para debug)
     */
    static void printConfig(bool hideSecrets = true) {
        ConfigEnvironment::printConfig(hideSecrets);
    }

    // ============================================================
    // MÉTODOS DEPRECADOS (compatibilidad con código antiguo)
    // ============================================================

    /**
     * @brief Obtiene todas las variables (DEPRECADO)
     * @deprecated Use getKeys() en su lugar
     */
    [[deprecated("Use getKeys() en su lugar")]]
    static std::map<std::string, std::string> getAll() {
        std::map<std::string, std::string> result;
        auto keys = ConfigEnvironment::getKeys();
        for (const auto& key : keys) {
            result[key] = ConfigEnvironment::get(key);
        }
        return result;
    }
};
