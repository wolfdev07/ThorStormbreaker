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

class FingerEnrollBind : public IFingerEnrollBind {
public:
    explicit FingerEnrollBind(std::shared_ptr<WebViewUI> webView);

    void fingerEnroll(const std::string &request, std::function<void(const std::string &)> resolve) override;

    void cancel() const;

private:
    std::shared_ptr<WebViewUI> webView;
    std::shared_ptr<IFingerprintService> fingerprintService;
    std::shared_ptr<FingerEnrollService> enrollService;

    void emitToJS(const std::string& event, const std::string& payload) const;
};