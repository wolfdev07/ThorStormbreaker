//
// Created by artur on 20/01/2026.
//
#include "AccessBackgroundService.h"
#include "Respository.h"

#include <iostream>
#include <windows.h>

#include "FingerprintDeviceManager.h"


AccessBackgroundService::AccessBackgroundService(
    std::shared_ptr<WebViewUI> webView,
    std::shared_ptr<FingerprintDeviceManager> manager
    )
: m_webView(std::move(webView)), m_deviceManager(std::move(manager)) {}


AccessBackgroundService::~AccessBackgroundService() {
    stop();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void AccessBackgroundService::start() {
    if (running) return;
    running = true;

    workerThread = std::thread(&AccessBackgroundService::loop, this);
}

void AccessBackgroundService::stop() {
    running = false;

    if (m_deviceManager) {
        m_deviceManager->stop();
    }
}

void AccessBackgroundService::loop() {
    std::cout << "[AccessBackgroundService] Starting loop" << std::endl;

    if (!m_deviceManager->switchToAccess()) {
        std::cerr << "[Access] Cannot switch device to access mode" << std::endl;
        return;
    }

    auto fp = m_deviceManager->service();

    if (!fp) {
        std::cerr << "[Access] Fingerprint service not available" << std::endl;
        return;
    }

    FingerPrintRepository repo("repository/thor.db");
    std::vector<unsigned char> img;
    std::vector<unsigned char> tmpl;

    while (running) {
        if (!fp->acquireFingerprint(img, tmpl)) {
            Sleep(100);
            continue;
        }

        unsigned int fid = 0;
        unsigned int score = 0;

        if (!fp->identify(tmpl, fid, score)) {
            Sleep(800);
            continue;
        }

        if (static_cast<int>(fid) == lastFid) {
            Sleep(800);
            continue;
        }

        lastFid = static_cast<int>(fid);

        try {
            FingerPrint fpData = repo.getById(lastFid);

            std::cout << "[Access] Match OK | fid=" << fid
                      << " | uid=" << fpData.uid
                      << " | score=" << score << std::endl;

            emitEventToUI("fingerprint_uid", fpData.uid);
        } catch (const std::exception& e) {
            std::cerr << "[Access] Repository error: " << e.what() << std::endl;
        }

        Sleep(2000);
    }

    m_deviceManager->stop();
    std::cout << "[AccessBackgroundService] Loop stopped" << std::endl;
}

void AccessBackgroundService::emitEventToUI(
    const std::string& event,
    const std::string& payload
    ) const {
    if (!m_webView) return;

    const std::string js =
        "window.dispatchEvent(new CustomEvent('fingerprint-access-event', {"
        "detail: {"
        "event: '" + event + "',"
        "payload: '" + payload + "'"
        "}"
        "}));";

    m_webView->dispatchToUI([js, webView = m_webView]() {
       webView->executeJS(js);
    });
}
