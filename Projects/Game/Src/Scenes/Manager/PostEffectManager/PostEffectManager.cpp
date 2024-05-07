#include "PostEffectManager.h"
#include "Utils/EngineInfo/EngineInfo.h"

Lamb::SafePtr<PostEffectManager> PostEffectManager::instance_ = nullptr;

PostEffectManager::PostEffectManager():
	camera_(),
	gray_()
{
	camera_ = std::make_unique<Camera>();
	camera_->pos.z = -10.0f;
	camera_->Update();

	gray_ = std::make_unique<PeraRender>();
	gray_->Initialize("./Resources/Shaders/PostShader/PostGrayScale.PS.hlsl");
	gray_->scale = Lamb::ClientSize();
}

void PostEffectManager::Initialize() {
	instance_.reset(new PostEffectManager());
}

void PostEffectManager::Finalize() {
	instance_.reset();
}


