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
    std::cout << "[Access] Starting loop" << std::endl;

    auto fp = m_deviceManager->service();
    FingerPrintRepository repo("repository/thor.db");

    std::vector<unsigned char> img;
    std::vector<unsigned char> tmpl;

    while (running) {
        if (!m_deviceManager->isInAccessMode()) {
            Sleep(200);
            continue;
        }

        if (!fp->acquireFingerprint(img, tmpl)) {
            Sleep(100);
            continue;
        }

        unsigned int fid = 0;
        unsigned int score = 0;

        if (!fp->identify(tmpl, fid, score)) {
            Sleep(500);
            continue;
        }

        try {
            FingerPrint fpData = repo.getById(static_cast<int>(fid));

            emitEventToUI("fingerprint_uid", fpData.uid);

            std::cout << "[Access] Match OK | fid=" << fid << " | score=" << score << "\n";

            m_deviceManager->switchToAccess();

            Sleep(1500);
        } catch (std::exception& e) {
            std::cout << "[Access] Exception : " << e.what() << std::endl;
            Sleep(1500);
        }
    }
    std::cout << "[Access] Background loop stopped\n";
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
