#include "RenderingManager.h"

std::unique_ptr<RenderingManager> RenderingManager::instance_;

RenderingManager::RenderingManager()
{
}

RenderingManager::~RenderingManager()
{
}

void RenderingManager::Initialize(){
	instance_.reset(new RenderingManager());
}

void RenderingManager::Finalize(){
	instance_.reset();
}

Lamb::SafePtr<RenderingManager> const RenderingManager::GetInstance()
{
	return instance_.get();
}

void RenderingManager::Draw()
{
	DrawRGB();
	DrawSkyBox();
	DrawRGBA();
	DrawParticle();
	DrawPostEffect();
	DrawUI();
}

void RenderingManager::DrawRGB()
{
}

void RenderingManager::DrawSkyBox()
{
}

void RenderingManager::DrawRGBA()
{
}

void RenderingManager::DrawParticle()
{
}

void RenderingManager::DrawPostEffect()
{
}

void RenderingManager::DrawUI()
{
}

void RenderingManager::ZSrot()
{
}

void RenderingManager::SetMainRenderTarget()
{
}
