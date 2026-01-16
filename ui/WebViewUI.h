#pragma once

#include "webview/webview.h"
#include <string>

/**
 * @brief Clase wrapper para webview que facilita su uso
 */
class WebViewUI {
public:
    /**
     * @brief Constructor - inicializa el webview
     */
    WebViewUI();

    /**
     * @brief Carga una URL en el webview
     * @param url URL a cargar (puede ser http://, https://, file://)
     */
    void loadUrl(const std::string& url);

    /**
	 * @brief Expone una función C++ al contexto JavaScript (binding)
	 * @param name Nombre de la función en JavaScript (window.<name>)
	 * @param fn Función C++ a ejecutar (firma: void(std::string req, std::function<void(std::string res)> resolve))
	 */
    void bind(const std::string& name, std::function<void(const std::string& req, std::function<void(const std::string& result)>)> fn);

    /**
     * @brief Carga contenido HTML directamente
     * @param html Código HTML a renderizar
     */
    void loadHTML(const std::string& html);

    /**
     * @brief Ejecuta código JavaScript en el contexto de la página
     * @param js Código JavaScript a ejecutar
     */
    void executeJS(const std::string& js);

    /**
     * @brief Inicia el event loop del webview (bloqueante)
     * Este método bloquea hasta que se cierra la ventana
     */
    void run();

    /**
     * @brief Termina el webview
     */
    void terminate();

    private:
        webview::webview wv;
};
