#include "Water.h"
#include "Utils/EngineInfo.h"
#include "Utils/SafeDelete.h"
#include "Utils/Random.h"

#include "Utils/PerlinNoise.h"

#include "Engine/Graphics/RenderingManager/RenderingManager.h"

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#endif // USE_DEBUG_CODE


Water* Water::pInstance_ = nullptr;

void Water::Initialize()
{
	if (!pInstance_) {
		pInstance_ = new Water{};
		pInstance_->Init();
	}
}

void Water::Finalize()
{
	Lamb::SafeDelete(pInstance_);
}

Water* const Water::GetInstance()
{
	return pInstance_;
}

void Water::Init() {
	transform.translate.y = -0.1f;
	transform.translate.z = 100.0f;
	transform.scale.x = 400.0f;
	transform.scale.z = 400.0f;

	waterSurface_ = std::make_unique<WaterTex2D>();
	waterSurface_->Load();

	color_ = 0x16CEDAb4;

	randomVec_ = Lamb::Random(Vector2::kZero, Vector2::kIdentity);

	waveData_.ripplesPoint = transform.translate;
	waveData_.waveStrength = 0.5f;
	waveData_.ripples = 10.0f;
	waveData_.waveSpeed = 10.0f;
	waveData_.timeAttenuation = 0.1f;

	lightRotate_ = Vector3(-90.0f, 0.0f, 90.0f) * Lamb::Math::toRadian<float>;

	lightScale_ = 8.0f;
	light_ = Light{
			.ligDirection = Vector3::kXIdentity * Quaternion::EulerToQuaternion(lightRotate_),
			.ligColor = Vector3::kIdentity * lightScale_,
			.eyePos = Vector3::kZero,
			.shinness = 42.0f
	};

	density_ = 1.3f * 2.0f;
}

void Water::Update(const Vector3& cameraPos) {
	light_.eyePos = cameraPos;

	randomVec_.x += 0.006f * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);
	randomVec_.y += 0.006f * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);

	//waveData_.time += Lamb::DeltaTime();
}

void Water::Draw(const Mat4x4& cameraMat, [[maybe_unused]]PeraRender* const pera) {
	RenderingManager::GetInstance()->SetWaterMatrix(transform.CreateMatrix());

	waterSurface_->Draw(
		transform.CreateMatrix(),
		cameraMat,
		randomVec_,
		density_,
		edgeDivision_,
		insideDivision_,
		waveData_,
		color_.GetColorRGBA(),
		BlendType::kNormal
	);
}

void Water::Debug([[maybe_unused]]const std::string& guiName){
#ifdef USE_DEBUG_CODE
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat("density", &density_, 0.01f);

	ImGui::ColorEdit4("color", color_.data());

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

	if (ImGui::TreeNode("ポリゴン分割数")) {
		ImGui::DragInt("edgeDivision", &edgeDivision_, 0.1f, 1, 64);
		ImGui::DragInt("insideDivision", &insideDivision_, 0.1f, 1, 64);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Wave")) {
		ImGui::DragFloat("波の高さm", &waveData_.waveStrength, 0.01f);
		ImGui::DragFloat("波長", &waveData_.ripples, 0.001f);
		ImGui::DragFloat("波の速度m/s", &waveData_.waveSpeed, 0.001f);
		ImGui::DragFloat("時間s", &waveData_.time, 0.01f);
		ImGui::DragFloat("時間減衰", &waveData_.timeAttenuation, 0.01f);
		ImGui::DragFloat3("波源", waveData_.ripplesPoint.data(), 0.01f);

		ImGui::TreePop();
	}

	ImGui::End();
#endif // USE_DEBUG_CODE
}

void Water::SetWaveData(const WaterTex2D::WaveData& waveData)
{
	waveData_ = waveData;
}

float Water::CalcWaveHeight(float32_t2 uv) {
	return Lamb::CreateNoise(uv, randomVec_, density_);
}
