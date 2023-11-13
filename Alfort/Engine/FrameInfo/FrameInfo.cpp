#include "FrameInfo.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "../externals/imgui/imgui.h"

#include <cmath>
#include <thread>
#include <Windows.h>
#undef max
#undef min
#include <fstream>
#include <string>
#include <format>
#include <algorithm>

FrameInfo::FrameInfo() :
#ifdef _DEBUG
	isDebugStopGame_(false),
	isOneFrameActive_(false),
	isFixedDeltaTime_(true),
#endif // _DEBUG
	frameStartTime_(),
	deltaTime_(0.0),
	fps_(0.0),
	maxFps_(0.0),
	minFps_(0.0),
	frameCount_(0),
	fpsLimit_(0.0),
	minTime_(),
	minCheckTime_(),
	gameSpeedSccale_(1.0),
	frameDatas_(),
	frameDataDuration_(1),
	frameDataDurationStartTime_{},
	avgProcDuration_{600llu}
{
	//画面情報構造体
	DEVMODE mode{};

	//現在の画面情報を取得
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	// リフレッシュレート取得
	fps_ = static_cast<float>(mode.dmDisplayFrequency);
	minFps_ = maxFps_ = fps_;
	deltaTime_ = 1.0f / fps_;

	auto nowTime = std::chrono::steady_clock::now();
	gameStartTime_ = nowTime;
	reference_ = nowTime;
	frameDataDurationStartTime_ = nowTime;

	maxFpsLimit_ = fps_;

	SetFpsLimit(fps_);
}

FrameInfo::~FrameInfo() {
	auto end = std::chrono::steady_clock::now();

	//画面情報構造体
	DEVMODE mode{};

	//現在の画面情報を取得
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	auto playtime =
		std::chrono::duration_cast<std::chrono::milliseconds>(end - gameStartTime_);

	maxFps_ = std::clamp(maxFps_, 0.0, static_cast<double>(mode.dmDisplayFrequency));
	minFps_ = std::clamp(minFps_, 0.0, static_cast<double>(mode.dmDisplayFrequency));

	double avgFps = 0.0;
	double size = static_cast<double>(frameDatas_.size());
	while (!frameDatas_.empty()) {
		avgFps += frameDatas_.front();

		frameDatas_.pop();
	}

	avgFps /= size;

	Log::AddLog(std::format("Average Fps : {:.2f}\n", avgFps));
	if (std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_) > std::chrono::seconds(1)) {
		Log::AddLog(std::format("Max Fps : {:.2f}\n", maxFps_));
		Log::AddLog(std::format("Min Fps : {:.2f}\n", minFps_));
	}
}

FrameInfo* const FrameInfo::GetInstance() {
	static FrameInfo instance;

	return &instance;
}

void FrameInfo::Start() {
	frameStartTime_ = std::chrono::steady_clock::now();
}

void FrameInfo::End() {
	static std::chrono::steady_clock::time_point end{};
	end = std::chrono::steady_clock::now();

	// 10^-6
	static constexpr double unitAdjustment = 0.000001;
	
	auto elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(end - reference_);

	if (elapsed < minCheckTime_) {
		while (std::chrono::steady_clock::now() - reference_ < minTime_) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(250));
		}
	}

	end = std::chrono::steady_clock::now();
	auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(end - frameStartTime_);

	deltaTime_ = static_cast<double>(frameTime.count()) * unitAdjustment;
	fps_ = 1.0 / deltaTime_;


	if (std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_) > std::chrono::seconds(1)) {
		maxFps_ = std::max(fps_, maxFps_);
		minFps_ = std::min(fps_, minFps_);
	}

	frameCount_++;
	auto nowTime = std::chrono::steady_clock::now();
	reference_ = nowTime;

	if (std::chrono::duration_cast<std::chrono::seconds>(frameDataDurationStartTime_ - nowTime) < frameDataDuration_) {
		frameDataDurationStartTime_ = nowTime;
		frameDatas_.push(fps_);

		if (avgProcDuration_ < frameDatas_.size()) {
			double avgFps = 0.0;
			double size = static_cast<double>(frameDatas_.size());
			while (!frameDatas_.empty()) {
				avgFps += frameDatas_.front();

				frameDatas_.pop();
			}

			avgFps /= size;
			frameDatas_.push(avgFps);
		}
	}
}

void FrameInfo::SetFpsLimit(double fpsLimit) {
	fpsLimit_ = std::clamp(fpsLimit, 10.0, maxFpsLimit_);
	           
	minTime_ = std::chrono::microseconds(uint64_t(1000000.0 / fpsLimit_));
	minCheckTime_ = std::chrono::microseconds(uint64_t(1000000.0 / (fpsLimit_ + (5.0f * (fpsLimit_ / 60.0f)))));
}

void FrameInfo::Debug() {
#ifdef _DEBUG
	static float fpsLimit = static_cast<float>(fpsLimit_);
	fpsLimit = static_cast<float>(fpsLimit_);

	ImGui::Begin("fps");
	ImGui::Text("Frame rate: %3.0lf fps", fps_);
	ImGui::Text("Delta Time: %.4lf", deltaTime_);
	ImGui::Text("Frame Count: %llu", frameCount_);
	ImGui::DragFloat("fps limit", &fpsLimit, 1.0f, 10.0f, 165.0f);
	fpsLimit_ = static_cast<double>(fpsLimit);
	SetFpsLimit(fpsLimit_);
	if (ImGui::TreeNode("DEBUG")) {
		ImGui::Checkbox("is Debug Stop", &isDebugStopGame_);
		if (isDebugStopGame_) {
			if (ImGui::Button("DEBUG frame next")) {
				isOneFrameActive_ = true;
			}
		}
		else {
			ImGui::Checkbox("is fixed deltaTime", &isFixedDeltaTime_);
		}
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}

void FrameInfo::SetGameSpeedScale(float gameSpeedSccale) {
	gameSpeedSccale = std::clamp(gameSpeedSccale, 0.0f, 10.0f);
	gameSpeedSccale_ = static_cast<double>(gameSpeedSccale);
}