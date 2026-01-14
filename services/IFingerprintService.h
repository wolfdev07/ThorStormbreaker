#pragma once

#include <memory>
#include <vector>
#include <string>

/**
 * @brief Interfaz para el servicio de lector de huellas dactilares
 * 
 * Esta es una clase abstracta pura
 * que define el contrato para las operaciones del lector de huellas.
 */
class IFingerprintService {
public:
    virtual ~IFingerprintService() = default;

    // ============================================
    // Métodos de inicialización y terminación
    // ============================================
    
    /**
     * @brief Inicializa el SDK de huellas dactilares
     * @return true si la inicialización fue exitosa, false en caso contrario
     */
    virtual bool initialize() = 0;

    /**
     * @brief Libera los recursos del SDK
     * @return true si la terminación fue exitosa, false en caso contrario
     */
    virtual bool terminate() = 0;

    // ============================================
    // Métodos de gestión de dispositivos
    // ============================================

    /**
     * @brief Obtiene el número de dispositivos conectados
     * @return Número de dispositivos encontrados
     */
    virtual int getDeviceCount() = 0;

    /**
     * @brief Abre un dispositivo específico
     * @param index Índice del dispositivo (0 para el primero)
     * @return true si el dispositivo se abrió correctamente, false en caso contrario
     */
    virtual bool openDevice(int index) = 0;

    /**
     * @brief Cierra el dispositivo actual
     * @return true si el dispositivo se cerró correctamente, false en caso contrario
     */
    virtual bool closeDevice() = 0;

    /**
     * @brief Verifica si hay un dispositivo abierto
     * @return true si hay un dispositivo abierto, false en caso contrario
     */
    virtual bool isDeviceOpen() const = 0;

    // ============================================
    // Métodos de captura de huellas
    // ============================================

    /**
     * @brief Captura una huella dactilar y extrae su template
     * @param fpImage [out] Vector para almacenar la imagen capturada
     * @param fpTemplate [out] Vector para almacenar el template extraído
     * @return true si la captura fue exitosa, false en caso contrario
     */
    virtual bool acquireFingerprint(std::vector<unsigned char>& fpImage, 
                                   std::vector<unsigned char>& fpTemplate) = 0;

    /**
     * @brief Captura solo la imagen de la huella (sin extraer template)
     * @param fpImage [out] Vector para almacenar la imagen capturada
     * @return true si la captura fue exitosa, false en caso contrario
     */
    virtual bool acquireFingerprintImage(std::vector<unsigned char>& fpImage) = 0;

    // ============================================
    // Métodos de gestión de base de datos
    // ============================================

    /**
     * @brief Inicializa el cache de la base de datos de templates
     * @return true si la inicialización fue exitosa, false en caso contrario
     */
    virtual bool initDatabase() = 0;

    /**
     * @brief Libera el cache de la base de datos
     * @return true si la liberación fue exitosa, false en caso contrario
     */
    virtual bool freeDatabase() = 0;

    /**
     * @brief Combina tres templates en uno de registro
     * @param temp1 Primer template
     * @param temp2 Segundo template
     * @param temp3 Tercer template
     * @param regTemplate [out] Template de registro resultante
     * @return true si la combinación fue exitosa, false en caso contrario
     */
    virtual bool mergeTemplates(const std::vector<unsigned char>& temp1,
                               const std::vector<unsigned char>& temp2,
                               const std::vector<unsigned char>& temp3,
                               std::vector<unsigned char>& regTemplate) = 0;

    /**
     * @brief Agrega un template a la base de datos
     * @param fid ID de la huella (debe ser > 0)
     * @param fpTemplate Template a agregar
     * @return true si se agregó correctamente, false en caso contrario
     */
    virtual bool addTemplate(unsigned int fid, const std::vector<unsigned char>& fpTemplate) = 0;

    /**
     * @brief Elimina un template de la base de datos
     * @param fid ID de la huella a eliminar
     * @return true si se eliminó correctamente, false en caso contrario
     */
    virtual bool deleteTemplate(unsigned int fid) = 0;

    /**
     * @brief Limpia todos los templates de la base de datos
     * @return true si se limpió correctamente, false en caso contrario
     */
    virtual bool clearDatabase() = 0;

    /**
     * @brief Obtiene el número de templates en la base de datos
     * @return Número de templates almacenados
     */
    virtual int getDatabaseCount() = 0;

    // ============================================
    // Métodos de identificación y verificación
    // ============================================

    /**
     * @brief Identifica una huella comparándola con todas las de la BD (1:N)
     * @param fpTemplate Template a identificar
     * @param fid [out] ID de la huella identificada
     * @param score [out] Puntuación de coincidencia
     * @return true si se identificó correctamente, false en caso contrario
     */
    virtual bool identify(const std::vector<unsigned char>& fpTemplate,
                         unsigned int& fid,
                         unsigned int& score) = 0;

    /**
     * @brief Compara dos templates (1:1)
     * @param template1 Primer template
     * @param template2 Segundo template
     * @param score [out] Puntuación de coincidencia
     * @return true si la comparación fue exitosa, false en caso contrario
     */
    virtual bool matchTemplates(const std::vector<unsigned char>& template1,
                               const std::vector<unsigned char>& template2,
                               int& score) = 0;

    // ============================================
    // Métodos de utilidad
    // ============================================

    /**
     * @brief Obtiene las dimensiones de la imagen capturada
     * @param width [out] Ancho de la imagen
     * @param height [out] Alto de la imagen
     * @return true si se obtuvieron las dimensiones, false en caso contrario
     */
    virtual bool getImageDimensions(int& width, int& height) = 0;

    /**
     * @brief Obtiene el último código de error
     * @return Código de error de la última operación
     */
    virtual int getLastError() const = 0;

    /**
     * @brief Obtiene el mensaje de error asociado a un código
     * @param errorCode Código de error
     * @return Descripción del error
     */
    virtual std::string getErrorMessage(int errorCode) const = 0;
};
