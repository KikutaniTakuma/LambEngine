#pragma once

#include "../Object.h"
#include "AudioManager/AudioManager.h"
class AudioComp : public IComp {
public:
	using IComp::IComp;

	~AudioComp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
#ifdef USE_DEBUG_CODE
	void Init() override;
#endif // USE_DEBUG_CODE

	void Finalize() override;

	void Load() override;

	void Debug(const std::string& guiName) override;

	Lamb::SafePtr<class Audio> GetAudio();
private:
	std::string fileName_;

	Lamb::SafePtr<class Audio> audio_;

public:
	float volume = 0.0f;
	bool isLoop = false;

#ifdef USE_DEBUG_CODE
	std::vector<std::filesystem::path> filePaths_;
#endif // USE_DEBUG_CODE
};
