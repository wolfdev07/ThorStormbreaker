//
// Created by artur on 24/01/2026.
//
#pragma once
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>

#include "FingerEnrollService.h"
#include "IFingerprintService.h"

enum class FingerprintMode {
    Idle,
    Access,
    Enroll,
};

class FingerprintDeviceManager {
public:
    explicit FingerprintDeviceManager(std::shared_ptr<IFingerprintService> fp);

    bool switchToAccess();

    bool switchToEnroll(
        const std::string& memberNumber,
        std::function<void(const std::string&, const std::string&)> emit,
        std::function<void(bool)> done
        );

    void stop();

    void cancelEnroll();

    std::shared_ptr<IFingerprintService> service();

private:

    bool openDeviceLocked() const;

    bool loadDatabaseFromSQLite() const;

    std::shared_ptr<IFingerprintService> m_fp;

    std::mutex m_mutex;

    FingerprintMode mode{FingerprintMode::Idle};

    std::shared_ptr<FingerEnrollService> currentEnrollService;
};