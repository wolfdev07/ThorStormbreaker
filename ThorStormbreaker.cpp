// ThorStormbreaker.cpp: define the entry point app.
#include "ThorStormbreaker.h"
#include "WebViewUI.h"
#include "services/implement/FingerprintServiceImpl.h"
#include "services/implement/FingerEnrollServiceImpl.h"

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

using namespace std;

static void initUI() {
    try {
		// Create webview instance
        WebViewUI ui;
		// Load the UI from local server
        ui.loadUrl("http://localhost:8000");
        ui.run();
    }
    catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << endl;
    }
}

static void enrollUI() {
    std::cout << "=====================================\n";
    std::cout << " ThorStormbreaker - Enroll Console\n";
    std::cout << "=====================================\n\n";

    std::string uid;
    std::cout << "Ingrese UID del usuario: ";
    std::getline(std::cin, uid);

    if (uid.empty()) {
        std::cerr << "UID invalido\n";
        return;
    }

    // Create SDK base service
    const std::shared_ptr<IFingerprintService> fingerprintService = std::make_shared<FingerprintServiceImpl>();

    // Create enroll service
    const std::shared_ptr<FingerEnrollService> enrollService = std::make_shared<FingerEnrollServiceImpl>(fingerprintService);

    if (!enrollService->isDeviceAvailable()) {
        std::cerr << "[ERROR] FingerprintService not available" << std::endl;
        return;
    }

    bool finished = false;

    enrollService->enroll(
        uid,
        [](const std::string& event, const std::string& payload) {
            std::cout << "[EVENT] " << event;
            if (!payload.empty())
                std::cout << " -> " << payload;
            std::cout << std::endl;
        },
        [&](const bool success) {
            std::cout << "\n[RESULT] "
                      << (success ? "ENROLL OK" : "ENROLL FAILED")
                      << std::endl;
            finished = true;
        }
    );

    // Esperar a que termine el thread interno
    while (!finished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    enrollUI();
    return 0;
}
