#include "AudioComp.h"
#include "Utils/FileUtils.h"

void AudioComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["fileName"] = fileName_;
	json["volume"] = volume;
	json["isLoop"] = isLoop;
}

void AudioComp::Load(nlohmann::json& json) {
	fileName_ = json["fileName"].get<std::string>();
	volume = json["volume"].get<float32_t>();
	isLoop = json["isLoop"].get<bool>();
}

#ifdef _DEBUG
void AudioComp::Init(){
	filePaths_ = Lamb::GetFilePathFormDir("./", ".mp3");
	auto wav = Lamb::GetFilePathFormDir("./", ".wav");
	filePaths_.insert(filePaths_.end(), wav.begin(), wav.end());
}
#endif // _DEBUG

void AudioComp::Finalize() {
	if (audio_.have()) {
		audio_->Stop();
	}
}

void AudioComp::Load() {
	AudioManager::GetInstance()->Load(fileName_);
	audio_ = AudioManager::GetInstance()->Get(fileName_);
}

void AudioComp::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::DragFloat("音量", &volume, 0.001f, 0.0f, 1.0f);
		ImGui::Checkbox("ループ", &isLoop);
		if (audio_.have()) {
			audio_->SetVolume(volume);
			
			if (ImGui::Button("Start")) {
				audio_->Start(volume, isLoop);
			}
			if (ImGui::Button("Stop")) {
				audio_->Stop();
			}
			if (ImGui::Button("ReStart")) {
				audio_->ReStart();
			}
			if (ImGui::Button("Pause")) {
				audio_->Pause();
			}
		}

        if (ImGui::Button("ファイルパス再読み込み")) {
            size_t size = filePaths_.size();
            filePaths_.clear();
            filePaths_.reserve(size);
			filePaths_ = Lamb::GetFilePathFormDir("./", ".mp3");
			auto wav = Lamb::GetFilePathFormDir("./", ".wav");
			filePaths_.insert(filePaths_.end(), wav.begin(), wav.end());
        }

        if (ImGui::TreeNode("テクスチャ読み込み")) {
            for (auto itr = filePaths_.begin(); itr != filePaths_.end(); itr++) {
                if (ImGui::Button(itr->string().c_str())) {
                    fileName_ = itr->string();
					AudioManager::GetInstance()->Load(fileName_);
					audio_ = AudioManager::GetInstance()->Get(fileName_);
                }
            }
            ImGui::TreePop();
        }

		ImGui::TreePop();
	}
#endif // _DEBUG

}

Lamb::SafePtr<Audio> AudioComp::GetAudio()
{
	return audio_;
}
