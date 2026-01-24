//
// Created by artur on 19/01/2026.
//
#pragma once
#include <memory>
#include <WebViewUI.h>

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
};
