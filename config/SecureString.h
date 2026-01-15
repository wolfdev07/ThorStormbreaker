#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <windows.h>

/**
 * @brief String seguro que limpia automáticamente su memoria
 * 
 * Almacena datos sensibles y los limpia de forma segura al destruirse.
 * Usa SecureZeroMemory en Windows para evitar optimizaciones del compilador.
 */
class SecureString {
private:
    std::vector<char> data_;
    bool locked_;

    /**
     * @brief Limpia de forma segura la memoria
     */
    void secureWipe() {
        if (!data_.empty()) {
#ifdef _WIN32
            // Windows: usar SecureZeroMemory que no puede ser optimizado
            SecureZeroMemory(data_.data(), data_.size());
#else
            // Linux/Unix: volatile para evitar optimización
            volatile char* ptr = data_.data();
            for (size_t i = 0; i < data_.size(); ++i) {
                ptr[i] = 0;
            }
#endif
            data_.clear();
            data_.shrink_to_fit();
        }
    }

public:
    /**
     * @brief Constructor por defecto
     */
    SecureString() : locked_(false) {}

    /**
     * @brief Constructor desde string
     */
    explicit SecureString(const std::string& str) : locked_(false) {
        data_.assign(str.begin(), str.end());
        data_.push_back('\0');
    }

    /**
     * @brief Constructor desde buffer
     */
    SecureString(const char* buffer, size_t length) : locked_(false) {
        data_.assign(buffer, buffer + length);
        data_.push_back('\0');
    }

    /**
     * @brief Constructor de copia (eliminar para seguridad)
     */
    SecureString(const SecureString& other) = delete;

    /**
     * @brief Operador de asignación (eliminar para seguridad)
     */
    SecureString& operator=(const SecureString& other) = delete;

    /**
     * @brief Constructor de movimiento
     */
    SecureString(SecureString&& other) noexcept 
        : data_(std::move(other.data_))
        , locked_(other.locked_) {
        other.locked_ = false;
    }

    /**
     * @brief Operador de movimiento
     */
    SecureString& operator=(SecureString&& other) noexcept {
        if (this != &other) {
            secureWipe();
            data_ = std::move(other.data_);
            locked_ = other.locked_;
            other.locked_ = false;
        }
        return *this;
    }

    /**
     * @brief Destructor - limpia memoria de forma segura
     */
    ~SecureString() {
        secureWipe();
    }

    /**
     * @brief Obtiene el string (solo lectura)
     */
    std::string getString() const {
        if (data_.empty()) return "";
        return std::string(data_.data(), data_.size() - 1); // Excluir '\0'
    }

    /**
     * @brief Obtiene puntero a los datos (peligroso, usar con cuidado)
     */
    const char* c_str() const {
        return data_.empty() ? "" : data_.data();
    }

    /**
     * @brief Obtiene el tamaño del string (sin contar '\0')
     */
    size_t size() const {
        return data_.empty() ? 0 : data_.size() - 1;
    }

    /**
     * @brief Verifica si está vacío
     */
    bool empty() const {
        return data_.empty() || data_.size() <= 1;
    }

    /**
     * @brief Limpia el contenido de forma segura
     */
    void clear() {
        secureWipe();
    }

    /**
     * @brief Bloquea la memoria en RAM (evita swap a disco)
     * @return true si se bloqueó correctamente
     */
    bool lockMemory() {
        if (locked_ || data_.empty()) return false;

#ifdef _WIN32
        // Windows: usar VirtualLock
        locked_ = VirtualLock(data_.data(), data_.size()) != 0;
#else
        // Linux/Unix: usar mlock
        locked_ = mlock(data_.data(), data_.size()) == 0;
#endif
        return locked_;
    }

    /**
     * @brief Desbloquea la memoria
     */
    void unlockMemory() {
        if (!locked_) return;

#ifdef _WIN32
        VirtualUnlock(data_.data(), data_.size());
#else
        munlock(data_.data(), data_.size());
#endif
        locked_ = false;
    }
};
