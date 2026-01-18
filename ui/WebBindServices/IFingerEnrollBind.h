//
// Created by artur on 17/01/2026.
//
#pragma once
#include <string>
#include <functional>

/**
 * @brief Contrato para handlers de enrollment de huella
 */
class IFingerEnrollBind {
public:
    virtual ~IFingerEnrollBind() = default;

    /**
     * @brief Maneja el payload recibido desde JS
     * @param request Payload crudo recibido por webview
     * @param resolve Callback para responder a JS
     */
    virtual void fingerEnroll(const std::string& request, std::function<void(const std::string&)> resolve) = 0;
};