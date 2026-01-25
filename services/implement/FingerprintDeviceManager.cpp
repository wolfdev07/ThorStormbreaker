//
// Created by artur on 24/01/2026.
//
#include "FingerprintDeviceManager.h"
#include <iostream>
#include <thread>

#include "FingerEnrollServiceImpl.h"
#include "Respository.h"

FingerprintDeviceManager::FingerprintDeviceManager(std::shared_ptr<IFingerprintService> fp)
: m_fp(std::move(fp)) {}

std::shared_ptr<IFingerprintService> FingerprintDeviceManager::service() {
    return m_fp;
}

bool FingerprintDeviceManager::openDeviceLocked() const {
    if (!m_fp->initialize()) return false;
    if (m_fp->getDeviceCount() <= 0) return false;
    if (!m_fp->openDevice(0)) return false;
    return m_fp->initDatabase();
}

bool FingerprintDeviceManager::switchToAccess() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (mode == FingerprintMode::Access) return true;

    stop();
    std::cout << "[DeviceManager] Switching to ACCESS mode" << std::endl;

    if (!openDeviceLocked()) return false;

    if (!loadDatabaseFromSQLite()) {
        std::cerr << "[DeviceManager] Failed to load fingerprints" << std::endl;
        return false;
    }

    mode = FingerprintMode::Access;
    return true;
}

bool FingerprintDeviceManager::switchToEnroll(
    const std::string& memberNumber,
    std::function<void(const std::string&, const std::string&)> emit,
    std::function<void(bool)> done
    ) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (mode == FingerprintMode::Enroll) {
        return true;
    }

    stop();

    std::cout << "[DeviceManager] Switching to ENROLL mode" << std::endl;
    if (!openDeviceLocked()) {
        done(false);
        return false;
    }

    // Create enroll service
    currentEnrollService = std::make_shared<FingerEnrollServiceImpl>(m_fp);
    mode = FingerprintMode::Enroll;

    m_fp->clearDatabase();

    // Execute enroll in other thread
    std::thread([this, svc = currentEnrollService, memberNumber, emit, done]() {
        svc->enroll(
            memberNumber,
            emit,
            [this, done](const bool success) {
                {
                    std::lock_guard<std::mutex> lock_done(m_mutex);
                    currentEnrollService.reset();
                    mode = FingerprintMode::Enroll;
                }

                switchToAccess();
                done(success);
            }
            );
    }).detach();

    return true;
}

void FingerprintDeviceManager::stop() {
    if (m_fp && m_fp->isDeviceOpen()) {
        m_fp->closeDevice();
    }

    mode = FingerprintMode::Idle;
}

bool FingerprintDeviceManager::loadDatabaseFromSQLite() const {
    if (!m_fp) return false;

    FingerPrintRepository repo("repository/thor.db");

    if (!m_fp->initDatabase()) {
        std::cerr << "[DeviceManager] initDatabase failed" << std::endl;
        return false;
    }

    m_fp->initDatabase();

    const auto all = repo.getAll();

    for (const auto& fp : all) {
        if (!m_fp->addTemplate(fp.id, fp.templateData)) {
            std::cerr << "[DeviceManager] Failed to load fid=" << fp.id << std::endl;
        }
    }

    std::cout << "[DeviceManager] Loaded " << all.size() << " fingerprints into device cache" << std::endl;

    return true;
}

void FingerprintDeviceManager::cancelEnroll() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (currentEnrollService) {
        std::cout << "[DeviceManager] Cancel enroll" << std::endl;
        currentEnrollService->cancelEnroll();
        currentEnrollService.reset();
    }

    mode = FingerprintMode::Idle;
}
