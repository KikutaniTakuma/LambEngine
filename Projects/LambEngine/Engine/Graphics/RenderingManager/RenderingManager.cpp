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

void RenderingManager::DrawRGB(const std::list<const RenderData*>& renderList) {
	std::array<RenderTarget*, 3> renderTargets;
	renderTargets[0] = &normalTexture_->GetRender();
	renderTargets[1] = &colorTexture_->GetRender();
	renderTargets[2] = &worldPositionTexture_->GetRender();

	RenderTarget::SetRenderTargets(renderTargets.data(), static_cast<uint32_t>(renderTargets.size()), &depthStencil_->GetDepthHandle());

	for (const auto& i : renderList) {
		i->Draw();
	}

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
