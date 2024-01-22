#include "SceneLoad.h"
#include "Engine/Engine.h"
#include "Utils/Camera/Camera.h"
#include "Utils/EngineInfo/EngineInfo.h"

SceneLoad::Desc SceneLoad::setting = {};

SceneLoad::SceneLoad() :
	loadDrawThread_{},
	mtx_{},
	loadProc_{},
	loadTex_{},
	exit_{ false },
	isLoad_{ false }
{
	loadTex_.reset(new Texture2D{ setting.fileName });

	loadTex_->scale = Lamb::ClientSize();
	loadTex_->uvSize.x = 1.0f / static_cast<float>(setting.animationNumber);
	loadTex_->uvPibotSpd = 1.0f / static_cast<float>(setting.animationNumber);

	std::unique_ptr<Camera> camera{ new Camera{} };
	camera->Update();
	cameraMatrix_ = camera->GetViewOthographics();

		loadProc_ = [this]() {
		std::unique_lock<std::mutex> uniqueLock(mtx_);


		while (!exit_) {
			Engine::FrameStart();

			loadTex_->Animation(
				static_cast<size_t>(setting.animationSpeed),
				true,
				0.0f,
				static_cast<float>(setting.animationNumber)
			);

			loadTex_->Update();

			loadTex_->Draw(cameraMatrix_);

			Engine::FrameEnd();
		}

		Engine::FrameStart();
		};
}

SceneLoad::~SceneLoad()
{
	exit_ = true;
	if (loadDrawThread_.joinable()) {
		loadDrawThread_.join();
	}
}

void SceneLoad::Start()
{
	if (!isLoad_) {
		loadTex_->AnimationStart();
		isLoad_ = true;

		loadTex_->Update();
		loadTex_->Draw(cameraMatrix_);
		Engine::FrameEnd();
		CreateLoad();
	}
}

void SceneLoad::Stop()
{
	if (isLoad_) {
		isLoad_ = false;
		exit_ = true;
		loadDrawThread_.join();
	}
}

void SceneLoad::CreateLoad()
{
	loadDrawThread_ = std::thread{ loadProc_ };
}
