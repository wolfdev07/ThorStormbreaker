#pragma once

#include "SecureString.h"
#include "Cipher.h"
#include <string>
#include <map>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <windows.h>

/**
 * @brief Gestor de configuración cifrada
 * 
 * Lee configuración desde un archivo .dat cifrado, descifra los valores en RAM,
 * y limpia la memoria de forma segura después de su uso.
 * 
 * Formato del archivo config.dat (cifrado):
 * CLAVE1=valor1
 * CLAVE2=valor2
 * # Comentarios
 */
class ConfigManager {
private:
    std::map<std::string, SecureString> config_;
    std::string configPath_;
    bool loaded_;

    /**
     * @brief Parsea una línea del archivo de configuración
     */
    static std::pair<std::string, std::string> parseLine(const std::string& line) {
        // Ignorar comentarios y líneas vacías
        if (line.empty() || line[0] == '#') {
            return {"", ""};
        }

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            return {"", ""};
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Trim whitespace
        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
        };

        trim(key);
        trim(value);

        return {key, value};
    }

    /**
     * @brief Lee el archivo cifrado
     */
    std::vector<unsigned char> readEncryptedFile(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("No se puede abrir el archivo: " + filePath);
        }

        // Leer todo el archivo
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> buffer(fileSize);
        file.read((char*)buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    /**
     * @brief Escribe datos cifrados al archivo
     */
    void writeEncryptedFile(const std::string& filePath, 
                           const std::vector<unsigned char>& data) {
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("No se puede escribir el archivo: " + filePath);
        }

        file.write((const char*)data.data(), data.size());
        file.close();
    }

public:
    /**
     * @brief Constructor
     * @param configPath Ruta al archivo config.dat
     */
    explicit ConfigManager(const std::string& configPath = "config.dat")
        : configPath_(configPath)
        , loaded_(false) {
    }

    /**
     * @brief Destructor - limpia memoria de forma segura
     */
    ~ConfigManager() {
        clear();
    }

    /**
     * @brief Carga el archivo de configuración cifrado
     * @param password Contraseña para descifrar
     * @return true si se cargó correctamente
     */
    bool load(const std::string& password) {
        try {
            // Leer archivo cifrado
            auto encryptedData = readEncryptedFile(configPath_);

            // Descifrar en RAM
            SecureString decryptedContent = Cipher::decrypt(encryptedData, password);

            // Bloquear memoria en RAM (evitar swap a disco)
            decryptedContent.lockMemory();

            // Parsear contenido
            std::string content = decryptedContent.getString();
            std::istringstream iss(content);
            std::string line;

            while (std::getline(iss, line)) {
                auto [key, value] = parseLine(line);
                if (!key.empty()) {
                    SecureString secureValue(value);
                    secureValue.lockMemory(); // Bloquear en RAM
                    config_[key] = std::move(secureValue);
                }
            }

            // Limpiar contenido descifrado
            decryptedContent.clear();

            loaded_ = true;
            return true;

        } catch (const std::exception& e) {
            // Limpiar en caso de error
            clear();
            throw std::runtime_error(std::string("Error al cargar config: ") + e.what());
        }
    }

    /**
     * @brief Crea un nuevo archivo de configuración cifrado
     * @param password Contraseña para cifrar
     * @param initialConfig Configuración inicial (clave-valor)
     */
    void create(const std::string& password, 
                const std::map<std::string, std::string>& initialConfig = {}) {
        try {
            // Crear contenido del archivo
            std::ostringstream oss;
            oss << "# ThorStormbreaker Configuration File\n";
            oss << "# Este archivo está cifrado con AES-256\n";
            oss << "# Generado automáticamente\n\n";

            for (const auto& [key, value] : initialConfig) {
                oss << key << "=" << value << "\n";
            }

            std::string content = oss.str();

            // Cifrar
            auto encryptedData = Cipher::encrypt(content, password);

            // Escribir al archivo
            writeEncryptedFile(configPath_, encryptedData);

            // Limpiar
            SecureZeroMemory((void*)content.data(), content.size());

        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error al crear config: ") + e.what());
        }
    }

    /**
     * @brief Obtiene un valor de configuración
     * @param key Clave
     * @param defaultValue Valor por defecto si no existe
     * @return Valor como string
     */
    std::string get(const std::string& key, const std::string& defaultValue = "") const {
        auto it = config_.find(key);
        if (it != config_.end()) {
            return it->second.getString();
        }
        return defaultValue;
    }

    /**
     * @brief Obtiene un valor como int
     */
    int getInt(const std::string& key, int defaultValue = 0) const {
        auto it = config_.find(key);
        if (it != config_.end()) {
            try {
                return std::stoi(it->second.getString());
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    /**
     * @brief Obtiene un valor como bool
     */
    bool getBool(const std::string& key, bool defaultValue = false) const {
        auto it = config_.find(key);
        if (it != config_.end()) {
            std::string value = it->second.getString();
            for (auto& c : value) c = std::tolower(c);
            return value == "true" || value == "1" || value == "yes" || value == "on";
        }
        return defaultValue;
    }

    /**
     * @brief Obtiene un valor como double
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        auto it = config_.find(key);
        if (it != config_.end()) {
            try {
                return std::stod(it->second.getString());
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    /**
     * @brief Verifica si existe una clave
     */
    bool has(const std::string& key) const {
        return config_.find(key) != config_.end();
    }

    /**
     * @brief Obtiene una clave requerida (lanza excepción si no existe)
     */
    std::string require(const std::string& key) const {
        auto it = config_.find(key);
        if (it == config_.end()) {
            throw std::runtime_error("Clave de configuración requerida no encontrada: " + key);
        }
        return it->second.getString();
    }

    /**
     * @brief Establece un valor (solo en memoria)
     */
    void set(const std::string& key, const std::string& value) {
        SecureString secureValue(value);
        secureValue.lockMemory();
        config_[key] = std::move(secureValue);
    }

    /**
     * @brief Guarda la configuración actual al archivo cifrado
     * @param password Contraseña para cifrar
     */
    void save(const std::string& password) {
        std::ostringstream oss;
        oss << "# ThorStormbreaker Configuration File\n";
        oss << "# Este archivo está cifrado con AES-256\n\n";

        for (const auto& [key, value] : config_) {
            oss << key << "=" << value.getString() << "\n";
        }

        std::string content = oss.str();
        auto encryptedData = Cipher::encrypt(content, password);
        writeEncryptedFile(configPath_, encryptedData);

        // Limpiar
        SecureZeroMemory((void*)content.data(), content.size());
    }

    /**
     * @brief Limpia toda la configuración de forma segura
     */
    void clear() {
        config_.clear();
        loaded_ = false;
    }

    /**
     * @brief Verifica si la configuración está cargada
     */
    bool isLoaded() const {
        return loaded_;
    }

    /**
     * @brief Obtiene todas las claves disponibles
     */
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(config_.size());
        for (const auto& [key, _] : config_) {
            keys.push_back(key);
        }
        return keys;
    }

    /**
     * @brief Genera una contraseña segura para el archivo config.dat
     * @return SecureString con la contraseña generada
     */
    static SecureString generateMasterPassword() {
        return Cipher::generatePassword(64); // 64 bytes = 512 bits
    }
};
