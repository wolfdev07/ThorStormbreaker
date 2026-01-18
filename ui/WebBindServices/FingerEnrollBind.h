//
// Created by artur on 17/01/2026.
//
#pragma once
#include "IFingerEnrollBind.h"
#include "IFingerprintService.h"
#include "FingerEnrollService.h"

#include <nlohmann/json.hpp>
#include <memory>
#include <functional>
#include <string>

class FingerEnrollBind : public IFingerEnrollBind {
public:
    FingerEnrollBind();

    void fingerEnroll(const std::string &request, std::function<void(const std::string &)> resolve) override;

private:
    std::shared_ptr<IFingerprintService> fingerprintService;
    std::shared_ptr<FingerEnrollService> enrollService;
};