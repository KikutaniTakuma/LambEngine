#include "Thread.h"
#include "Engine/Engine.h"
#include "Utils/ExecutionLog.h"
#include "Error/Error.h"


uint32_t Lamb::Thread::currentThreadNum_ = 0;
uint32_t Lamb::Thread::kHardwareThread_  = std::thread::hardware_concurrency();

Lamb::Thread::~Thread() {
	exit_ = true;
	condition_.notify_all();
	if (thraed_.joinable()) {
		thraed_.join();
	}
}

void Lamb::Thread::Notify() {
	condition_.notify_all();
}

void Lamb::Thread::Create(
	const std::function<void(void)>& userProcess, 
	const std::function<bool(void)>& waitProcess, 
	const std::function<bool(void)>& restartProcess
) {
	if (kHardwareThread_ <= currentThreadNum_) {
		throw Lamb::Error::Code<Lamb::Thread>("over hardware thread number", ErrorPlace);
	}
	if (thraed_.joinable()) {
		throw Lamb::Error::Code<Lamb::Thread>("Already created", ErrorPlace);
	}

	userProcess_ = userProcess;
	waitProcess_ = waitProcess;
	restartProcess_ = restartProcess;

	threadProcess_ = [this]() {
		HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (SUCCEEDED(hr)) {
			Lamb::AddLog("CoInitializeEx succeeded");
		}
		else {
			throw Lamb::Error::Code<Lamb::Thread>("CoInitializeEx failed", ErrorPlace);
		}

		std::unique_lock<std::mutex> uniqueLock(mtx_);

		while (!exit_) {
			if (waitProcess_()) {
				isWait_ = true;
				condition_.wait(uniqueLock, [this]() { return restartProcess_() or exit_; });
			}
			if (exit_) {
				break;
			}
			isWait_ = false;

			userProcess_();
		}

		// COM 終了
		CoUninitialize();
		Lamb::AddLog("CoUninitialize succeeded");
	};

	thraed_ = std::move(std::thread(threadProcess_));
}
