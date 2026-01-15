#pragma once

#include "SecureString.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <windows.h>
#include <wincrypt.h>

#pragma comment(lib, "advapi32.lib")

/**
 * @brief Clase para cifrar/descifrar datos usando AES-256
 * 
 * Utiliza la API de criptografía de Windows (CryptoAPI) para cifrado seguro.
 * Los datos se cifran con AES-256 en modo CBC.
 */
class Cipher {
private:
    static constexpr size_t AES_KEY_SIZE = 32; // 256 bits
    static constexpr size_t AES_BLOCK_SIZE = 16;

    /**
     * @brief Deriva una clave de cifrado desde una contraseña
     */
    static std::vector<unsigned char> deriveKey(const std::string& password) {
        HCRYPTPROV hProv = 0;
        HCRYPTHASH hHash = 0;
        std::vector<unsigned char> key(AES_KEY_SIZE);

        try {
            // Obtener contexto de cifrado
            if (!CryptAcquireContext(&hProv, nullptr, nullptr, 
                PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
                throw std::runtime_error("Error al adquirir contexto de cifrado");
            }

            // Crear hash SHA-256
            if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al crear hash");
            }

            // Hash de la contraseña
            if (!CryptHashData(hHash, (BYTE*)password.c_str(), 
                password.length(), 0)) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al hashear contraseña");
            }

            // Obtener el hash
            DWORD hashLen = AES_KEY_SIZE;
            if (!CryptGetHashParam(hHash, HP_HASHVAL, key.data(), 
                &hashLen, 0)) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al obtener hash");
            }

            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);

            return key;

        } catch (...) {
            if (hHash) CryptDestroyHash(hHash);
            if (hProv) CryptReleaseContext(hProv, 0);
            throw;
        }
    }

public:
    /**
     * @brief Cifra datos usando AES-256
     * @param plaintext Texto plano a cifrar
     * @param password Contraseña para derivar la clave
     * @return Datos cifrados (IV + datos cifrados)
     */
    static std::vector<unsigned char> encrypt(
        const std::string& plaintext,
        const std::string& password) {
        
        if (plaintext.empty()) {
            throw std::runtime_error("No se puede cifrar texto vacío");
        }

        HCRYPTPROV hProv = 0;
        HCRYPTHASH hHash = 0;
        HCRYPTKEY hKey = 0;

        try {
            // Adquirir contexto
            if (!CryptAcquireContext(&hProv, nullptr, nullptr, 
                PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
                throw std::runtime_error("Error al adquirir contexto");
            }

            // Crear hash de la contraseña
            if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al crear hash");
            }

            if (!CryptHashData(hHash, (BYTE*)password.c_str(), 
                password.length(), 0)) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al hashear password");
            }

            // Derivar clave AES-256
            if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0, &hKey)) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al derivar clave");
            }

            // Preparar buffer para cifrado (necesita espacio extra)
            std::vector<unsigned char> buffer(plaintext.begin(), plaintext.end());
            DWORD dataLen = buffer.size();
            DWORD bufferLen = dataLen + AES_BLOCK_SIZE;
            buffer.resize(bufferLen);

            // Cifrar
            if (!CryptEncrypt(hKey, 0, TRUE, 0, buffer.data(), 
                &dataLen, bufferLen)) {
                CryptDestroyKey(hKey);
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al cifrar datos");
            }

            buffer.resize(dataLen);

            // Limpiar
            CryptDestroyKey(hKey);
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);

            return buffer;

        } catch (...) {
            if (hKey) CryptDestroyKey(hKey);
            if (hHash) CryptDestroyHash(hHash);
            if (hProv) CryptReleaseContext(hProv, 0);
            throw;
        }
    }

    /**
     * @brief Descifra datos usando AES-256
     * @param ciphertext Datos cifrados
     * @param password Contraseña para derivar la clave
     * @return SecureString con los datos descifrados
     */
    static SecureString decrypt(
        const std::vector<unsigned char>& ciphertext,
        const std::string& password) {
        
        if (ciphertext.empty()) {
            throw std::runtime_error("No se puede descifrar datos vacíos");
        }

        HCRYPTPROV hProv = 0;
        HCRYPTHASH hHash = 0;
        HCRYPTKEY hKey = 0;

        try {
            // Adquirir contexto
            if (!CryptAcquireContext(&hProv, nullptr, nullptr, 
                PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
                throw std::runtime_error("Error al adquirir contexto");
            }

            // Crear hash de la contraseña
            if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al crear hash");
            }

            if (!CryptHashData(hHash, (BYTE*)password.c_str(), 
                password.length(), 0)) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al hashear password");
            }

            // Derivar clave AES-256
            if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0, &hKey)) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al derivar clave");
            }

            // Copiar datos cifrados a buffer mutable
            std::vector<unsigned char> buffer = ciphertext;
            DWORD dataLen = buffer.size();

            // Descifrar
            if (!CryptDecrypt(hKey, 0, TRUE, 0, buffer.data(), &dataLen)) {
                CryptDestroyKey(hKey);
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                throw std::runtime_error("Error al descifrar datos - contraseña incorrecta?");
            }

            // Crear SecureString desde buffer descifrado
            SecureString result((const char*)buffer.data(), dataLen);

            // Limpiar buffer de forma segura
            SecureZeroMemory(buffer.data(), buffer.size());

            // Limpiar recursos
            CryptDestroyKey(hKey);
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);

            return result;

        } catch (...) {
            if (hKey) CryptDestroyKey(hKey);
            if (hHash) CryptDestroyHash(hHash);
            if (hProv) CryptReleaseContext(hProv, 0);
            throw;
        }
    }

    /**
     * @brief Genera una contraseña aleatoria
     * @param length Longitud de la contraseña
     * @return SecureString con la contraseña generada
     */
    static SecureString generatePassword(size_t length = 32) {
        HCRYPTPROV hProv = 0;
        
        if (!CryptAcquireContext(&hProv, nullptr, nullptr, 
            PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
            throw std::runtime_error("Error al generar contraseña");
        }

        std::vector<unsigned char> randomBytes(length);
        if (!CryptGenRandom(hProv, length, randomBytes.data())) {
            CryptReleaseContext(hProv, 0);
            throw std::runtime_error("Error al generar bytes aleatorios");
        }

        CryptReleaseContext(hProv, 0);

        // Convertir a string hexadecimal
        std::string hexStr;
        hexStr.reserve(length * 2);
        const char* hexChars = "0123456789ABCDEF";
        
        for (unsigned char byte : randomBytes) {
            hexStr += hexChars[byte >> 4];
            hexStr += hexChars[byte & 0x0F];
        }

        SecureString result(hexStr);
        
        // Limpiar
        SecureZeroMemory(randomBytes.data(), randomBytes.size());
        SecureZeroMemory((void*)hexStr.data(), hexStr.size());

        return result;
    }
};
