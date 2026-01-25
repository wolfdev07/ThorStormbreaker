//
// Created by artur on 19/01/2026.
//
#pragma once
#include <memory>
#include <WebViewUI.h>
#include "AccessBackgroundService.h"
#include "FingerprintDeviceManager.h"

class WebViewUI;

class App {
public:
    App();
    void start();

private:
    void initUI();
    void initBinds();
    void initBackgroundServices();

    static bool checkDevices();

    std::shared_ptr<WebViewUI> webView;
    std::shared_ptr<FingerprintDeviceManager> deviceManager;
    std::shared_ptr<AccessBackgroundService> accessBackgroundService;
};
