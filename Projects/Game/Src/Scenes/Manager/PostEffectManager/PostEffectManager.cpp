#include "PostEffectManager.h"
#include "Utils/EngineInfo/EngineInfo.h"

Lamb::SafePtr<PostEffectManager> PostEffectManager::instance_ = nullptr;

PostEffectManager::PostEffectManager():
	camera_(),
	pera_()
{
	camera_ = std::make_unique<Camera>();
	camera_->pos.z = -10.0f;
	camera_->Update();

	pera_ = std::make_unique<PeraRender>();
	pera_->Initialize("./Resources/Shaders/PostShader/PostVignetting.PS.hlsl");
	pera_->scale = Lamb::ClientSize();
}

void PostEffectManager::Initialize() {
	instance_.reset(new PostEffectManager());
}

void PostEffectManager::Finalize() {
	instance_.reset();
}


