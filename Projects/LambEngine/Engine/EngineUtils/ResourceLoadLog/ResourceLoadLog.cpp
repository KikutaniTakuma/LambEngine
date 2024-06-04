#include "ResourceLoadLog.h"
#include "Utils/ConvertString.h"
#include <fstream>


ResourceLoadLog::~ResourceLoadLog() {
	std::ofstream file;
	file.open("./Log/LoadResrouce.log");

	for (auto& i : resoucePathData) {
		file << i.first.string() << std::endl;
	}

	file.close();
}

ResourceLoadLog* ResourceLoadLog::GetInstance()
{
	static ResourceLoadLog instance_;
	return &instance_;
}

void ResourceLoadLog::Set(const std::filesystem::path& path) {
	GetInstance()->resoucePathData[path] = 0;
}
