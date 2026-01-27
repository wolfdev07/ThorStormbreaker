//
// Created by artur on 20/01/2026.
//
#pragma once
#include <memory>
#include <atomic>
#include <thread>
#include <string>

#include <WebViewUI.h>
#include "FingerprintDeviceManager.h"

class AccessBackgroundService {
public:
    explicit AccessBackgroundService(
        std::shared_ptr<WebViewUI> webView,
        std::shared_ptr<FingerprintDeviceManager> manager
        );

    ~AccessBackgroundService();

    void start();
    void stop();

private:
    void loop();
    void emitEventToUI(const std::string& event, const std::string& payload) const;

private:
    std::shared_ptr<WebViewUI> m_webView;
    std::shared_ptr<FingerprintDeviceManager> m_deviceManager;

    std::atomic<bool> running{false};
    std::thread workerThread;

    int lastFid{-1};
};
