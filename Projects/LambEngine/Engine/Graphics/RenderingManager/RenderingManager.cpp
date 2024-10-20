#include "RenderingManager.h"


#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/EngineInfo.h"

#include "Drawer/Line/Line.h"

#include "Engine/Graphics/PipelineObject/Luminate/Luminate.h"

#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "Engine/Core/ImGuiManager/ImGuiManager.h"
#include "Engine/Core/DescriptorHeap/RtvHeap.h"
#include "Utils/HSV.h"


#include "Engine/Graphics/PipelineObject/Distortion/Distortion.h"

#include "Drawer/DrawerManager.h"

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#endif // USE_DEBUG_CODE


std::unique_ptr<RenderingManager> RenderingManager::instance_;

RenderingManager::RenderingManager() {
	resetDrawCount_ = 
		[](RenderDataList& list) {
			std::for_each(
				list.second.begin(),
				std::next(list.second.begin(), list.first),
				[](RenderData* element) {
					element->ResetDrawCount();
				}
			);
		};

	deferredRendering_ = std::make_unique<DeferredRendering>();
	deferredRendering_->Init();

	shadow_ = std::make_unique<ShadowRendering>();
	shadow_->Init();

	uint32_t width = uint32_t(Lamb::ClientSize().x);
	uint32_t height = uint32_t(Lamb::ClientSize().y);
	normalTexture_ = std::make_unique<RenderTarget>(width, height);
	colorTexture_ = std::make_unique<RenderTarget>(width, height);
	worldPositionTexture_ = std::make_unique<RenderTarget>(width, height);
	distortionTexture_ = std::make_unique<RenderTarget>(width, height);
	distortionTextureRGBA_ = std::make_unique<RenderTarget>(width, height);

	auto* const srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->BookingHeapPos(5u);
	srvHeap->CreateView(*colorTexture_);
	srvHeap->CreateView(*normalTexture_);
	srvHeap->CreateView(*worldPositionTexture_);
	srvHeap->CreateView(*distortionTexture_);
	srvHeap->CreateView(*distortionTextureRGBA_);

	deferredRendering_->SetColorHandle(colorTexture_->GetHandleGPU());
	deferredRendering_->SetNormalHandle(normalTexture_->GetHandleGPU());
	deferredRendering_->SetWoprldPositionHandle(worldPositionTexture_->GetHandleGPU());
	deferredRendering_->SetDistortionHandle(distortionTexture_->GetHandleGPU());
	deferredRenderingData_.isDirectionLight = 1;
	deferredRenderingData_.environmentCoefficient = 0.2f;
	deferredRenderingData_.directionLight.shinness = 42.0f;
	deferredRenderingData_.directionLight.ligColor = Vector3::kIdentity;
	deferredRenderingData_.directionLight.ligDirection = Vector3::kXIdentity * Quaternion::EulerToQuaternion(Vector3(-90.0f, 0.0f, 90.0f) * Lamb::Math::toRadian<float>);

	depthStencil_ = std::make_unique<DepthBuffer>();
	depthStencilShadow_ = std::make_unique<DepthBuffer>();
	srvHeap->BookingHeapPos(2u);
	srvHeap->CreateView(*depthStencil_);
	srvHeap->CreateView(*depthStencilShadow_);

	shadow_->SetDepthHandle(depthStencil_->GetHandleGPU());
	shadow_->SetDepthShadowHandle(depthStencilShadow_->GetHandleGPU());

	auto distortion = std::make_unique<Distortion>();
	distortion->Init();
	distortion->SetDistortionTexHandle(distortionTextureRGBA_->GetHandleGPU());

	rgbaTexture_ = std::make_unique<PeraRender>();
	rgbaTexture_->Initialize(distortion.release());
	Vector4 rgba = rgbaTexture_->color;
	hsv_ = RGBToHSV({ rgba.color.r,rgba.color.g,rgba.color.b });

	std::unique_ptr<Luminate> luminate = std::make_unique<Luminate>();
	luminate->Init();
	luminate_ = luminate.release();
	luminanceThreshold = 1.2f;
	luminate_->SetLuminanceThreshold(luminanceThreshold);

	luminateTexture_ = std::make_unique<PeraRender>();
	luminateTexture_->Initialize(luminate_.get());
	gaussianHorizontalTexture_ = std::make_unique<PeraRender>();
	gaussianVerticalTexture_ = std::make_unique<PeraRender>();

	std::array<std::unique_ptr<GaussianBlur>, GaussianIndex::kNum> gaussianPipeline = { std::make_unique<GaussianBlur>(), std::make_unique<GaussianBlur>() };
	gaussianPipeline[GaussianIndex::kHorizontal]->Init();
	gaussianBlurStateHorizontal_ = GaussianBlur::State{
		.dir = Vector2(1.0f, 0.0f),
		.sigma = 10.0f,
		.kernelSize = 8,
	};

	gaussianPipeline[GaussianIndex::kHorizontal]->SetGaussianState(gaussianBlurStateHorizontal_);
	gaussianPipeline_[GaussianIndex::kHorizontal] = gaussianPipeline[GaussianIndex::kHorizontal].release();

	gaussianPipeline[GaussianIndex::kVertical]->Init(
		"./Shaders/PostShader/Post.VS.hlsl",
		"./Shaders/PostShader/PostGaussian.PS.hlsl",
		{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB }
	);
	gaussianBlurStateVertical_ = GaussianBlur::State{
			.dir = Vector2(0.0f, 1.0f),
			.sigma = 10.0f,
			.kernelSize = 8,
	};
	gaussianPipeline[GaussianIndex::kVertical]->SetGaussianState(gaussianBlurStateVertical_);
	gaussianPipeline_[GaussianIndex::kVertical] = gaussianPipeline[GaussianIndex::kVertical].release();

	gaussianHorizontalTexture_->Initialize(gaussianPipeline_[GaussianIndex::kHorizontal].get());
	gaussianVerticalTexture_->Initialize(gaussianPipeline_[GaussianIndex::kVertical].get());

	std::unique_ptr<Outline> outline = std::make_unique<Outline>();
	outline->SetDepthBuffer(depthStencil_.get());
	outline->Init();
	outlinePipeline_ = outline.release();
	outlineTexture_ = std::make_unique<PeraRender>();
	outlineTexture_->Initialize(outlinePipeline_.get());
	outlinePipeline_->SetWeight(outlineWeight_);

	skyBox_ = std::make_unique<AirSkyBox>();
	skyBox_->Load();
	skyBoxTransform_.scale *= 500.0f;


	isUseMesh_ = Lamb::IsCanUseMeshShader();

}

RenderingManager::~RenderingManager()
{
	auto* const srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->ReleaseView(colorTexture_->GetHandleUINT());
	srvHeap->ReleaseView(normalTexture_->GetHandleUINT());
	srvHeap->ReleaseView(worldPositionTexture_->GetHandleUINT());
	srvHeap->ReleaseView(distortionTexture_->GetHandleUINT());
	srvHeap->ReleaseView(distortionTextureRGBA_->GetHandleUINT());
}

void RenderingManager::Initialize() {
	instance_.reset(new RenderingManager());
}

void RenderingManager::Finalize() {
	instance_.reset();
}

const Lamb::SafePtr<RenderingManager> RenderingManager::GetInstance()
{
	return instance_.get();
}

void RenderingManager::FrameStart()
{
	Lamb::SafePtr directXSwapChain = DirectXSwapChain::GetInstance();
	Lamb::SafePtr directXCommand = DirectXCommand::GetMainCommandlist();
	Lamb::SafePtr stringOutPutManager = StringOutPutManager::GetInstance();

	// 最初のフレームは通らない
	if (not isFirstFrame_) {
		ImGuiManager::GetInstance()->End();

		directXSwapChain->ChangeBackBufferState();

		// コマンドリストを確定させる
		directXCommand->CloseCommandlist();

		// GPUにコマンドリストの実行を行わせる
		directXCommand->ExecuteCommandList();

		// GPUとOSに画面の交換を行うように通知する
		directXSwapChain->SwapChainPresent();


		preBufferIndex_ = bufferIndex_++;
		if (DirectXSwapChain::kBackBufferNumber <= bufferIndex_) {
			bufferIndex_ = 0;
		}
	}
	
	ImGuiManager::GetInstance()->Start();

	// これからコマンドを積むインデックスをセット
	directXCommand->SetBufferIndex(bufferIndex_);

	directXSwapChain->ChangeBackBufferState();
	depthStencil_->Clear();
	depthStencilShadow_->Clear();
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
	Lamb::SafePtr directXSwapChain = DirectXSwapChain::GetInstance();
	Lamb::SafePtr directXCommand = DirectXCommand::GetMainCommandlist();
	Lamb::SafePtr stringOutPutManager = StringOutPutManager::GetInstance();

	// 描画コマンドを積む
	Draw();

	if (isFirstFrame_) {
		// 最初のコマンドを積み終わったのでfalse
		isFirstFrame_ = false;
	}
	else /* 最初のフレームは通らない */ {

		// 実行中のコマンドリストのインデックスをセットする
		directXCommand->SetBufferIndex(preBufferIndex_);

		stringOutPutManager->GmemoryCommit();

		// 実行待ち
		directXCommand->WaitForFinishCommnadlist();

		// リセット
		directXCommand->ResetCommandlist();
	}

	// コマンドを積んだインデックスをセットする
	directXCommand->SetBufferIndex(bufferIndex_);
}

void RenderingManager::Draw() {
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	deferredRenderingData_.directionLight.ligDirection = atmosphericParams_.lightDirection;
	gaussianPipeline_[GaussianIndex::kHorizontal]->SetGaussianState(gaussianBlurStateHorizontal_);
	gaussianPipeline_[GaussianIndex::kVertical]->SetGaussianState(gaussianBlurStateVertical_);
	luminate_->SetLuminanceThreshold(luminanceThreshold);
	outlinePipeline_->SetWeight(outlineWeight_);

	CalcLightCamera();
	deferredRendering_->SetCameraMatrix(viewMatrix_ * projectionMatrix_);
	deferredRendering_->SetLightCameraMatrix(lightCamera_);

	/// ====================================================================================

	// 色、歪み、法線、ワールドポジション用レンダーターゲットをセット
	std::array<RenderTarget*, 4> renderTargets;
	renderTargets[0] = colorTexture_.get();
	renderTargets[1] = distortionTexture_.get();
	renderTargets[2] = normalTexture_.get();
	renderTargets[3] = worldPositionTexture_.get();

	RenderTarget::ChangeToWriteResources(
		renderTargets.data(),
		static_cast<uint32_t>(renderTargets.size())
	);
	RenderTarget::SetRenderTargets(
		renderTargets.data(),
		static_cast<uint32_t>(renderTargets.size()),
		&depthStencil_->GetDepthHandle()
	);
	RenderTarget::Clear(
		renderTargets.data(),
		static_cast<uint32_t>(renderTargets.size())
	);

	RenderDataList rgbRenderList = renderContextManager->CreateRenderList(BlendType::kNone);

	DrawRGB(rgbRenderList);

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

	if (isDrawSkyBox_) {
		DrawSkyBox();
	}

	/// ====================================================================================
	
	RenderDataLists rgbaList = {
		renderContextManager->CreateRenderList(BlendType::kNormal),
		renderContextManager->CreateRenderList(BlendType::kAdd),
		renderContextManager->CreateRenderList(BlendType::kSub),
		renderContextManager->CreateRenderList(BlendType::kMul)
	};

	// ZSort(アルファ値付きなのでソート)
	ZSort(rgbaList);

	// 色書き込み用のレンダーターゲットをセット
	std::array<RenderTarget*, 2> rgbaTextureRenderTarget = {
		&(rgbaTexture_->GetRender()),
		(distortionTextureRGBA_.get()),
	};
	RenderTarget::ChangeToWriteResources(
		rgbaTextureRenderTarget.data(),
		static_cast<uint32_t>(rgbaTextureRenderTarget.size())
	);
	RenderTarget::SetRenderTargets(
		rgbaTextureRenderTarget.data(),
		static_cast<uint32_t>(rgbaTextureRenderTarget.size()),
		&depthStencil_->GetDepthHandle()
	);
	RenderTarget::Clear(
		rgbaTextureRenderTarget.data(),
		static_cast<uint32_t>(rgbaTextureRenderTarget.size())
	);

	// リソースをテクスチャとして扱えるようにする
	RenderTarget::ChangeToTextureResources(
		renderTargets.data(),
		static_cast<uint32_t>(renderTargets.size())
	);

	DrawDeferred();

	// Deferredでライティングした後に描画
	DrawRGBA(rgbaList);

	RenderTarget::ChangeToTextureResources(
		rgbaTextureRenderTarget.data(),
		static_cast<uint32_t>(rgbaTextureRenderTarget.size())
	);

	/// ===================================================================================

	// depthだけセット
	RenderTarget::SetRenderTargets(
		nullptr,
		0u,
		&depthStencilShadow_->GetDepthHandle()
	);

	DrawShadow(rgbRenderList);


	/// ====================================================================================


	// メインと輝度抽出用のレンダーターゲットをセット
	std::array<RenderTarget*, 1> luminate = {
		&(luminateTexture_->GetRender())
	};
	RenderTarget::ChangeResourceState(
		luminate.data(),
		static_cast<uint32_t>(luminate.size())
	);
	RenderTarget::SetMainAndRenderTargets(
		luminate.data(),
		static_cast<uint32_t>(luminate.size()),
		nullptr
	);
	RenderTarget::Clear(
		luminate.data(),
		static_cast<uint32_t>(luminate.size())
	);

	rgbaTexture_->Draw(Pipeline::None, nullptr);

	/// ====================================================================================

	// ポストエフェクトの描画
	DrawPostEffect();

	/// ====================================================================================

	RenderDataLists nodepthLists = {
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthNone),
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthNormal),
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthAdd),
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthSub),
		renderContextManager->CreateRenderList(BlendType::kUnenableDepthMul)
	};

	ZSort(nodepthLists);

	// UIの描画(depth書き込まないやつ)
	DrawNoDepth(nodepthLists);
	// 深度値なしのlineを描画
	Line::AllDraw(false);

	/// ====================================================================================

	// Drawカウントリセット
	resetDrawCount_(rgbRenderList);

	std::for_each(rgbaList.begin(), rgbaList.end(), resetDrawCount_);

	std::for_each(nodepthLists.begin(), nodepthLists.end(), resetDrawCount_);
}

void RenderingManager::CalcLightCamera() {
	Quaternion cameraRotate;
	Vector3 cameraScale, cameraTranslate;
	viewMatrix_.Inverse().Decompose(cameraScale, cameraRotate, cameraTranslate);
	Vector3 cameraDirection = Vector3::kZIdentity * cameraRotate;

	// いったんマジックナンバー
	cameraDirection *= 1000.0f * 0.1f;

	Quaternion lightRotate = Quaternion::EulerToQuaternion(lightRotate_);

	Vector3 lightBasePos = cameraTranslate + cameraDirection;
	Vector3 lightDirection = (kLightRotateBaseVector * lightRotate).Normalize();
	Vector3 lightPos = lightBasePos + (lightDirection * cameraDirection.Length());
	Quaternion viewRotate = Quaternion::DirectionToDirection(Vector3::kZIdentity, (lightBasePos - lightPos).Normalize());

	lightCamera_ = Mat4x4::MakeAffin(Vector3::kIdentity, viewRotate, lightPos) * Mat4x4::MakeOrthographic(160.0f, 90.0f, 0.1f, 1000.0f);
}

DepthBuffer* RenderingManager::GetDepthBuffer()
{
	return depthStencil_.get();
}

void RenderingManager::SetState(const State& state) {
	SetCameraPos(state.cameraPos);
	SetViewMatrix(state.viewMatrix);
	SetProjectionMatrix(state.projectionMatrix);
	SetHsv(state.hsv);
	SetIsLighting(state.isLighting);
	isUseMesh_ = state.isUseMeshShader;
	isDrawSkyBox_ = state.isDrawSkyBox;
	skyBoxTransform_.scale = state.skyBoxScale;
	isDrawOutLine_ = state.isDrawOutline;
	outlineWeight_ = state.outlineWeight;
	SetLightRotate(state.lightRotate);
	SetBloomKernelSize(state.bloomHorizontalSize, state.bloomVerticalSize);
	SetEnvironmentCoefficient(state.skyBoxEnvironmentCoefficient);
}

void RenderingManager::SetCameraPos(const Vector3& cameraPos) {
	deferredRenderingData_.eyePos = cameraPos;
	skyBoxTransform_.translate = cameraPos;
	atmosphericParams_.cameraPosition = cameraPos;
	atmosphericParams_.lightDirection = kLightRotateBaseVector * Quaternion::EulerToQuaternion(lightRotate_);
}

void RenderingManager::SetViewMatrix(const Mat4x4& view) {
	viewMatrix_ = view;
}

void RenderingManager::SetProjectionMatrix(const Mat4x4& projection) {
	projectionMatrix_ = projection;
	outlinePipeline_->SetProjectionInverse(projectionMatrix_.Inverse());
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

void RenderingManager::SetIsLighting(bool isLighting) {
	deferredRenderingData_.isDirectionLight = static_cast<uint32_t>(isLighting);
}

void RenderingManager::SetLightRotate(const Vector3& lightRotate) {
	lightRotate_ = lightRotate;
}

void RenderingManager::SetBloomKernelSize(int32_t x, int32_t y) {
	gaussianBlurStateHorizontal_.kernelSize = x;
	gaussianBlurStateVertical_.kernelSize = y;
}

void RenderingManager::SetEnvironmentCoefficient(float32_t environmentCoefficient) {
	deferredRenderingData_.environmentCoefficient = environmentCoefficient;
}

void RenderingManager::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef USE_DEBUG_CODE
	if (ImGui::TreeNode(guiName.c_str())) {
		if (Lamb::IsCanUseMeshShader()) {
			ImGui::Checkbox("MeshShader", &isUseMesh_);
		}

		ImGui::Checkbox("lighting", reinterpret_cast<bool*>(&deferredRenderingData_.isDirectionLight));
		ImGui::Checkbox("isShadow", reinterpret_cast<bool*>(&deferredRenderingData_.isShadow));
		lightRotate_ *= Lamb::Math::toDegree<float>;
		ImGui::DragFloat3("ライト角度", lightRotate_.data(), 1.0f);
		lightRotate_.x = std::fmodf(lightRotate_.x, 360.0f);
		lightRotate_.y = std::fmodf(lightRotate_.y, 360.0f);
		lightRotate_.z = std::fmodf(lightRotate_.z, 360.0f);
		lightRotate_ *= Lamb::Math::toRadian<float>;

		atmosphericParams_.lightDirection = kLightRotateBaseVector * Quaternion::EulerToQuaternion(lightRotate_);
		if (ImGui::TreeNode("hsv")) {
			ImGui::DragFloat("h", &hsv_.x, 0.1f, 0.0f, 360.0f);
			ImGui::DragFloat("s", &hsv_.y, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("v", &hsv_.z, 0.001f, 0.0f, 1.0f);
			Vector3 rgb = HSVToRGB(hsv_);
			Vector4 rgba = { rgb, 1.0f };
			ImGui::Text("%.4f, %.4f, %.4f", rgb.x, rgb.y, rgb.z);
			rgbaTexture_->color = rgba.GetColorRGBA();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Bloom")) {
			ImGui::DragInt("横カーネルサイズ", &gaussianBlurStateHorizontal_.kernelSize, 0.1f, 0, 128);
			ImGui::DragInt("縦カーネルサイズ", &gaussianBlurStateVertical_.kernelSize, 0.1f, 0, 128);
			ImGui::DragFloat("輝度しきい値", &luminanceThreshold, 0.001f, 0.0f, 2.0f);

			gaussianPipeline_[GaussianIndex::kHorizontal]->SetGaussianState(gaussianBlurStateHorizontal_);
			gaussianPipeline_[GaussianIndex::kVertical]->SetGaussianState(gaussianBlurStateVertical_);
			luminate_->SetLuminanceThreshold(luminanceThreshold);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("SkyBox")) {
			if (isDrawSkyBox_) {
				ImGui::DragFloat("environment", &deferredRenderingData_.environmentCoefficient, 0.001f, 0.0f, 5.0f);
			}
			ImGui::Checkbox("SkyBox描画", &isDrawSkyBox_);
			ImGui::DragFloat3("scale", skyBoxTransform_.scale.data(), 0.1f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Outline")) {
			ImGui::DragFloat("アウトラインしきい値", &outlineWeight_, 0.001f, 0.0f, 1000.0f);
			ImGui::Checkbox("アウトライン有効", &isDrawOutLine_);
			outlinePipeline_->SetWeight(outlineWeight_);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
#endif // USE_DEBUG_CODE

}

void RenderingManager::Save(nlohmann::json& jsonFile) {
	auto& json = jsonFile["RederingSetting"];

	json["isDirectionLight"] = static_cast<bool>(deferredRenderingData_.isDirectionLight);
	json["isShadow"] = static_cast<bool>(deferredRenderingData_.isShadow);
	json["lightRotate"] = nlohmann::json::array();
	for (auto& i : lightRotate_) {
		json["lightRotate"].push_back(i);
	}
	json["hsv"] = nlohmann::json::array();
	for (auto& i : hsv_) {
		json["hsv"].push_back(i);
	}
	json["bloom"]["x"] = gaussianBlurStateHorizontal_.kernelSize;
	json["bloom"]["y"] = gaussianBlurStateVertical_.kernelSize;
	json["bloom"]["luminanceThreshold"] = luminanceThreshold;
	json["skybox"]["scale"] = nlohmann::json::array();
	for (auto& i : skyBoxTransform_.scale) {
		json["skybox"]["scale"].push_back(i);
	}
	json["skybox"]["environmentCoefficient"] = deferredRenderingData_.environmentCoefficient;
	json["skybox"]["isDraw"] = isDrawSkyBox_;
	json["skybox"]["environment"] = deferredRenderingData_.environmentCoefficient;
	json["outline"] = outlineWeight_;
	json["outline_enable"] = isDrawOutLine_;
}

void RenderingManager::Load(nlohmann::json& jsonFile) {
	auto& json = jsonFile["RederingSetting"];
	deferredRenderingData_.isDirectionLight = json["isDirectionLight"].get<bool>();
	if (json.contains("isShadow")) {
		deferredRenderingData_.isShadow = json["isShadow"].get<bool>();
	}
	for (size_t i = 0; i < lightRotate_.size(); i++) {
		lightRotate_[i] = json["lightRotate"][i].get<float>();
	}

	for (size_t i = 0; i < hsv_.size(); i++) {
		hsv_[i] = json["hsv"][i].get<float>();
	}
	gaussianBlurStateHorizontal_.kernelSize = json["bloom"]["x"].get<int32_t>();
	gaussianBlurStateVertical_.kernelSize = json["bloom"]["y"].get<int32_t>();
	if (json["bloom"]["luminanceThreshold"].is_number_float()) {
		luminanceThreshold = json["bloom"]["luminanceThreshold"].get<float>();
	}
	else {
		luminanceThreshold = 1.0f;
	}
	for (size_t i = 0; i < skyBoxTransform_.scale.size(); i++) {
		skyBoxTransform_.scale[i] = json["skybox"]["scale"][i].get<float>();
	}
	if (json["skybox"]["environmentCoefficient"].is_number_float()) {
		deferredRenderingData_.environmentCoefficient = json["skybox"]["environmentCoefficient"].get<float>();
	}
	else {
		deferredRenderingData_.environmentCoefficient = 0.0f;
	}
	isDrawSkyBox_ = json["skybox"]["isDraw"].get<bool>();
	if (json["skybox"].contains("environment")) {
		deferredRenderingData_.environmentCoefficient = json["skybox"]["environment"].get<float>();
	}

	outlineWeight_ = json["outline"].get<float>();

	if (json.contains("outline_enable")) {
		isDrawOutLine_ = json["outline_enable"].get<bool>();
	}

}

void RenderingManager::SetIsUseMeshShader(bool isUseMesh) {
	if (Lamb::IsCanUseMeshShader()) {
		isUseMesh_ = isUseMesh;
	}
	else {
		isUseMesh_ = false;
	}
}

bool RenderingManager::GetIsUseMeshShader() const
{
	return isUseMesh_;
}

uint32_t RenderingManager::GetBufferIndex() const
{
	return bufferIndex_;
}

void RenderingManager::DrawRGB(const RenderDataList& renderList) {
	for (size_t index = 0; const auto & element : renderList.second) {
		if (renderList.first <= index) {
			break;
		}
		element->DataSet();
		element->Draw();
		index++;
	}
}

void RenderingManager::DrawSkyBox() {
	skyBox_->Draw(skyBoxTransform_.GetMatrix(), viewMatrix_ * projectionMatrix_, 0xffffffff);
}

void RenderingManager::DrawRGBA(const RenderDataLists& rgbaList) {
	for (auto& list : rgbaList) {
		for (size_t count = 0; auto & element : list.second) {
			if (list.first <= count) {
				break;
			}

			element->Draw();

			count++;
		}
	}
}

void RenderingManager::DrawDeferred() {
	deferredRendering_->SetDeferredRenderingData(deferredRenderingData_);
	deferredRendering_->SetAtmosphericParams(atmosphericParams_);

	deferredRendering_->Draw();
}

void RenderingManager::DrawShadow(const RenderDataList& rgbList)
{
	for (size_t index = 0; auto element : rgbList.second) {
		if (rgbList.first <= index) {
			break;
		}
		element->SetLightCameraMatrix(viewMatrix_ * projectionMatrix_);
		element->DrawShadow();
		index++;
	}
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

	//depthStencilShadow_->Barrier();
	depthStencil_->Barrier();
	//shadow_->Draw();

	if (isDrawOutLine_) {
		outlineTexture_->ChangeResourceState();
		RenderTarget::SetMainAndRenderTargets(
			nullptr,
			0,
			nullptr
		);
		outlineTexture_->Draw(Pipeline::Blend::Normal, nullptr);
	}
	depthStencil_->Barrier();
	//depthStencilShadow_->Barrier();
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

void RenderingManager::ZSort(const RenderDataLists& rgbaList) {
	for (auto& list : rgbaList) {
		for (size_t count = 0; auto & element : list.second) {
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
