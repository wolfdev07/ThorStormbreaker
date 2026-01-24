#pragma once
#include <string>
#include <functional>

class FingerEnrollService {

public:
	virtual ~FingerEnrollService() = default;

	// Verify if the fingerprint sensor is available
	virtual bool isDeviceAvailable() = 0;

	// Start the fingerprint enrollment process
	virtual void enroll(
		const std::string& memberNumber,
		std::function<void(const std::string& event, const std::string& payload)> emit,
		std::function<void(bool success)> done
	) = 0;

	virtual void cancelEnroll() = 0;
};