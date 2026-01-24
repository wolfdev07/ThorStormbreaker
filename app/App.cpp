//
// Created by artur on 19/01/2026.
//
#include <App.h>
#include <WebViewUI.h>
#include  "WebBindServices/FingerEnrollBind.h"
#include "implement/FingerEnrollServiceImpl.h"
#include "implement/FingerprintServiceImpl.h"
#include "AccessBackgroundService.h"
#include <iostream>

App::App(){}

void App::start() {
    initUI();

    if (!checkDevices()) {
        std::cerr << "[App] ZKTeco no disponible" << std::endl;
        webView->run();
        return;
    }

    initBinds();
    initBackgroundServices();
    webView->run();
}

void App::initUI() {
    webView = std::make_shared<WebViewUI>();
    webView->loadUrl("http://localhost:8000");
}

bool App::checkDevices() {
    const auto fingerprintService = std::make_shared<FingerprintServiceImpl>();
    FingerEnrollServiceImpl enrollService(fingerprintService);

    return enrollService.isDeviceAvailable();
}

void App::initBinds() {
    auto fingerEnrollBind = std::make_shared<FingerEnrollBind>(webView);

    webView->bind("fingerEnrollService", [fingerEnrollBind](const std::string& request, auto resolve) {
        fingerEnrollBind->fingerEnroll(request, resolve);
    });
};

void App::initBackgroundServices() {
    const auto accessService = std::make_shared<AccessBackgroundService>(webView);
    accessService->start();
}
