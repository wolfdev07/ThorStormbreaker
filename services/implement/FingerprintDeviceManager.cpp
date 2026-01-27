//
// Created by artur on 24/01/2026.
//
#include "FingerprintDeviceManager.h"
#include <iostream>
#include <thread>

#include "FingerEnrollServiceImpl.h"
#include "Respository.h"

FingerprintDeviceManager::FingerprintDeviceManager(std::shared_ptr<IFingerprintService> fp): m_fp(std::move(fp)) {}

std::shared_ptr<IFingerprintService> FingerprintDeviceManager::service() {
    return m_fp;
}

/*
 * STATE
 */
bool FingerprintDeviceManager::isInAccessMode() const {
    return mode.load() == FingerprintMode::Access;
}
bool FingerprintDeviceManager::isInEnrollMode() const {
    return mode.load() == FingerprintMode::Enroll;
}
/*
 * DEVICE CONTROL
 */
bool FingerprintDeviceManager::openDevice() const{
    if (!m_fp->initialize()) return false;
    if (m_fp->getDeviceCount() <= 0) return false;
    if (!m_fp->openDevice(0)) return false;

    m_fp->freeDatabase();
    return m_fp->initDatabase();
}

void FingerprintDeviceManager::closeDevice() const{
    if (m_fp && m_fp->isDeviceOpen()) {
        m_fp->closeDevice();
    }
    m_fp->freeDatabase();
}
/*
 * ACCESS MODE
 */
bool FingerprintDeviceManager::switchToAccess() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (mode == FingerprintMode::Access) return true;

    std::cout << "[DeviceManager] Switching to ACCESS mode\n";

    stop();

    if (!openDevice()) {
        std::cerr << "[DeviceManager] Failed to open device for ACCESS\n";
        return false;
    }

    if (!reloadDatabase()) {
        std::cerr << "[DeviceManager] Failed to reload database\n";
        closeDevice();
        return false;
    }

    mode = FingerprintMode::Access;
    return true;
}
/*
 * ENROLL MODE
 */
bool FingerprintDeviceManager::switchToEnroll(
    const std::string& memberNumber,
    const std::function<void(const std::string&, const std::string&)>& emit,
    const std::function<void(bool)>& done
)  {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (mode == FingerprintMode::Enroll) return false;

    std::cout << "[DeviceManager] Switching to ENROLL mode\n";

    stop();

    if (!openDevice()) {
        done(false);
        return false;
    }

    currentEnrollService = std::make_shared<FingerEnrollServiceImpl>(m_fp);

    mode = FingerprintMode::Enroll;

    std::thread([this, svc = currentEnrollService, memberNumber, emit, done]() {
        svc->enroll(
            memberNumber,
            emit,
            [this, done](const bool success){
                {
                    std::lock_guard<std::mutex> lock_enroll(m_mutex);
                    currentEnrollService.reset();
                    mode = FingerprintMode::Idle;
                }
                closeDevice();
                std::cout << "[DeviceManager] Enroll device change to Access mode\n";
                switchToAccess();
                done(success);
            }
        );
    }).detach();

    return true;
}
/*
 * STOP / CANCEL
 */
void FingerprintDeviceManager::stop() {
    if (mode == FingerprintMode::Enroll && currentEnrollService) {
        currentEnrollService->cancelEnroll();
        currentEnrollService.reset();
    }

    closeDevice();
    mode = FingerprintMode::Idle;
}

void FingerprintDeviceManager::cancelEnroll() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (currentEnrollService) {
        currentEnrollService->cancelEnroll();
        currentEnrollService.reset();
    }

    closeDevice();
    mode = FingerprintMode::Idle;
}

/*
 *  SQLITE -> DEVICE
 */
bool FingerprintDeviceManager::reloadDatabase() const {
    FingerPrintRepository repo("repository/thor.db");
    const auto all = repo.getAll();

    for (const auto& fp : all) {
        if (!m_fp->addTemplate(fp.id, fp.templateData)) {
            std::cerr << "[DeviceManager] DBAdd failed fid=" << fp.id << "\n";
        }
    }

    std::cout << "[DeviceManager] Loaded " << all.size() << " fingerprints\n";
    return true;
}
