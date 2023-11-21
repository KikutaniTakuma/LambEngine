#include "AudioManager.h"
#include "Engine/EngineUtils/ErrorCheck/ErrorCheck.h"
#include "Engine/Engine.h"
#include <cassert>
#include <filesystem>

AudioManager* AudioManager::instance = nullptr;
void AudioManager::Inititalize() {
	instance = new AudioManager();
	if (instance == nullptr) {
		ErrorCheck::GetInstance()->ErrorTextBox("Inititalize() : Instance failed", "AudioManager");
	}
}
void AudioManager::Finalize() {
	delete instance;
	instance = nullptr;
}

AudioManager::AudioManager() :
	xAudio2_(),
	masterVoice_(nullptr),
	audios_{},
	threadAudioBuff_{},
	load_{},
	mtx_{},
	isThreadLoadFinish_{false}
{
	HRESULT hr = XAudio2Create(xAudio2_.GetAddressOf(), 0u, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Constructor : XAudio2Create() failed", "AudioManager");
	}

	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Constructor : CreateMasteringVoicey() failed", "AudioManager");
	}
}
AudioManager::~AudioManager() {
	xAudio2_.Reset();
	if (load_.joinable()) {
		load_.join();
	}
}

Audio* AudioManager::LoadWav(const std::string& fileName, bool loopFlg) {
	if (!std::filesystem::exists(std::filesystem::path(fileName))) {
		ErrorCheck::GetInstance()->ErrorTextBox("LoadWav() Fialed : There is not this file -> " + fileName, "AudioManager");
	}

	if (audios_.empty()) {
		auto audio = std::make_unique<Audio>();
		audio->Load(fileName, loopFlg);
		audios_.insert({ fileName, std::move(audio) });
	}
	else {
		auto itr = audios_.find(fileName);

		if (itr == audios_.end()) {
			auto audio = std::make_unique<Audio>();
			audio->Load(fileName, loopFlg);
			audios_.insert({ fileName, std::move(audio) });
		}
	}

	return audios_[fileName].get();
}

void AudioManager::LoadWav(const std::string& fileName, bool loopFlg, class Audio** audio) {
	// コンテナに追加
	threadAudioBuff_.push({fileName, loopFlg, audio});
}

void AudioManager::ThreadLoad() {
	if (!threadAudioBuff_.empty() && !load_.joinable()) {
		auto loadProc = [this]() {
			std::lock_guard<std::mutex> lock(mtx_);
			while (!threadAudioBuff_.empty()) {
				if (Engine::IsFinalize()) {
					break;
				}

				auto& front = threadAudioBuff_.front();

				auto audio = audios_.find(front.fileName_);
				if (audio == audios_.end()) {;
					audios_.insert(std::make_pair(front.fileName_, std::make_unique<Audio>()));
					audios_[front.fileName_]->Load(front.fileName_, front.loopFlg_);
					*front.audio_ = audios_[front.fileName_].get();
				}
				else {
					*front.audio_ = audio->second.get();
				}

				threadAudioBuff_.pop();
			}

			isThreadLoadFinish_ = true;
			};

		load_ = std::thread{ loadProc };
	}
}

void AudioManager::JoinThread() {
	if (load_.joinable() && threadAudioBuff_.empty()) {
		load_.join();
	}
}

void AudioManager::CheckThreadLoadFinish() {
	if (isThreadLoadFinish_ && threadAudioBuff_.empty()) {
		JoinThread();
		isThreadLoadFinish_ = false;
	}
}