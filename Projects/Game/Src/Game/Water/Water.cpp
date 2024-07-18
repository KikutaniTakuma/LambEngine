#include "Water.h"
#include "Utils/EngineInfo.h"
#include "imgui.h"
#include "Utils/SafeDelete.h"
#include "Engine/Graphics/PipelineObject/GaussianBlur/GaussianBlur.h"
#include "Utils/Random.h"

#include "Camera/Camera.h"

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
	transform.translate.y = -0.1f;
	transform.translate.z = 0.0f;
	transform.scale.x = 200.0f;
	transform.scale.y = 200.0f;
	transform.rotate.x = 1.57f;

	waterSurface_ = std::make_unique<WaterTex2D>();
	waterSurface_->Load();

	color_ = Vector4{ 0.1f, 0.25f, 0.5f, 1.0f }.GetColorRGBA();

	/*luminate_ = std::make_unique<PeraRender>();
	luminate_->Initialize("./Resources/Shaders/PostShader/PostLuminate.PS.hlsl");


	gaussianBlurObjectWidth_ = Lamb::MakeSafePtr<GaussianBlur>();
	try {
		gaussianBlurObjectWidth_->Init();
	}
	catch (const Lamb::Error& err) {
		gaussianBlurObjectWidth_.reset();
		throw err;
	}
	gaussianBlurWidth_ = std::make_unique<PeraRender>();
	gaussianBlurWidth_->Initialize(gaussianBlurObjectWidth_.get());

	gaussianBlurObjectHeight_ = Lamb::MakeSafePtr<GaussianBlur>();
	try {
		gaussianBlurObjectHeight_->Init();
	}
	catch (const Lamb::Error& err) {
		gaussianBlurObjectHeight_.reset();
		throw err;
	}
	gaussianBlurHeight_ = std::make_unique<PeraRender>();
	gaussianBlurHeight_->Initialize(gaussianBlurObjectHeight_.get());

	gaussianBlurObjectWidth_->SetGaussianState(
		GaussianBlur::GaussianBlurState{
			.dir = Vector2(1.0f, 0.0f),
			.sigma = 10.0f,
			.kernelSize = 8,
		}
	);
	gaussianBlurObjectHeight_->SetGaussianState(
		GaussianBlur::GaussianBlurState{
			.dir = Vector2(0.0f, 1.0f),
			.sigma = 10.0f,
			.kernelSize = 8,
		}
	);*/

	randomVec_ = Lamb::Random(Vector2::kZero, Vector2::kIdentity);

	waveData.ripplesPoint = transform.translate;
	waveData.waveStrength = 0.5f;
	waveData.ripples = 10.0f;
	waveData.waveSpeed = 10.0f;
	waveData.timeAttenuation = 0.1f;

	lightRotate_ = Vector3(-90.0f, 0.0f, 90.0f) * Lamb::Math::toRadian<float>;

	lightScale_ = 8.0f;
	light_ = Light{
			.ligDirection = Vector3::kXIdentity * Quaternion::EulerToQuaternion(lightRotate_),
			.ligColor = Vector3::kIdentity * lightScale_,
			.eyePos = Vector3::kZero,
			.shinness = 42.0f
	};

	density_ = 1.3f;
}

void Water::Update(const Vector3& cameraPos) {
	light_.eyePos = cameraPos;
	waterSurface_->SetLight(light_);

	randomVec_.x += 0.006f * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);
	randomVec_.y += 0.006f * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);

	waveData.time += Lamb::DeltaTime();
}

void Water::Draw(const Mat4x4& cameraMat, [[maybe_unused]]PeraRender* const pera) {
	/*std::vector renderTargets = {
		&luminate_->GetRender()
	};

	if (pera) {
		renderTargets.push_back(&pera->GetRender());
	}


	RenderTarget::SetMainAndRenderTargets(
		renderTargets.data(),
		static_cast<uint32_t>(renderTargets.size()),
		nullptr
	);*/

	waterSurface_->Draw(
		transform.GetMatrix(),
		cameraMat,
		randomVec_,
		density_,
		edgeDivision_,
		insideDivision_,
		waveData,
		color_,
		BlendType::kNone
	);
	/*waterSurface_->AllDraw(BlendType::kNone);

	luminate_->Draw(Pipeline::None, nullptr,  gaussianBlurWidth_.get());
	gaussianBlurWidth_->Draw(Pipeline::None, nullptr, gaussianBlurHeight_.get());
	gaussianBlurHeight_->Draw(Pipeline::Add, nullptr);*/
}

void Water::Debug([[maybe_unused]]const std::string& guiName){
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat("density", &density_, 0.01f);

	if (ImGui::TreeNode("Light")) {
		lightRotate_ *= Lamb::Math::toDegree<float>;
		ImGui::DragFloat3("lightDirection", lightRotate_.data(), 1.0f, -360.0f, 360.0f);
		lightRotate_ *= Lamb::Math::toRadian<float>;
		light_.ligDirection = Vector3::kXIdentity * Quaternion::EulerToQuaternion(lightRotate_);
		light_.ligDirection = light_.ligDirection.Normalize();
		light_.ligColor /= lightScale_;
		ImGui::ColorEdit3("ligColor", light_.ligColor.data());
		ImGui::DragFloat("ligColorScale", &lightScale_, 0.01f);
		light_.ligColor *= lightScale_;
		ImGui::DragFloat("shinness", &light_.shinness, 0.01f, 0.0f, 256.0f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("WaterSRT")) {
		ImGui::DragFloat3("pos", transform.translate.data(), 0.01f);
		ImGui::DragFloat3("scale", transform.scale.data(), 0.01f);
		ImGui::DragFloat3("rotate", transform.rotate.data(), 0.01f);
		ImGui::TreePop();
	}
	//gaussianBlurObjectWidth_->Debug("gaussianBlurObjectWidth");
	//gaussianBlurObjectHeight_->Debug("gaussianBlurObjectHeight");

	if (ImGui::TreeNode("ポリゴン分割数")) {
		ImGui::DragInt("edgeDivision", &edgeDivision_, 0.1f, 1, 64);
		ImGui::DragInt("insideDivision", &insideDivision_, 0.1f, 1, 64);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Wave")) {
		ImGui::DragFloat("波の高さm", &waveData.waveStrength, 0.01f);
		ImGui::DragFloat("波長", &waveData.ripples, 0.001f);
		ImGui::DragFloat("波の速度m/s", &waveData.waveSpeed, 0.001f);
		ImGui::DragFloat("時間s", &waveData.time, 0.01f);
		ImGui::DragFloat("時間減衰", &waveData.timeAttenuation, 0.01f);
		ImGui::DragFloat3("波源", waveData.ripplesPoint.data(), 0.01f);

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}
