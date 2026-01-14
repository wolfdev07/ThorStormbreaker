#include "FingerprintServiceImpl.h"
#include <windows.h>
#include <libzkfp.h>
#include <sstream>

// Constante para el tamaño máximo de template
#define MAX_TEMPLATE_SIZE 2048

// Incluir códigos de error
#include <libzkfperrdef.h>

// Redefinir los códigos de error que tienen punto y coma en el header original
#ifdef ZKFP_ERR_NOT_OPENED
#undef ZKFP_ERR_NOT_OPENED
#endif
#define ZKFP_ERR_NOT_OPENED -23

#ifdef ZKFP_ERR_NOT_INIT
#undef ZKFP_ERR_NOT_INIT
#endif
#define ZKFP_ERR_NOT_INIT -24

#ifdef ZKFP_ERR_ALREADY_OPENED
#undef ZKFP_ERR_ALREADY_OPENED
#endif
#define ZKFP_ERR_ALREADY_OPENED -25

FingerprintServiceImpl::FingerprintServiceImpl()
    : m_hDevice(nullptr)
    , m_hDBCache(nullptr)
    , m_imageWidth(0)
    , m_imageHeight(0)
    , m_lastError(0)
    , m_isInitialized(false)
    , m_isDeviceOpen(false)
{
}

FingerprintServiceImpl::~FingerprintServiceImpl() {
    // Limpieza automática de recursos
    closeDevice();
    freeDatabase();
    terminate();
}

bool FingerprintServiceImpl::initialize() {
    if (m_isInitialized) {
        m_lastError = ZKFP_ERR_ALREADY_INIT;
        return true; // Ya está inicializado
    }

    m_lastError = ZKFPM_Init();
    if (m_lastError == ZKFP_ERR_OK) {
        m_isInitialized = true;
        return true;
    }
    return false;
}

bool FingerprintServiceImpl::terminate() {
    if (!m_isInitialized) {
        return true; // Ya está terminado
    }

    // Asegurarse de cerrar todo antes de terminar
    closeDevice();
    freeDatabase();

    m_lastError = ZKFPM_Terminate();
    if (m_lastError == ZKFP_ERR_OK) {
        m_isInitialized = false;
        return true;
    }
    return false;
}

int FingerprintServiceImpl::getDeviceCount() {
    if (!m_isInitialized) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return -1;
    }

    int count = ZKFPM_GetDeviceCount();
    if (count < 0) {
        m_lastError = count;
    }
    return count;
}

bool FingerprintServiceImpl::openDevice(int index) {
    if (!m_isInitialized) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return false;
    }

    if (m_isDeviceOpen) {
        m_lastError = ZKFP_ERR_ALREADY_OPENED;
        return false;
    }

    m_hDevice = ZKFPM_OpenDevice(index);
    if (m_hDevice == nullptr) {
        m_lastError = ZKFP_ERR_OPEN;
        return false;
    }

    m_isDeviceOpen = true;
    
    // Obtener parámetros de captura
    if (!fetchCaptureParameters()) {
        closeDevice();
        return false;
    }

    m_lastError = ZKFP_ERR_OK;
    return true;
}

bool FingerprintServiceImpl::closeDevice() {
    if (!m_isDeviceOpen || m_hDevice == nullptr) {
        return true; // Ya está cerrado
    }

    m_lastError = ZKFPM_CloseDevice(m_hDevice);
    if (m_lastError == ZKFP_ERR_OK) {
        m_hDevice = nullptr;
        m_isDeviceOpen = false;
        m_imageWidth = 0;
        m_imageHeight = 0;
        return true;
    }
    return false;
}

bool FingerprintServiceImpl::isDeviceOpen() const {
    return m_isDeviceOpen;
}

bool FingerprintServiceImpl::fetchCaptureParameters() {
    if (!m_isDeviceOpen || m_hDevice == nullptr) {
        m_lastError = ZKFP_ERR_NOT_OPENED;
        return false;
    }

    unsigned int width = 0;
    unsigned int widthSize = sizeof(width);
    m_lastError = ZKFPM_GetParameters(m_hDevice, 1, 
                                      reinterpret_cast<unsigned char*>(&width), 
                                      &widthSize);
    if (m_lastError != ZKFP_ERR_OK) {
        return false;
    }

    unsigned int height = 0;
    unsigned int heightSize = sizeof(height);
    m_lastError = ZKFPM_GetParameters(m_hDevice, 2, 
                                      reinterpret_cast<unsigned char*>(&height), 
                                      &heightSize);
    if (m_lastError != ZKFP_ERR_OK) {
        return false;
    }

    m_imageWidth = static_cast<int>(width);
    m_imageHeight = static_cast<int>(height);

    return true;
}

bool FingerprintServiceImpl::acquireFingerprint(std::vector<unsigned char>& fpImage, 
                                               std::vector<unsigned char>& fpTemplate) {
    if (!m_isDeviceOpen || m_hDevice == nullptr) {
        m_lastError = ZKFP_ERR_NOT_OPENED;
        return false;
    }

    // Redimensionar buffers
    fpImage.resize(m_imageWidth * m_imageHeight);
    fpTemplate.resize(MAX_TEMPLATE_SIZE);

    unsigned int templateSize = MAX_TEMPLATE_SIZE;
    m_lastError = ZKFPM_AcquireFingerprint(m_hDevice, 
                                          fpImage.data(), 
                                          static_cast<unsigned int>(fpImage.size()),
                                          fpTemplate.data(), 
                                          &templateSize);

    if (m_lastError == ZKFP_ERR_OK) {
        fpTemplate.resize(templateSize); // Ajustar al tamaño real
        return true;
    }

    return false;
}

bool FingerprintServiceImpl::acquireFingerprintImage(std::vector<unsigned char>& fpImage) {
    if (!m_isDeviceOpen || m_hDevice == nullptr) {
        m_lastError = ZKFP_ERR_NOT_OPENED;
        return false;
    }

    // Redimensionar buffer
    fpImage.resize(m_imageWidth * m_imageHeight);

    m_lastError = ZKFPM_AcquireFingerprintImage(m_hDevice, 
                                               fpImage.data(), 
                                               static_cast<unsigned int>(fpImage.size()));

    return (m_lastError == ZKFP_ERR_OK);
}

bool FingerprintServiceImpl::initDatabase() {
    if (m_hDBCache != nullptr) {
        return true; // Ya está inicializado
    }

    m_hDBCache = ZKFPM_DBInit();
    if (m_hDBCache == nullptr) {
        m_lastError = ZKFP_ERR_INITLIB;
        return false;
    }

    m_lastError = ZKFP_ERR_OK;
    return true;
}

bool FingerprintServiceImpl::freeDatabase() {
    if (m_hDBCache == nullptr) {
        return true; // Ya está liberado
    }

    m_lastError = ZKFPM_DBFree(m_hDBCache);
    if (m_lastError == ZKFP_ERR_OK) {
        m_hDBCache = nullptr;
        return true;
    }
    return false;
}

bool FingerprintServiceImpl::mergeTemplates(const std::vector<unsigned char>& temp1,
                                           const std::vector<unsigned char>& temp2,
                                           const std::vector<unsigned char>& temp3,
                                           std::vector<unsigned char>& regTemplate) {
    if (m_hDBCache == nullptr) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return false;
    }

    regTemplate.resize(MAX_TEMPLATE_SIZE);
    unsigned int regTemplateSize = MAX_TEMPLATE_SIZE;

    m_lastError = ZKFPM_DBMerge(m_hDBCache,
                               const_cast<unsigned char*>(temp1.data()),
                               const_cast<unsigned char*>(temp2.data()),
                               const_cast<unsigned char*>(temp3.data()),
                               regTemplate.data(),
                               &regTemplateSize);

    if (m_lastError == ZKFP_ERR_OK) {
        regTemplate.resize(regTemplateSize);
        return true;
    }

    return false;
}

bool FingerprintServiceImpl::addTemplate(unsigned int fid, 
                                        const std::vector<unsigned char>& fpTemplate) {
    if (m_hDBCache == nullptr) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return false;
    }

    m_lastError = ZKFPM_DBAdd(m_hDBCache, fid, 
                             const_cast<unsigned char*>(fpTemplate.data()),
                             static_cast<unsigned int>(fpTemplate.size()));

    return (m_lastError == ZKFP_ERR_OK);
}

bool FingerprintServiceImpl::deleteTemplate(unsigned int fid) {
    if (m_hDBCache == nullptr) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return false;
    }

    m_lastError = ZKFPM_DBDel(m_hDBCache, fid);
    return (m_lastError == ZKFP_ERR_OK);
}

bool FingerprintServiceImpl::clearDatabase() {
    if (m_hDBCache == nullptr) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return false;
    }

    m_lastError = ZKFPM_DBClear(m_hDBCache);
    return (m_lastError == ZKFP_ERR_OK);
}

int FingerprintServiceImpl::getDatabaseCount() {
    if (m_hDBCache == nullptr) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return -1;
    }

    unsigned int count = 0;
    m_lastError = ZKFPM_DBCount(m_hDBCache, &count);

    if (m_lastError == ZKFP_ERR_OK) {
        return static_cast<int>(count);
    }

    return -1;
}

bool FingerprintServiceImpl::identify(const std::vector<unsigned char>& fpTemplate,
                                     unsigned int& fid,
                                     unsigned int& score) {
    if (m_hDBCache == nullptr) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return false;
    }

    m_lastError = ZKFPM_DBIdentify(m_hDBCache,
                                  const_cast<unsigned char*>(fpTemplate.data()),
                                  static_cast<unsigned int>(fpTemplate.size()),
                                  &fid,
                                  &score);

    return (m_lastError == ZKFP_ERR_OK);
}

bool FingerprintServiceImpl::matchTemplates(const std::vector<unsigned char>& template1,
                                           const std::vector<unsigned char>& template2,
                                           int& score) {
    if (m_hDBCache == nullptr) {
        m_lastError = ZKFP_ERR_NOT_INIT;
        return false;
    }

    int matchScore = ZKFPM_DBMatch(m_hDBCache,
                                   const_cast<unsigned char*>(template1.data()),
                                   static_cast<unsigned int>(template1.size()),
                                   const_cast<unsigned char*>(template2.data()),
                                   static_cast<unsigned int>(template2.size()));

    if (matchScore >= 0) {
        score = matchScore;
        m_lastError = ZKFP_ERR_OK;
        return true;
    }

    m_lastError = matchScore;
    return false;
}

bool FingerprintServiceImpl::getImageDimensions(int& width, int& height) {
    if (!m_isDeviceOpen) {
        m_lastError = ZKFP_ERR_NOT_OPENED;
        return false;
    }

    width = m_imageWidth;
    height = m_imageHeight;
    return true;
}

int FingerprintServiceImpl::getLastError() const {
    return m_lastError;
}

std::string FingerprintServiceImpl::getErrorMessage(int errorCode) const {
    switch (errorCode) {
        case ZKFP_ERR_OK:
            return "Operación exitosa";
        case ZKFP_ERR_ALREADY_INIT:
            return "Ya está inicializado";
        case ZKFP_ERR_INITLIB:
            return "Error al inicializar la librería de algoritmos";
        case ZKFP_ERR_INIT:
            return "Error al inicializar la librería de captura";
        case ZKFP_ERR_NO_DEVICE:
            return "No se encontró ningún dispositivo";
        case ZKFP_ERR_NOT_SUPPORT:
            return "Interfaz no soportada";
        case ZKFP_ERR_INVALID_PARAM:
            return "Parámetro inválido";
        case ZKFP_ERR_OPEN:
            return "Error al abrir el dispositivo";
        case ZKFP_ERR_INVALID_HANDLE:
            return "Handle inválido";
        case ZKFP_ERR_CAPTURE:
            return "Error en la captura";
        case ZKFP_ERR_EXTRACT_FP:
            return "Error al extraer el template de huella";
        case ZKFP_ERR_ABSORT:
            return "Operación abortada";
        case ZKFP_ERR_MEMORY_NOT_ENOUGH:
            return "Memoria insuficiente";
        case ZKFP_ERR_BUSY:
            return "Ocupado capturando";
        case ZKFP_ERR_ADD_FINGER:
            return "Error al agregar template de huella";
        case ZKFP_ERR_DEL_FINGER:
            return "Error al eliminar huella";
        case ZKFP_ERR_FAIL:
            return "Operación fallida";
        case ZKFP_ERR_CANCEL:
            return "Captura cancelada";
        case ZKFP_ERR_VERIFY_FP:
            return "Error al verificar huella";
        case ZKFP_ERR_MERGE:
            return "Error al fusionar templates de registro";
        case ZKFP_ERR_NOT_OPENED:
            return "Dispositivo no abierto";
        case ZKFP_ERR_NOT_INIT:
            return "No inicializado";
        case ZKFP_ERR_ALREADY_OPENED:
            return "Dispositivo ya abierto";
        case ZKFP_ERR_LOADIMAGE:
            return "Error al cargar archivo";
        case ZKFP_ERR_ANALYSE_IMG:
            return "Error al analizar imagen";
        case ZKFP_ERR_TIMEOUT:
            return "Tiempo de espera agotado";
        default:
            return "Error desconocido (código: " + std::to_string(errorCode) + ")";
    }
}
