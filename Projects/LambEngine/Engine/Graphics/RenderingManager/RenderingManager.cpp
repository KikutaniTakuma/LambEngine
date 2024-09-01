#include "RenderingManager.h"


#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/EngineInfo.h"

#include "Drawers/Line/Line.h"

#include "Engine/Graphics/PipelineObject/Luminate/Luminate.h"

#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "Engine/Core/ImGuiManager/ImGuiManager.h"
#include "Engine/Core/DescriptorHeap/RtvHeap.h"
#include "Utils/HSV.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


std::unique_ptr<RenderingManager> RenderingManager::instance_;

RenderingManager::RenderingManager() {
	this->deferredRendering_ = std::make_unique<DeferredRendering>();
	deferredRendering_->Init();


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

	deferredRendering_->SetColorHandle(colorTexture_->GetHandleGPU());
	deferredRendering_->SetNormalHandle(normalTexture_->GetHandleGPU());
	deferredRendering_->SetWoprldPositionHandle(worldPositionTexture_->GetHandleGPU());
	deferredRenderingData_.isDirectionLight = 1;
	deferredRenderingData_.environmentCoefficient = 0.2f;
	deferredRenderingData_.directionLight.shinness = 42.0f;
	deferredRenderingData_.directionLight.ligColor = Vector3::kIdentity;
	deferredRenderingData_.directionLight.ligDirection = Vector3::kXIdentity * Quaternion::EulerToQuaternion(Vector3(-90.0f, 0.0f, 90.0f) * Lamb::Math::toRadian<float>);

	depthStencil_ = std::make_unique<DepthBuffer>();
	srvHeap->BookingHeapPos(1u);
	srvHeap->CreateView(*depthStencil_);


	rgbaTexture_ = std::make_unique<PeraRender>();
	rgbaTexture_->Initialize("./Resources/Shaders/PostShader/PostNone2.PS.hlsl", {
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			DXGI_FORMAT_R32G32B32A32_FLOAT
		}
	);
	Vector4 rgba = rgbaTexture_->color;
	hsv_ = RGBToHSV({ rgba.color.r,rgba.color.g,rgba.color.b });

	std::unique_ptr<Luminate> luminate = std::make_unique<Luminate>();
	luminate->Init();
	luminate_ = luminate.release();
	luminanceThreshold = 1.4f;
	luminate_->SetLuminanceThreshold(luminanceThreshold);

	luminateTexture_ = std::make_unique<PeraRender>();
	luminateTexture_->Initialize(luminate_.get());
	gaussianHorizontalTexture_ = std::make_unique<PeraRender>();
	gaussianVerticalTexture_ = std::make_unique<PeraRender>();

	std::array<std::unique_ptr<GaussianBlur>, 2> gaussianPipeline = { std::make_unique<GaussianBlur>(), std::make_unique<GaussianBlur>() };
	gaussianPipeline[0]->Init();
	gaussianBlurStateHorizontal_ = GaussianBlur::GaussianBlurState{
		.dir = Vector2(1.0f, 0.0f),
		.sigma = 10.0f,
		.kernelSize = 8,
	};

	gaussianPipeline[0]->SetGaussianState(gaussianBlurStateHorizontal_);
	gaussianPipeline_[0] = gaussianPipeline[0].release();

	gaussianPipeline[1]->Init(
		"./Resources/Shaders/PostShader/Post.VS.hlsl",
		"./Resources/Shaders/PostShader/PostGaussian.PS.hlsl",
		{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB }
	);
	gaussianBlurStateVertical_ = GaussianBlur::GaussianBlurState{
			.dir = Vector2(0.0f, 1.0f),
			.sigma = 10.0f,
			.kernelSize = 8,
	};
	gaussianPipeline[1]->SetGaussianState(gaussianBlurStateVertical_);
	gaussianPipeline_[1] = gaussianPipeline[1].release();

	gaussianHorizontalTexture_->Initialize(gaussianPipeline_[0].get());
	gaussianVerticalTexture_->Initialize(gaussianPipeline_[1].get());

	std::unique_ptr<Outline> outline = std::make_unique<Outline>();
	outline->Init();
	outlinePipeline_ = outline.release();
	outlineTexture_ = std::make_unique<PeraRender>();
	outlineTexture_->Initialize(outlinePipeline_.get());
	outlinePipeline_->SetWeight(weight_);

	skyBox_ = std::make_unique<AirSkyBox>();
	skyBox_->Load();
	transform_.scale *= 500.0f;

	atmosphericTime_ = 6.0f;

	//deferredRendering_->SetEnvironmentHandle(skyBox_->GetHandle());

	SetTime(8.0f);
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

void RenderingManager::FrameStart()
{
	Lamb::SafePtr directXSwapChain = DirectXSwapChain::GetInstance();
	directXSwapChain->ChangeBackBufferState();
	RtvHeap::GetInstance()->SetMainRtv(&depthStencil_->GetDepthHandle());
	depthStencil_->Clear();
	directXSwapChain->ClearBackBuffer();

	// ビューポート
	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	directXSwapChain->SetViewPort(static_cast<int32_t>(clientSize.x), static_cast<int32_t>(clientSize.y));

	// SRV用のヒープ
	const Lamb::SafePtr cbvSrvUavDescriptorHeap = CbvSrvUavHeap::GetInstance();
	std::array heapPtrs = { cbvSrvUavDescriptorHeap->Get() };
	DescriptorHeap::SetHeaps(heapPtrs.size(), heapPtrs.data());
}

void RenderingManager::FrameEnd()
{
	ImGuiManager::GetInstance()->End();
	Lamb::SafePtr directXSwapChain = DirectXSwapChain::GetInstance();
	Lamb::SafePtr directXCommand = DirectXCommand::GetMainCommandlist();
	Lamb::SafePtr stringOutPutManager = StringOutPutManager::GetInstance();

	directXSwapChain->ChangeBackBufferState();

	// コマンドリストを確定させる
	directXCommand->CloseCommandlist();

	// GPUにコマンドリストの実行を行わせる
	directXCommand->ExecuteCommandLists();


	// GPUとOSに画面の交換を行うように通知する
	directXSwapChain->SwapChainPresent();

	stringOutPutManager->GmemoryCommit();

	directXCommand->WaitForFinishCommnadlist();

	directXCommand->ResetCommandlist();
}

void RenderingManager::Draw() {
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	deferredRenderingData_.directionLight.ligDirection = atmosphericParams_.lightDirection;

	/// ====================================================================================

	// 色、法線、ワールドポジション用レンダーターゲットをセット
	std::array<RenderTarget*, 3> renderTargets;
	renderTargets[0] = colorTexture_.get();
	renderTargets[1] = normalTexture_.get();
	renderTargets[2] = worldPositionTexture_.get();

	RenderTarget::ResourceStateChangeRenderTargets(
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

	/// ====================================================================================

	// 色だけ出力
	RenderTarget::SetRenderTargets(
		renderTargets.data(),
		static_cast<uint32_t>(1),
		&depthStencil_->GetDepthHandle()
	);
	// 深度値付きのlineを描画
	Line::AllDraw(true);

	skyBox_->SetAtmosphericParams(atmosphericParams_);

	DrawSkyBox();

	RenderDataLists rgbaList = {
		renderContextManager->CreateRenderList(BlendType::kNormal),
		renderContextManager->CreateRenderList(BlendType::kAdd),
		renderContextManager->CreateRenderList(BlendType::kSub),
		renderContextManager->CreateRenderList(BlendType::kMul)
	};

	/// ====================================================================================

	// ZSort(アルファ値付きなのでソート)
	ZSrot(rgbaList);

	// 色書き込み用のレンダーターゲットをセット
	std::array<RenderTarget*, 1> rgbaTextureRenderTarget = {
		&(rgbaTexture_->GetRender())
	};
	RenderTarget::ResourceStateChangeRenderTargets(
		rgbaTextureRenderTarget.data(),
		static_cast<uint32_t>(rgbaTextureRenderTarget.size())
	);
	RenderTarget::SetRenderTargets(
		rgbaTextureRenderTarget.data(),
		static_cast<uint32_t>(rgbaTextureRenderTarget.size()),
		&depthStencil_->GetDepthHandle()
	);
	RenderTarget::ClearRenderTargets(
		rgbaTextureRenderTarget.data(),
		static_cast<uint32_t>(rgbaTextureRenderTarget.size())
	);

	// リソースをテクスチャとして扱えるようにする
	RenderTarget::ResourceStateChangeRenderTargets(
		renderTargets.data(),
		static_cast<uint32_t>(renderTargets.size())
	);

	DrawDefferd();

	// Defferdでライティングした後に描画
	DrawRGBA(rgbaList);

	RenderTarget::ResourceStateChangeRenderTargets(
		rgbaTextureRenderTarget.data(),
		static_cast<uint32_t>(rgbaTextureRenderTarget.size())
	);


	/// ====================================================================================


	// メインと輝度抽出用のレンダーターゲットをセット
	std::array<RenderTarget*, 1> luminate = {
		&(luminateTexture_->GetRender())
	};
	RenderTarget::ResourceStateChangeRenderTargets(
		luminate.data(),
		static_cast<uint32_t>(luminate.size())
	);
	RenderTarget::SetMainAndRenderTargets(
		luminate.data(),
		static_cast<uint32_t>(luminate.size()),
		nullptr
	);
	RenderTarget::ClearRenderTargets(
		luminate.data(),
		static_cast<uint32_t>(luminate.size())
	);

	rgbaTexture_->Draw(Pipeline::None, nullptr);

	/// ====================================================================================

	DrawPostEffect();

	/// ====================================================================================

	RenderDataLists nodepthLists = {
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthNone),
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthNormal),
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthAdd),
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthSub),
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthMul)
	};

	ZSrot(nodepthLists);

	// UIの描画(depth書き込まないやつ)
	DrawNoDepth(nodepthLists);
	// 深度値なしのlineを描画
	Line::AllDraw(false);
}

DepthBuffer& RenderingManager::GetDepthBuffer()
{
	return *depthStencil_;
}

void RenderingManager::SetCameraPos(const Vector3& cameraPos) {
	deferredRenderingData_.eyePos = cameraPos;
	transform_.translate = cameraPos;
	atmosphericParams_.cameraPosition = cameraPos;
}

void RenderingManager::SetCameraMatrix(const Mat4x4& camera)
{
	cameraMatrix_ = camera;
}

void RenderingManager::SetProjectionInverseMatrix(const Mat4x4& projectionInverse)
{
	outlinePipeline_->SetProjectionInverse(projectionInverse);
}

void RenderingManager::SetHsv(const Vector3& hsv)
{
	hsv_ = hsv;
	Vector3 rgb = HSVToRGB(hsv_);
	Vector4 rgba = { rgb, 1.0f };
	rgbaTexture_->color = rgba.GetColorRGBA();
}

void RenderingManager::SetColor(const Vector4& color)
{
	hsv_ = RGBToHSV(color.GetVector3());
	rgbaTexture_->color = color.GetColorRGBA();
}

void RenderingManager::SetTime(float32_t time) {
	atmosphericTime_ = time;
	float32_t rotate = std::lerp(0.0f, 360.0f * Lamb::Math::toRadian<float>, std::fmodf(atmosphericTime_, 24.0f) / 24.0f);
	atmosphericParams_.lightDirection = -Vector3::kYIdentity * Quaternion::MakeRotateZAxis(rotate);
}

void RenderingManager::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	if(ImGui::TreeNode(guiName.c_str())){
		ImGui::Checkbox("lighting", reinterpret_cast<bool*>(&deferredRenderingData_.isDirectionLight));
		ImGui::DragFloat("environment", &deferredRenderingData_.environmentCoefficient, 0.001f, 0.0f, 5.0f);
		if (ImGui::TreeNode("hsv")) {
			ImGui::DragFloat("h", &hsv_.h, 0.1f, 0.0f, 360.0f);
			ImGui::DragFloat("s", &hsv_.s, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("v", &hsv_.v, 0.001f, 0.0f, 1.0f);
			Vector3 rgb = HSVToRGB(hsv_);
			Vector4 rgba = { rgb, 1.0f };
			ImGui::Text("%.4f, %.4f, %.4f", rgb.r, rgb.g, rgb.b);
			rgbaTexture_->color = rgba.GetColorRGBA();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Bloom")) {
			ImGui::DragInt("横カーネルサイズ", &gaussianBlurStateHorizontal_.kernelSize, 0.1f, 0, 128);
			ImGui::DragInt("縦カーネルサイズ", &gaussianBlurStateVertical_.kernelSize, 0.1f, 0, 128);
			ImGui::DragFloat("輝度しきい値", &luminanceThreshold, 0.001f, 0.0f, 2.0f);

			gaussianPipeline_[0]->SetGaussianState(gaussianBlurStateHorizontal_);
			gaussianPipeline_[1]->SetGaussianState(gaussianBlurStateVertical_);
			luminate_->SetLuminanceThreshold(luminanceThreshold);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("AtmosphericParams")) {
			ImGui::DragFloat("時間", &atmosphericTime_, 0.01f, 0.0f, 24.0f);
			float32_t rotate = std::lerp(0.0f, 360.0f * Lamb::Math::toRadian<float>, atmosphericTime_ / 24.0f);
			atmosphericParams_.lightDirection = -Vector3::kYIdentity * Quaternion::MakeRotateZAxis(rotate);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Outline")) {
			ImGui::DragFloat("アウトライン閾値", &weight_, 0.01f);
			outlinePipeline_->SetWeight(weight_);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
#endif // _DEBUG

}

void RenderingManager::DrawRGB(std::pair<size_t, const std::list<RenderData*>&> renderList) {
	for (size_t index = 0; const auto& element : renderList.second) {
		if (renderList.first <= index) {
			break;
		}
		element->DataSet();
		element->Draw();
		index++;
	}
}

void RenderingManager::DrawSkyBox() {
	skyBox_->Draw(transform_.GetMatrix(), cameraMatrix_, 0xffffffff);
}

void RenderingManager::DrawRGBA(const RenderDataLists& rgbaList) {
	for (auto& list : rgbaList) {
		for (size_t count = 0; auto& element : list.second) {
			if (list.first <= count) {
				break;
			}

			element->Draw();

			count++;
		}
	}
}

void RenderingManager::DrawDefferd() {
	deferredRendering_->SetDeferredRenderingData(deferredRenderingData_);
	deferredRendering_->SetAtmosphericParams(atmosphericParams_);

	deferredRendering_->Draw();
}

void RenderingManager::DrawPostEffect() {
	gaussianHorizontalTexture_->GetRender().SetThisRenderTarget(nullptr);
	luminateTexture_->ChangeResourceState();
	luminateTexture_->Draw(Pipeline::Blend::None, nullptr);

	gaussianHorizontalTexture_->ChangeResourceState();
	gaussianVerticalTexture_->GetRender().SetThisRenderTarget(nullptr);
	gaussianHorizontalTexture_->Draw(Pipeline::Blend::Add, nullptr);

	gaussianVerticalTexture_->ChangeResourceState();
	RenderTarget::SetMainAndRenderTargets(
		nullptr,
		0,
		nullptr
	);
	gaussianVerticalTexture_->Draw(Pipeline::Blend::Add, nullptr);

	outlinePipeline_->ChangeDepthBufferState();
	outlineTexture_->ChangeResourceState();
	RenderTarget::SetMainAndRenderTargets(
		nullptr,
		0,
		nullptr
	);
	outlineTexture_->Draw(Pipeline::Blend::Normal, nullptr);
	outlinePipeline_->ChangeDepthBufferState();
}

void RenderingManager::DrawNoDepth(const RenderDataLists& nodepthList)
{
	for (auto& list : nodepthList) {
		for (size_t count = 0; auto & element : list.second) {
			if (list.first <= count) {
				break;
			}

			element->SetLight(deferredRenderingData_.directionLight);
			element->SetCameraPos(deferredRenderingData_.eyePos);
			element->DataSet();
			element->Draw();

			count++;
		}
	}
}

void RenderingManager::ZSrot(const RenderDataLists& rgbaList) {
	for (auto& list : rgbaList) {
		for (size_t count = 0; auto& element : list.second) {
			if (list.first <= count) {
				break;
			}

			element->ZSort();
			element->SetLight(deferredRenderingData_.directionLight);
			element->SetCameraPos(deferredRenderingData_.eyePos);
			element->DataSet();

			count++;
		}
	}
}
