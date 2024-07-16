#include "RenderingManager.h"

#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/EngineInfo.h"

std::unique_ptr<RenderingManager> RenderingManager::instance_;

RenderingManager::RenderingManager() {
	deferredRenderingPera_ = std::make_unique<PeraRender>();
	std::unique_ptr<DeferredRendering> deferredRendering = std::make_unique<DeferredRendering>();
	deferredRendering->Init();
	this->deferredRendering_ = deferredRendering.release();
	deferredRenderingPera_->Initialize(deferredRendering_.get());


	uint32_t width = uint32_t(Lamb::ClientSize().x);
	uint32_t height = uint32_t(Lamb::ClientSize().y);
	normalTexture_ = std::make_unique<RenderTarget>(width, height);
	colorTexture_ = std::make_unique<RenderTarget>(width, height);
	worldPositionTexture_ = std::make_unique<RenderTarget>(width, height);

	auto* const srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->BookingHeapPos(3u);
	srvHeap->CreateView(*colorTexture_);
	srvHeap->CreateView(*normalTexture_);
	srvHeap->CreateView(*worldPositionTexture_);

	deferredRendering_->SetNormalHandle(colorTexture_->GetHandleGPU());
	deferredRendering_->SetColorHandle(normalTexture_->GetHandleGPU());
	deferredRendering_->SetWoprldPositionHandle(worldPositionTexture_->GetHandleGPU());

	depthStencil_ = std::make_unique<DepthBuffer>();
	srvHeap->BookingHeapPos(1u);
	srvHeap->CreateView(*depthStencil_);


	luminateTexture_ = std::make_unique<PeraRender>();
	luminateTexture_->Initialize("./Resources/Shaders/PostShader/PostLuminate.PS.hlsl");
	gaussianHorizontalTexture_ = std::make_unique<PeraRender>();
	gaussianVerticalTexture_ = std::make_unique<PeraRender>();

	std::array<std::unique_ptr<GaussianBlur>, 2> gaussianPipeline = { std::make_unique<GaussianBlur>(), std::make_unique<GaussianBlur>() };
	for (size_t i = 0; i < gaussianPipeline.size(); i++) {
		gaussianPipeline[i]->Init();
		gaussianPipeline_[i] = gaussianPipeline[i].release();
	}
	gaussianHorizontalTexture_->Initialize(gaussianPipeline_[0].get());
	gaussianVerticalTexture_->Initialize(gaussianPipeline_[1].get());

	std::unique_ptr<Outline> outline = std::make_unique<Outline>();
	outline->Init();
	outlinePipeline_ = outline.release();
	outlineTexture_ = std::make_unique<PeraRender>();
	outlineTexture_->Initialize(outlinePipeline_.get());

}

RenderingManager::~RenderingManager()
{
	auto* const srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->ReleaseView(colorTexture_->GetHandleUINT());
	srvHeap->ReleaseView(normalTexture_->GetHandleUINT());
	srvHeap->ReleaseView(worldPositionTexture_->GetHandleUINT());
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

void RenderingManager::Draw() {
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	// 色、法線、ワールドポジション用レンダーターゲットをセット
	std::array<RenderTarget*, 3> renderTargets;
	renderTargets[0] = colorTexture_.get();
	renderTargets[1] = normalTexture_.get();
	renderTargets[2] = worldPositionTexture_.get();

	RenderTarget::ResourceStateChnageRenderTargets(
		renderTargets.data(), 
		static_cast<uint32_t>(renderTargets.size())
	);
	RenderTarget::SetRenderTargets(
		renderTargets.data(), 
		static_cast<uint32_t>(renderTargets.size()), 
		&depthStencil_->GetDepthHandle()
	);
	RenderTarget::ClearRenderTargets(
		renderTargets.data(), 
		static_cast<uint32_t>(renderTargets.size())
	);

	DrawRGB(renderContextManager->CreateRenderList(BlendType::kNone));

	// 色だけ出力
	RenderTarget::SetRenderTargets(
		&renderTargets[1],
		static_cast<uint32_t>(1),
		&depthStencil_->GetDepthHandle()
	);
	DrawSkyBox();

	// 同じく色だけ出力
	DrawRGBA();

	// 加算とか？
	// DrawParticle();

	// リソースをテクスチャとして扱えるようにする
	RenderTarget::ResourceStateChnageRenderTargets(
		renderTargets.data(),
		static_cast<uint32_t>(renderTargets.size())
	);
	// 自動でセットしてくれる
	DrawDefferd();


	DrawPostEffect();

	// UIの描画(depth書き込まないやつ)
	DrawUI();
}

void RenderingManager::DrawRGB(std::pair<size_t, const std::list<const RenderData*>&> renderList) {
	for (size_t index = 0; const auto& elemnt : renderList.second) {
		elemnt->Draw();
		index++;
		if (renderList.first <= index) {
			break;
		}
	}

}

void RenderingManager::DrawSkyBox() {
}

void RenderingManager::DrawRGBA() {
	
}

void RenderingManager::DrawParticle()
{
}

void RenderingManager::DrawDefferd() {
	deferredRendering_->SetDeferredRenderingData(deferredRenderingData_);

	deferredRenderingPera_->PreDraw(nullptr);
	deferredRenderingPera_->Draw(
		Pipeline::Blend::None,
		nullptr,
		nullptr
	);
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
