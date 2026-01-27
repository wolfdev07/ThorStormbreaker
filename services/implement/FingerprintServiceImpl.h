#pragma once

#include "../IFingerprintService.h"
#include <windows.h>

/**
 * @brief Implementación concreta del servicio de huellas dactilares usando libzkfp
 * 
 * Esta clase implementa la interfaz IFingerprintService utilizando el SDK
 * de ZKTeco libzkfp para operaciones con lectores de huellas dactilares.
 */
class FingerprintServiceImpl : public IFingerprintService {
private:
    HANDLE m_hDevice;           // Handle del dispositivo
    HANDLE m_hDBCache;          // Handle del cache de la base de datos
    int m_imageWidth;           // Ancho de la imagen
    int m_imageHeight;          // Alto de la imagen
    int m_lastError;            // Último código de error
    bool m_isInitialized;       // Estado de inicialización del SDK
    bool m_isDeviceOpen;        // Estado del dispositivo

    /**
     * @brief Obtiene los parámetros de captura del dispositivo
     * @return true si se obtuvieron correctamente, false en caso contrario
     */
    bool fetchCaptureParameters();

public:
    /**
     * @brief Constructor
     */
    FingerprintServiceImpl();

    /**
     * @brief Destructor - limpia recursos automáticamente
     */
    ~FingerprintServiceImpl() override;

    // Implementación de métodos de IFingerprintService

    bool initialize() override;
    bool terminate() override;

    int getDeviceCount() override;
    bool openDevice(int index) override;
    bool closeDevice() override;
    bool isDeviceOpen() const override;

    bool acquireFingerprint(std::vector<unsigned char>& fpImage, 
                          std::vector<unsigned char>& fpTemplate) override;
    bool acquireFingerprintImage(std::vector<unsigned char>& fpImage) override;

    bool initDatabase() override;
    bool freeDatabase() override;
    bool mergeTemplates(const std::vector<unsigned char>& temp1,
                       const std::vector<unsigned char>& temp2,
                       const std::vector<unsigned char>& temp3,
                       std::vector<unsigned char>& regTemplate) override;
    bool addTemplate(unsigned int fid, const std::vector<unsigned char>& fpTemplate) override;
    bool deleteTemplate(unsigned int fid) override;
    bool clearDatabase() override;
    int getDatabaseCount() override;

    bool identify(const std::vector<unsigned char>& fpTemplate,
                 unsigned int& fid,
                 unsigned int& score) override;
    bool matchTemplates(const std::vector<unsigned char>& template1,
                       const std::vector<unsigned char>& template2,
                       int& score) override;

    bool getImageDimensions(int& width, int& height) override;
    int getLastError() const override;

    bool setLed(int ledCode, bool on) override;

    void ledGreenOn() override;
    void ledGreenOff() override;
    void ledRedOn() override;
    void ledRedOff() override;

    std::string getErrorMessage(int errorCode) const override;

    void cancel() override;
};
