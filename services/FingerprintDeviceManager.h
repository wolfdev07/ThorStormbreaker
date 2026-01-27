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

    std::shared_ptr<IFingerprintService> service();

    // Switch Mode
    bool switchToAccess();

    bool switchToEnroll(
        const std::string &memberNumber,
        const std::function<void(const std::string &, const std::string &)>& emit,
        const std::function<void(bool)>& done
    );

    // Stoppers
    void stop();
    void cancelEnroll();

    // State Queries
    bool isInAccessMode() const;
    bool isInEnrollMode() const;

private:
    bool openDevice() const;
    void closeDevice() const;
    bool reloadDatabase() const;

private:
    std::shared_ptr<IFingerprintService> m_fp;

    mutable std::mutex m_mutex;
    std::atomic<FingerprintMode> mode{FingerprintMode::Idle};

    std::shared_ptr<FingerEnrollService> currentEnrollService;
};