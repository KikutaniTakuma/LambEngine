#include "Utils/ExecutionLog/ExecutionLog.h"
#include <fstream>
#include <filesystem>
#include <cassert>
#include <chrono>

namespace Log {
	bool AddLog(const std::string& text) {
		static const std::filesystem::path fileName = "./Log/Execution.log";
		static bool isOpned = false;

#ifdef _DEBUG
		DebugLog(text);
#endif // _DEBUG


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

	void DebugLog(const std::string& text, const Vector2& vec) {
		OutputDebugStringA((text + std::string{ " : " } + std::format("{}, {}", vec.x, vec.y) + "\n").c_str());
	}

	void DebugLog(const std::string& text, const Vector3& vec) {
		OutputDebugStringA((text + std::string{ " : " } + std::format("{}, {}, {}", vec.x, vec.y, vec.z) + "\n").c_str());
	}

	void DebugLog(const std::string& text, const Vector4& vec) {
		OutputDebugStringA((text + std::string{ " : " } + std::format("{}, {}, {}, {}", vec.vec.x, vec.vec.y, vec.vec.z, vec.vec.w) + "\n").c_str());
	}

	std::string NowTime() {
		auto now = std::chrono::system_clock::now();
		auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
		std::chrono::zoned_time zt{ "Asia/Tokyo", nowSec };

		return std::format("{:%Y/%m/%d %H:%M:%S}", zt);
	}
}