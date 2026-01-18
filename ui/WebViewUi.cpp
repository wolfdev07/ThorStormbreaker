#include "WebViewUI.h"
#include <iostream>

WebViewUI::WebViewUI() 
    : wv(true, nullptr) {  // Inicializar en lista de inicialización
    // Configure the webview
    wv.set_title("Forza (Thor Storm Breaker version 2.1.8)");
    wv.set_size(1280, 720, WEBVIEW_HINT_NONE);
}

void WebViewUI::loadUrl(const std::string& url) {
    // Load the specified URL into the webview
    wv.navigate(url);
    std::cout << "[WebView] Cargando URL: " << url << std::endl;
}

void WebViewUI::bind(const std::string& name, std::function<void(const std::string&, std::function<void(const std::string&)>)> fn) {
    // Bind a C++ function to JavaScript
    wv.bind(
        name,
        [this, fn](const std::string& id, const std::string& req, void* /*arg*/) {
			// Call the user-provided function with a resolver
            auto resolve = [this, id](const std::string& result) {
                wv.resolve(id, 0, result);
            };
            fn(req, resolve);
        },
        nullptr
    );

    std::cout << "[WebView] Binding creado: " << name << std::endl;
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