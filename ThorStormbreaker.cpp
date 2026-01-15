// ThorStormbreaker.cpp: define el punto de entrada de la aplicación.
#include "ThorStormbreaker.h"
#include "WebViewUI.h"
#include <iostream>

using namespace std;

int main() {
    cout << "============================================" << endl;
    cout << "ThorStormbreaker - Sistema de Huellas v1.0" << endl;
    cout << "============================================" << endl << endl;
    
    try {
        cout << "[*] Iniciando WebView..." << endl;
        
        // Crear instancia de WebView
        WebViewUI webview;
        
        // Cargar URL (puede ser localhost:8000 o HTML inline)
        cout << "[*] Cargando interfaz web..." << endl;
        
        // Opción 1: Cargar desde servidor local
        webview.loadUrl("http://localhost:8000");
        
        cout << "[✓] WebView iniciado correctamente" << endl;
        cout << "[i] Cierre la ventana para salir" << endl << endl;
        
        // Ejecutar event loop (bloqueante)
        webview.run();
        
        cout << endl << "[✓] Aplicación finalizada" << endl;
        
    } catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << endl;
        return -1;
    }
    
    return 0;
}
