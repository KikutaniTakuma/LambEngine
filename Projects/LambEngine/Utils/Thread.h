#pragma once
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>

namespace Lamb {
	class Thread {
	public:
		static uint32_t GetCurretnThreadNum() {
			return currentThreadNum_;
		}
		static uint32_t GetHardwareThread() {
			return kHardwareThread_;
		}

	private:
		static uint32_t currentThreadNum_;
		static uint32_t kHardwareThread_;

	public:
		Thread() = default;
		Thread(const Thread&) = delete;
		Thread(Thread&&) = delete;
		~Thread();

	public:
		Thread& operator=(const Thread&) = delete;
		Thread& operator=(Thread&&) = delete;

	public:
		void Notify();

		void Create(
			const std::function<void(void)>& userProcess,
			const std::function<bool(void)>& waitProcess,
			const std::function<bool(void)>& restartProcess
		);

		bool IsWait() const {
			return isWait_;
		}

	private:
		std::thread thraed_;
		std::mutex mtx_;
		std::condition_variable condition_;

		// threadで行う処理
		std::function<void(void)> threadProcess_;
		// threadProsess内で行う処理。
		std::function<void(void)> userProcess_;
		// waitに入るための関数(wait状態にするか否か)
		std::function<bool(void)> waitProcess_;
		// waitから起きるための関数(notify_allで起きるために必要)
		std::function<bool(void)> restartProcess_;

		bool exit_;
		bool isWait_;
	};
};