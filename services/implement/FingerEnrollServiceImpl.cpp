#include "FingerEnrollServiceImpl.h"
#include "../../repository/Respository.h"

#include <thread>
#include <utility>
#include <windows.h>


FingerEnrollServiceImpl::FingerEnrollServiceImpl(std::shared_ptr<IFingerprintService> fp) : m_fp(std::move(fp)) {}

bool FingerEnrollServiceImpl::isDeviceAvailable() {
	if (!m_fp->initialize()) return false;
	return m_fp->getDeviceCount() > 0;
}

void FingerEnrollServiceImpl::enroll(
    const std::string& uid,
    std::function<void(const std::string&, const std::string&)> emit,
    const std::function<void(bool)> done
) {
    std::thread([=]() {

        emit("status", "initializing");

        if (!m_fp->initialize()) {
            emit("error", "sdk_init_failed");
            done(false);
            return;
        }

        if (m_fp->getDeviceCount() <= 0) {
            emit("error", "no_device");
            done(false);
            return;
        }

        if (!m_fp->openDevice(0)) {
            emit("error", "open_device_failed");
            done(false);
            return;
        }

        if (!m_fp->initDatabase()) {
            emit("error", "init_database_failed");
            m_fp->closeDevice();
            return;
        }

        std::vector<std::vector<unsigned char>> templates;

        emit("status", "enrolling");

        // === 3 fingerPrint ===
        for (int i = 0; i < 3; ++i) {
            m_fp->setLed(101, true);
            emit("status", "place_finger");

            std::vector<unsigned char> img, tmpl;
            bool captured = false;

            for (int retry = 0; retry < 150; ++retry) {
                if (m_fp->acquireFingerprint(img, tmpl)) {
                    templates.push_back(tmpl);
                    emit("capture", std::to_string(i + 1));
                    captured = true;
                    m_fp->setLed(101, false);
                    break;
                }
                Sleep(100);
            }

            if (!captured) {
                emit("error", "capture_timeout");
                m_fp->closeDevice();
                done(false);
                return;
            }

            Sleep(1000);
        }

        // === DBMerge (igual que Python) ===
        emit("status", "merging");

        std::vector<unsigned char> regTemplate;
        if (!m_fp->mergeTemplates(
            templates[0],
            templates[1],
            templates[2],
            regTemplate
        )) {
            emit("error", "merge_failed");
            m_fp->closeDevice();
            done(false);
            return;
        }

        emit("status", "merged");

        // === Persistencia SQLite ===
        try {
            const FingerPrintRepository repo("repository/thor.db");

            const FingerPrint fp = repo.saveFingerPrint(uid, regTemplate);

            // === Cargar en RAM del lector (DBAdd) ===
            if (!m_fp->addTemplate(fp.id, regTemplate)) {
                emit("error", "dbadd_failed");
                m_fp->closeDevice();
                done(false);
                return;
            }

            emit("completed", std::to_string(fp.id));
            m_fp->setLed(101, true);
            Sleep(1500);
            m_fp->setLed(101, false);
            done(true);
        }
        catch (const std::exception& e) {
            emit("error", e.what());
            m_fp->setLed(102, true);
            Sleep(1500);
            m_fp->setLed(102, false);
            done(false);
        }
        m_fp->closeDevice();

    }).detach();
}