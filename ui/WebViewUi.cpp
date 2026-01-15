#include "WebViewUI.h"
#include <iostream>

WebViewUI::WebViewUI() 
    : wv(true, nullptr) {  // Inicializar en lista de inicialización
    // Configure the webview
    wv.set_title("Forza (ThorStormbreaker version 2.1.8)");
    wv.set_size(1280, 720, WEBVIEW_HINT_NONE);
}

void WebViewUI::loadUrl(const std::string& url) {
    // Load the specified URL into the webview
    wv.navigate(url);
    std::cout << "[WebView] Cargando URL: " << url << std::endl;
}

void WebViewUI::loadHTML(const std::string& html) {
    // Load HTML content directly
    std::string dataUri = "data:text/html," + html;
    wv.navigate(dataUri);
    std::cout << "[WebView] Cargando HTML inline" << std::endl;
}

void WebViewUI::executeJS(const std::string& js) {
    // Execute JavaScript code
    wv.eval(js);
}

void WebViewUI::run() {
    // Run the webview event loop (blocking)
    std::cout << "[WebView] Iniciando event loop..." << std::endl;
    wv.run();
}

void WebViewUI::terminate() {
    // Terminate the webview
    wv.terminate();
    std::cout << "[WebView] Terminando..." << std::endl;
}