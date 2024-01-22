#include "SceneLoad.h"
#include "Engine/Engine.h"
#include "Utils/Camera/Camera.h"
#include "Utils/EngineInfo/EngineInfo.h"

SceneLoad::SceneLoad():
	loadDrawThread_{},
	mtx_{},
	condtion_{},
	loadProc_{},
	loadTex_{},
	exit_{false},
	isLoad_{false}
{
	loadTex_.reset(new Texture2D{ "./Resources/Load.png" });

	loadTex_->scale = Lamb::ClientSize();
	loadTex_->uvSize.x = 0.25f;
	loadTex_->uvPibotSpd = 0.25f;

	loadProc_ = [this]() {
		std::unique_lock<std::mutex> uniqueLock(mtx_);

		std::unique_ptr<Camera> camera{ new Camera{} };
		camera->Update();

		while (!exit_) {
			Engine::FrameStart();

			loadTex_->Animation(500, true, 0.0f, 4.0f);

			loadTex_->Update();

			loadTex_->Draw(camera->GetViewOthographics());

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
