//
// Created by artur on 17/01/2026.
//
#include "FingerEnrollBind.h"
#include <iostream>

using json = nlohmann::json;

FingerEnrollBind::FingerEnrollBind(
    std::shared_ptr<WebViewUI> webView,
    std::shared_ptr<FingerprintDeviceManager> deviceManager

    ): webView(std::move(webView))
    , deviceManager(std::move(deviceManager)) {

    std::cout << "[FingerEnrollBind] Initialized" << std::endl;
}

void FingerEnrollBind::fingerEnroll(const std::string &request, std::function<void(const std::string &)> resolve) {
    std::cout << "[Thor] Payload request: " << request << std::endl;

    try {
        json args = json::parse(request);
        if (!args.is_array() || args.empty()) {
            throw std::invalid_argument("Payload recibido incorrecto");
        }
        // Convert payload
        const std::string payloadStr = args[0].get<std::string>();
        json payload = json::parse(payloadStr);

        const std::string memberNumber = payload.at("memberNumber").get<std::string>();
        std::cout << "[Thor] memberNumber = " << memberNumber << std::endl;

        deviceManager->switchToEnroll(
            memberNumber,

            [this](const std::string& event, const std::string& data) {
                emitToJS(event, data);
            },

            [resolve](const bool success) {
                resolve(success
                            ?R"({"success":true})"
                            :R"({"success":false})"
                );
            }
        );

    } catch (const std::exception &e) {
        std::cerr << "[Thor] Error: " << e.what() << std::endl;
        emitToJS("error", e.what());
        resolve(R"({"success":false})");
    }
}

void FingerEnrollBind::emitToJS(const std::string &event, const std::string &payload) const {
    if (!webView) return;

    const std::string js =
        "window.dispatchEvent(new CustomEvent('fingerprint-enroll-event', { detail: {"
        "event: '" + event + "', payload: '" + payload + "'"
        "} }));";

    webView->dispatchToUI([js]() {
        WebViewUI::enqueueJS(js);
    });
}

void FingerEnrollBind::cancel() const {
    if (!deviceManager) return;

    std::cout << "[FingerEnrollBind] Cancel enroll requested" << std::endl;
    deviceManager->cancelEnroll();
}
