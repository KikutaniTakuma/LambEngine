#include "Water.h"
#include "Utils/EngineInfo.h"
#include "imgui.h"
#include "Utils/SafeDelete.h"
#include "Engine/Graphics/PipelineObject/GaussianBlur/GaussianBlur.h"

Water* Water::instance_ = nullptr;

void Water::Initialize()
{
	if (!instance_) {
		instance_ = new Water{};
		instance_->Init();
	}
}

void Water::Finalize()
{
	Lamb::SafeDelete(instance_);
}

Water* const Water::GetInstance()
{
	return instance_;
}

void Water::Init() {
	waterPipelineObject_ = new WaterPipeline{};
	try {
		waterPipelineObject_->Init();
	}
	catch (const Lamb::Error& err) {
		delete waterPipelineObject_;
		throw err;
	}
	int32_t waterScale = 1;
	pera_ = std::make_unique<PeraRender>(1280 * waterScale,1280 * waterScale);
	pera_->Initialize(waterPipelineObject_);
	pos.y = -0.1f;
	pos.z = 0.0f;
	scale.x = 200.0f;
	scale.y = 200.0f;
	rotate.x = 1.57f;

	staticCamera_ = std::make_unique<Camera>();
	staticCamera_->pos.z = -10.0f;
	staticCamera_->Update();

	waterSurface_.reset(new Texture2D{});
	waterSurface_->Load();

	waterTransform_.scale = Lamb::ClientSize();
	waterTransform_.translate.z += 100.0f;
	color_ = Vector4{ 0.1f, 0.25f, 0.5f, 1.0f }.GetColorRGBA();

	luminate_ = std::make_unique<PeraRender>();
	luminate_->Initialize("./Resources/Shaders/PostShader/PostLuminate.PS.hlsl");


	gaussianBlurObjectWidth_ = new GaussianBlur{};
	try {
		gaussianBlurObjectWidth_->Init();
	}
	catch (const Lamb::Error& err) {
		delete gaussianBlurObjectWidth_;
		throw err;
	}
	gaussianBlurWidth_ = std::make_unique<PeraRender>();
	gaussianBlurWidth_->Initialize(gaussianBlurObjectWidth_);

	gaussianBlurObjectHeight_ = new GaussianBlur{};
	try {
		gaussianBlurObjectHeight_->Init();
	}
	catch (const Lamb::Error& err) {
		delete gaussianBlurObjectHeight_;
		throw err;
	}
	gaussianBlurHeight_ = std::make_unique<PeraRender>();
	gaussianBlurHeight_->Initialize(gaussianBlurObjectHeight_);

	gaussianBlurObjectWidth_->SetGaussianState(
		GaussianBlur::GaussianBlurState{
			.dir = Vector2(1.0f, 0.0f),
			.sigma = 10.0f,
			.kernelSize = 35,
		}
	);
	gaussianBlurObjectHeight_->SetGaussianState(
		GaussianBlur::GaussianBlurState{
			.dir = Vector2(0.0f, 1.0f),
			.sigma = 10.0f,
			.kernelSize = 35,
		}
	);
}

void Water::Update(const Vector3& cameraPos) {
	waterPipelineObject_->SetCameraPos(cameraPos);
	
	pera_->pos = pos;
	pera_->scale = scale;
	pera_->rotate = rotate;
	pera_->Update();

	luminate_->Update();
	gaussianBlurWidth_->Update();
	gaussianBlurHeight_->Update();
}

void Water::Draw(const Mat4x4& cameraMat, PeraRender* const pera) {
	pera_->PreDraw();
	/*waterSurface_->OnceDraw(
		waterTransform_.GetMatrix(),
		Mat4x4::kIdentity,
		staticCamera_->GetViewOthographics(), 
		0u,
		color_.GetColorRGBA(),
		BlendType::kUnenableDepthNone
	);*/
	pera_->Draw(cameraMat, Pipeline::None, pera, true);

	pera_->PreDraw();
	/*waterSurface_->OnceDraw(
		waterTransform_.GetMatrix(),
		Mat4x4::kIdentity,
		staticCamera_->GetViewOthographics(),
		0u,
		color_,
		BlendType::kUnenableDepthNone
	);*/
	pera_->Draw(cameraMat, Pipeline::None, luminate_.get());
	luminate_->Draw(staticCamera_->GetViewOthographics(), Pipeline::None, gaussianBlurWidth_.get());
	gaussianBlurWidth_->Draw(staticCamera_->GetViewOthographics(), Pipeline::None, gaussianBlurHeight_.get());
	gaussianBlurHeight_->Draw(staticCamera_->GetViewOthographics(), Pipeline::Add);
}

void Water::Debug([[maybe_unused]]const std::string& guiName){
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::ColorEdit4("color", color_.data());
	waterTransform_.Debug(guiName.c_str());
	if (ImGui::TreeNode("WaterSRT")) {
		ImGui::DragFloat3("pos", &pos.x, 0.01f);
		ImGui::DragFloat3("scale", &scale.x, 0.01f);
		ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
		ImGui::TreePop();
	}
	gaussianBlurObjectWidth_->Debug("gaussianBlurObjectWidth");
	gaussianBlurObjectHeight_->Debug("gaussianBlurObjectHeight");

	ImGui::End();
#endif // _DEBUG
}
