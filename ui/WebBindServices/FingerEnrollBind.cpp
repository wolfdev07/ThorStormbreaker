//
// Created by artur on 17/01/2026.
//
#include "FingerEnrollBind.h"
#include "implement/FingerprintServiceImpl.h"
#include "implement/FingerEnrollServiceImpl.h"

#include <iostream>

using json = nlohmann::json;

FingerEnrollBind::FingerEnrollBind() {
    fingerprintService = std::make_shared<FingerprintServiceImpl>();
    enrollService = std::make_shared<FingerEnrollServiceImpl>(fingerprintService);

    std::cout << "[FingerEnrollBind] Initialized" << std::endl;
}

void FingerEnrollBind::fingerEnroll(const std::string &request, std::function<void(const std::string &)> resolve) {
    std::cout << "[Thor] Payload request: " << request << std::endl;
    try {
        json args = json::parse(request);
        if (!args.is_array() || args.empty()) {
            throw std::invalid_argument("Payload recibido incorrecto");
        }
        // Convertir el payload
        const std::string payloadStr = args[0].get<std::string>();
        json payload = json::parse(payloadStr);

        const std::string memberNumber = payload.at("memberNumber").get<std::string>();
        std::cout << "[Thor] memberNumber = " << memberNumber << std::endl;

        // Validate divice
        if (!enrollService->isDeviceAvailable()) {
            resolve(R"({"success":false,"error":"device_not_available"})");
            return;
        }

        // Launch divice
        enrollService->enroll(
            memberNumber,

            [](const std::string& event, const std::string& payload) {
                std::cout << "[EVENT] " << event;
                if (!payload.empty())
                    std::cout << " -> " << payload;
                std::cout << std::endl;
            },

            [resolve](bool success) {
                resolve(success? R"({"success":true})" : R"({"success":false})");
            }
            );
    } catch (const std::exception &e) {
        std::cout << "[Thor] Error: " << e.what() << std::endl;
        resolve(R"({"success":false})");
    }
}
