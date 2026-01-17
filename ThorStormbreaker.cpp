// ThorStormbreaker.cpp: define el punto de entrada de la aplicación.
#include "ThorStormbreaker.h"
#include "WebViewUI.h"

#include "services/implement/FingerprintServiceImpl.h"
#include "services/implement/FingerEnrollServiceImpl.h"

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

static void initUI() {
    try {
		// Create webview instance
        WebViewUI ui;

		// Create servicios e inyectarlos al webview
        auto fpService = std::make_shared<FingerprintServiceImpl>();
        auto enrollService = std::make_shared<FingerEnrollServiceImpl>(fpService);

		// Load the UI from local server
        ui.loadUrl("http://localhost:8000");
        ui.run();
    }
    catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << endl;
    }
}

int main() {
    initUI();
    return 0;
}
