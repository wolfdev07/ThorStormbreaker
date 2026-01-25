#include "FingerEnrollServiceImpl.h"
#include "Respository.h"

#include <thread>
#include <utility>
#include <windows.h>


FingerEnrollServiceImpl::FingerEnrollServiceImpl(std::shared_ptr<IFingerprintService> fp) : m_fp(std::move(fp)) {}

bool FingerEnrollServiceImpl::isDeviceAvailable() {
	if (!m_fp->initialize()) return false;
	return m_fp->getDeviceCount() > 0;
}

void FingerEnrollServiceImpl::enroll(
    const std::string& memberNumber,
    const std::function<void(const std::string&, const std::string&)> emit,
    const std::function<void(bool)> done
) {
    cancelled = false;

    std::thread([this, emit, done, memberNumber](){

        emit("status", "initializing");

        std::vector<std::vector<unsigned char>> templates;

        emit("status", "enrolling");

        // === 3 fingerPrint ===
        for (int i = 0; i < 3; ++i) {
            m_fp->setLed(101, true);
            emit("status", "place_finger");

            std::vector<unsigned char> img, tmpl;
            bool captured = false;

            for (int retry = 0; retry < 150; ++retry) {

                if (cancelled) {
                    emit("cancelled", "user_cancelled");
                    done(false);
                    return;
                }

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
            done(false);
            return;
        }

        emit("status", "merged");

        // === Persistencia SQLite ===
        try {
            const FingerPrintRepository repo("repository/thor.db");

            FingerPrint fp;

            if (repo.existsByUid(memberNumber)) {
                fp = repo.replaceByUid(memberNumber, regTemplate);
            } else {
                fp = repo.saveFingerPrint(memberNumber, regTemplate);
            }

            // === Cargar en RAM del lector (DBAdd) ===
            if (!m_fp->addTemplate(fp.id, regTemplate)) {
                emit("error", "dbadd_failed");
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

    }).detach();
}

void FingerEnrollServiceImpl::cancelEnroll() {
    cancelled = true;

    if (m_fp) {
        m_fp->setLed(101, false);
        m_fp->setLed(102, false);
    }
}