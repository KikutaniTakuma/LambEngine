#include "Utils/ExecutionLog/ExecutionLog.h"
#include <fstream>
#include <filesystem>
#include <cassert>
#include <chrono>
#include <format>
#include <Windows.h>
#undef max
#undef min

namespace Log {
	bool AddLog(const std::string& text) {
		static const std::filesystem::path fileName = "./Log/Execution.log";
		static bool isOpned = false;

		if (!std::filesystem::exists(fileName.parent_path())) {
			std::filesystem::create_directories(fileName.parent_path());
		}

		std::ofstream file;
		if (isOpned) {
			file.open(fileName, std::ios::app);
		}
		else {
			file.open(fileName);
		}
		if (file.fail()) {
			return false;
		}

		file << NowTime() << " : " << text;

		isOpned = true;

		return true;
	}

	void DebugLog(const std::string& text) {
		OutputDebugStringA(text.c_str());
	}

	std::string NowTime() {
		auto now = std::chrono::system_clock::now();
		auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
		std::chrono::zoned_time zt{ "Asia/Tokyo", nowSec };

		return std::format("{:%Y/%m/%d %H:%M:%S}", zt);
	}
}