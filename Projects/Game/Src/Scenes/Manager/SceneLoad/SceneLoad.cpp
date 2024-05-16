#include "SceneLoad.h"
#include "Engine/Engine.h"
#include "./Camera/Camera.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Drawers/DrawerManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include <climits>

SceneLoad::Desc SceneLoad::setting = {};

SceneLoad::SceneLoad() :
	tex2Danimator_(),
	renderContextManager_()
{
	renderContextManager_ = RenderContextManager::GetInstance();

	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	loadTex_ = drawerManager->GetTexture2D();

	tex2Danimator_ = std::make_unique<Tex2DAniamtor>();

	tex2Danimator_->SetStartPos(Vector2::kZero);
	tex2Danimator_->SetDuration(setting.animationSpeed);
	tex2Danimator_->SetAnimationNumber(setting.animationNumber);
	tex2Danimator_->SetLoopAnimation(true);
	textureID_ = drawerManager->LoadTexture(setting.fileName);

	cameraMatrix_ = Camera::GetStaticViewOthographics();

	CreateLoad();
}

void SceneLoad::Start()
{
	if (!isLoad_) {
		isLoad_ = true;

		Engine::FrameEnd();

		tex2Danimator_->Start();
		renderContextManager_->SetIsNowThreading(isLoad_);

		thread_->Notify();
	}
}

void SceneLoad::Stop()
{
	if (isLoad_) {
		isLoad_ = false;
		while(!thread_->IsWait()){

		}

		tex2Danimator_->Stop();
		renderContextManager_->SetIsNowThreading(isLoad_);
		Engine::FrameStart();
	}
}

void SceneLoad::CreateLoad()
{
	thread_ = std::make_unique<Lamb::Thread>();
	thread_->Create(
		[this]()->void {
			Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();
			Engine::FrameStart();

			tex2Danimator_->Update();

			loadTex_->Draw(
				Mat4x4::MakeAffin(Vector3(Lamb::ClientSize(), 1.0f), Vector3::kZero, Vector3::kZIdentity * 1.0f),
				tex2Danimator_->GetUvMat4x4(),
				cameraMatrix_,
				textureID_,
				std::numeric_limits<uint32_t>::max(),
				BlendType::kNone
			);

			renderContextManager->Draw();

			Engine::FrameEnd();

			renderContextManager->ResetDrawCount();
		},
		[this]()->bool { return !isLoad_; },
		[this]()->bool { return isLoad_; }
	);
}
