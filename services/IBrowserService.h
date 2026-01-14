#pragma once

#include <string>
#include <functional>
#include <memory>

/**
 * @brief Interfaz para el servicio de navegador web embebido (CEF)
 * 
 * Esta interfaz define el contrato para gestionar un navegador Chromium embebido
 * capaz de renderizar aplicaciones web modernas
 */
class IBrowserService {
public:
    virtual ~IBrowserService() = default;

    // ============================================
    // Callbacks y eventos
    // ============================================

    /**
     * @brief Callback cuando la página termina de cargar
     */
    using OnLoadCompleteCallback = std::function<void(const std::string& url)>;

    /**
     * @brief Callback cuando ocurre un error de carga
     */
    using OnLoadErrorCallback = std::function<void(const std::string& url, int errorCode, const std::string& errorText)>;

    /**
     * @brief Callback cuando cambia el título de la página
     */
    using OnTitleChangeCallback = std::function<void(const std::string& title)>;

    /**
     * @brief Callback para mensajes de consola JavaScript
     */
    using OnConsoleMessageCallback = std::function<void(const std::string& message, const std::string& source, int line)>;

    // ============================================
    // Inicialización y configuración
    // ============================================

    /**
     * @brief Inicializa el subsistema CEF
     * @param argc Número de argumentos de línea de comandos
     * @param argv Array de argumentos de línea de comandos
     * @return true si la inicialización fue exitosa
     */
    virtual bool initialize(int argc, char** argv) = 0;

    /**
     * @brief Finaliza el subsistema CEF
     */
    virtual void shutdown() = 0;

    /**
     * @brief Verifica si CEF está inicializado
     * @return true si está inicializado
     */
    virtual bool isInitialized() const = 0;

    // ============================================
    // Creación y gestión de navegador
    // ============================================

    /**
     * @brief Crea una ventana de navegador
     * @param url URL inicial a cargar
     * @param windowTitle Título de la ventana
     * @param width Ancho de la ventana en píxeles
     * @param height Alto de la ventana en píxeles
     * @return true si se creó exitosamente
     */
    virtual bool createBrowser(
        const std::string& url,
        const std::string& windowTitle = "ThorStormbreaker Browser",
        int width = 1280,
        int height = 720
    ) = 0;

    /**
     * @brief Cierra el navegador actual
     */
    virtual void closeBrowser() = 0;

    /**
     * @brief Verifica si hay un navegador activo
     * @return true si hay un navegador activo
     */
    virtual bool hasBrowser() const = 0;

    // ============================================
    // Navegación
    // ============================================

    /**
     * @brief Navega a una URL específica
     * @param url URL de destino
     */
    virtual void loadURL(const std::string& url) = 0;

    /**
     * @brief Recarga la página actual
     * @param ignoreCache Si true, ignora el caché
     */
    virtual void reload(bool ignoreCache = false) = 0;

    /**
     * @brief Retrocede en el historial
     */
    virtual void goBack() = 0;

    /**
     * @brief Avanza en el historial
     */
    virtual void goForward() = 0;

    /**
     * @brief Detiene la carga actual
     */
    virtual void stopLoad() = 0;

    /**
     * @brief Verifica si se puede retroceder
     * @return true si hay páginas anteriores en el historial
     */
    virtual bool canGoBack() const = 0;

    /**
     * @brief Verifica si se puede avanzar
     * @return true si hay páginas siguientes en el historial
     */
    virtual bool canGoForward() const = 0;

    // ============================================
    // Ejecución de JavaScript
    // ============================================

    /**
     * @brief Ejecuta código JavaScript en el contexto de la página
     * @param code Código JavaScript a ejecutar
     * @return true si se ejecutó correctamente
     */
    virtual bool executeJavaScript(const std::string& code) = 0;

    /**
     * @brief Ejecuta JavaScript y obtiene el resultado
     * @param code Código JavaScript
     * @param result [out] Resultado de la ejecución (JSON string)
     * @return true si se ejecutó y obtuvo resultado correctamente
     */
    virtual bool executeJavaScriptWithResult(const std::string& code, std::string& result) = 0;

    // ============================================
    // DevTools y debugging
    // ============================================

    /**
     * @brief Abre las DevTools de Chrome
     */
    virtual void showDevTools() = 0;

    /**
     * @brief Cierra las DevTools
     */
    virtual void closeDevTools() = 0;

    /**
     * @brief Verifica si DevTools está abierto
     * @return true si está abierto
     */
    virtual bool isDevToolsOpen() const = 0;

    // ============================================
    // Configuración de callbacks
    // ============================================

    /**
     * @brief Establece el callback para carga completa
     */
    virtual void setOnLoadCompleteCallback(OnLoadCompleteCallback callback) = 0;

    /**
     * @brief Establece el callback para errores de carga
     */
    virtual void setOnLoadErrorCallback(OnLoadErrorCallback callback) = 0;

    /**
     * @brief Establece el callback para cambios de título
     */
    virtual void setOnTitleChangeCallback(OnTitleChangeCallback callback) = 0;

    /**
     * @brief Establece el callback para mensajes de consola
     */
    virtual void setOnConsoleMessageCallback(OnConsoleMessageCallback callback) = 0;

    // ============================================
    // Loop de eventos
    // ============================================

    /**
     * @brief Procesa los eventos del navegador (debe llamarse en el loop principal)
     */
    virtual void doMessageLoopWork() = 0;

    /**
     * @brief Ejecuta el loop de mensajes (bloqueante hasta que se cierre el navegador)
     * @return Código de salida
     */
    virtual int runMessageLoop() = 0;

    // ============================================
    // Información y estado
    // ============================================

    /**
     * @brief Obtiene la URL actual
     * @return URL actual o cadena vacía si no hay navegador
     */
    virtual std::string getCurrentURL() const = 0;

    /**
     * @brief Obtiene el título actual de la página
     * @return Título actual
     */
    virtual std::string getTitle() const = 0;

    /**
     * @brief Verifica si la página está cargando
     * @return true si está cargando
     */
    virtual bool isLoading() const = 0;

    /**
     * @brief Obtiene el progreso de carga (0.0 a 1.0)
     * @return Progreso de carga
     */
    virtual double getLoadProgress() const = 0;

    // ============================================
    // Configuración avanzada
    // ============================================

    /**
     * @brief Establece el zoom de la página
     * @param level Nivel de zoom (1.0 = 100%, 1.5 = 150%, etc.)
     */
    virtual void setZoomLevel(double level) = 0;

    /**
     * @brief Obtiene el nivel de zoom actual
     * @return Nivel de zoom
     */
    virtual double getZoomLevel() const = 0;

    /**
     * @brief Habilita/deshabilita el audio
     * @param muted true para silenciar
     */
    virtual void setAudioMuted(bool muted) = 0;

    /**
     * @brief Verifica si el audio está silenciado
     * @return true si está silenciado
     */
    virtual bool isAudioMuted() const = 0;

    /**
     * @brief Toma una captura de pantalla de la página
     * @param outputPath Ruta donde guardar la imagen (PNG)
     * @return true si se guardó correctamente
     */
    virtual bool takeScreenshot(const std::string& outputPath) = 0;

    /**
     * @brief Obtiene información de versión de CEF/Chromium
     * @return String con información de versión
     */
    virtual std::string getVersionInfo() const = 0;
};
