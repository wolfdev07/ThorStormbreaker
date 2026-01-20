#include "WebViewUI.h"
#include <iostream>

std::queue<std::string> jsQueue;
std::mutex jsMutex;

WebViewUI::WebViewUI() : wv(true, nullptr) {  // Inicializar en lista de inicialización
    // Configure the webview
    wv.set_title("Forza (Thor Storm Breaker version 2.1.8)");
    wv.set_size(1280, 720, WEBVIEW_HINT_NONE);

    wv.init(R"(
        window.__dispatch__ = function () {
            if (window.__cppDispatch) {
                window.__cppDispatch();
            }
        };
    )");
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

void WebViewUI::dispatchToUI(std::function<void()> fn) {
    // Administrador de tareas y subprocesos
    dispatcher.dispatch(std::move(fn));
}

void WebViewUI::run() {
    // Run the webview event loop (blocking)
    std::cout << "[WebView] Iniciando event loop..." << std::endl;

    wv.bind("__cppDispatch", [this](const std::string&, const std::string&, void*) {
        this->dispatch();
    }, nullptr);

    wv.run();
}

void WebViewUI::enqueueJS(std::string js) {
    std::lock_guard<std::mutex> lock(jsMutex);
    jsQueue.push(std::move(js));
}

void WebViewUI::dispatch() {
    std::queue<std::string> local;
    {
        std::lock_guard<std::mutex> lock(jsMutex);
        std::swap(local, jsQueue);
    }

    while (!local.empty()) {
        wv.eval(local.front());
        local.pop();
    }
}

void WebViewUI::terminate() {
    // Terminate the webview
    wv.terminate();
    std::cout << "[WebView] Terminando..." << std::endl;
}