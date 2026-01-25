//
// Created by artur on 17/01/2026.
//
#pragma once
#include "IFingerEnrollBind.h"
#include "IFingerprintService.h"
#include "FingerEnrollService.h"
#include "WebViewUI.h"

#include <nlohmann/json.hpp>
#include <memory>
#include <functional>
#include <string>

#include "FingerprintDeviceManager.h"

class FingerEnrollBind : public IFingerEnrollBind {
public:
    FingerEnrollBind(
        std::shared_ptr<WebViewUI> webView,
        std::shared_ptr<FingerprintDeviceManager> deviceManager
    );

    void fingerEnroll( const std::string &request, std::function<void(const std::string &)> resolve) override;

    void cancel() const;

private:
    std::shared_ptr<WebViewUI> webView;
    std::shared_ptr<IFingerprintService> fingerprintService;
    std::shared_ptr<FingerprintDeviceManager> deviceManager;
    std::shared_ptr<FingerEnrollService> enrollService;

    void emitToJS(const std::string& event, const std::string& payload) const;
};
