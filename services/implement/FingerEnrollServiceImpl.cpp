#include "FingerEnrollServiceImpl.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <windows.h>

static std::string base64(const std::vector<unsigned char>& data) {
	static const char* tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string out;

	int val = 0, valb = -6;
	for (unsigned char c : data) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back(tbl[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}

	if (valb > -6) out.push_back(tbl[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}

FingerEnrollServiceImpl::FingerEnrollServiceImpl(
	std::shared_ptr<IFingerprintService> fp
) : m_fp(fp) {}

bool FingerEnrollServiceImpl::isDeviceAvailable() {
	if (!m_fp->initialize()) return false;
	if (m_fp->getDeviceCount() <= 0) return false;
	return true;
}

void FingerEnrollServiceImpl::enroll(
	const std::string& memberNumber,
	std::function<void(const std::string&, const std::string&)> emit,
	std::function<void(bool)> done
) {
	std::thread([=]() {
		std::vector<std::vector<unsigned char>> templates;

		emit("status", "starting");

		for (int i = 0; i < 6; ++i) {
			emit("status", "place_finger");

			std::vector<unsigned char> img, tmpl;
			int retries = 50;

			while (retries--) {
				if (m_fp->acquireFingerprint(img, tmpl)) {
					templates.push_back(tmpl);
					emit("capture", std::to_string(i + 1));
					break;
				}
				Sleep(100);
			}

			if (i > 0) {
				int score;
				m_fp->matchTemplates(templates[0], tmpl, score);
				emit("score", std::to_string(score));

				if (score < 95) {
					emit("error", "low_similarity");
					done(false);
					return;
				}
			}
			Sleep(1000);
		}

		// Simular guardado
		std::string b64 = base64(templates[0]);

		std::cout << "\n[ENROLL SUCCESS]\n";
		std::cout << "Member: " << memberNumber << "\n";
		std::cout << "Fingerprint (b64): " << b64.substr(0, 64) << "... \n";

		emit("completed", "ok");
		done(true);

	}).detach();
}