#pragma once
#include "../FingerEnrollService.h"
#include "../IFingerprintService.h"
#include <memory>

class FingerEnrollServiceImpl : public FingerEnrollService {

public:
	explicit FingerEnrollServiceImpl(std::shared_ptr<IFingerprintService> fp);

	bool isDeviceAvailable() override;

	void enroll(
		const std::string& memberNumber,
		std::function<void(const std::string&, const std::string&)> emit,
		std::function<void(bool)> done
	) override;

private:
	std::shared_ptr<IFingerprintService> m_fp;
};
