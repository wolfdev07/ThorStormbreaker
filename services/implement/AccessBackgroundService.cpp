//
// Created by artur on 20/01/2026.
//
#include "AccessBackgroundService.h"
#include <thread>
#include <chrono>

using namespace std;

AccessBackgroundService::AccessBackgroundService(
    std::shared_ptr<WebViewUI> webView
) : webView(std::move(webView)) {}

void AccessBackgroundService::loop() {
    while (true) {
        // TODO: lógica real (scanner, polling, etc.)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void AccessBackgroundService::start() const {
    std::thread([this]() {
        loop();
    }).detach();
}
