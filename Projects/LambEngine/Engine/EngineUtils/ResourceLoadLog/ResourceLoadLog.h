#pragma once
#include <unordered_map>
#include <filesystem>

class ResourceLoadLog {
private:
	ResourceLoadLog() = default;
	ResourceLoadLog(const ResourceLoadLog&) = delete;
	ResourceLoadLog(ResourceLoadLog&&) = delete;
	
public:
	~ResourceLoadLog();


public:
	static ResourceLoadLog* GetInstance();

	static void Set(const std::filesystem::path& path);


private:
	std::unordered_map<std::filesystem::path, int32_t> resoucePathData;
};