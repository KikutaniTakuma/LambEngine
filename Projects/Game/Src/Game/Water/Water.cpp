#include "Water.h"
#include "Utils/EngineInfo.h"
#include "imgui.h"
#include "Utils/SafeDelete.h"
#include "Engine/Graphics/PipelineObject/GaussianBlur/GaussianBlur.h"
#include "Utils/Random.h"

#include "Camera/Camera.h"

#ifdef USE_DEBUG_CODE
const std::array<std::string, static_cast<size_t>(Water::Version::kNum)> Water::kComboVersionString_ 
= {
	"kFirst",        // 何もしていない初期の水
	"kTransparency", // 透過させた
	"kDistortion",   // 水面下のオブジェクトを歪ませる
	"kCaustics"      // 水面下にコーティクス
};
#endif // USE_DEBUG_CODE

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
	transform.translate.z = 100.0f;
	transform.scale.x = 400.0f;
	transform.scale.y = 400.0f;
	transform.rotate.x = 1.57f;

	waterSurface_ = std::make_unique<WaterTex2D>();
	waterSurface_->Load();

	color_ = 0x16CEDA58;

	shaderData_.randomVec = Lamb::Random(Vector2::kZero, Vector2::kIdentity);

	waveData_.ripplesPoint = transform.translate;
	waveData_.waveStrength = 0.5f;
	waveData_.ripples = 10.0f;
	waveData_.waveSpeed = 10.0f;
	waveData_.timeAttenuation = 0.1f;

	shaderData_.density = 1.3f * 2.0f;
}

void Water::Update() {

	shaderData_.randomVec.x += 0.006f * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);
	shaderData_.randomVec.y += 0.006f * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);

	switch (currentVersion)
	{
	case Water::Version::kFirst:
		color_ = 0x16CEDAFF;
		shaderData_.effectState.isEnableDistortion = 0u;
		break;
	case Water::Version::kTransparency:
		color_ = 0x16CEDA58;
		shaderData_.effectState.isEnableDistortion = 0u;
		break;
	case Water::Version::kDistortion:
		color_ = 0x16CEDA58;
		shaderData_.effectState.isEnableDistortion = 1u;
		break;
	case Water::Version::kCaustics:
		color_ = 0x16CEDA58;
		shaderData_.effectState.isEnableDistortion = 1u;
		break;
	}

	shaderData_.waveData = waveData_;

	//waveData_.time += Lamb::DeltaTime();
}

void Water::Draw(const Mat4x4& cameraMat, [[maybe_unused]]PeraRender* const pera) {
	Transform drawTransform_ = transform;
	drawTransform_.scale *= 0.5f;
	drawTransform_.translate += drawTransform_.scale * 0.5f;

	waterSurface_->Draw(
		transform.GetMatrix(),
		cameraMat,
		shaderData_,
		color_.GetColorRGBA(),
		BlendType::kNormal
	);

	drawTransform_.translate -= drawTransform_.scale * 0.5f;
	waterSurface_->Draw(
		transform.GetMatrix(),
		cameraMat,
		shaderData_,
		color_.GetColorRGBA(),
		BlendType::kNormal
	);

	drawTransform_.translate.x += drawTransform_.scale.x * 0.5f;
	drawTransform_.translate.z -= drawTransform_.scale.z * 0.5f;
	waterSurface_->Draw(
		transform.GetMatrix(),
		cameraMat,
		shaderData_,
		color_.GetColorRGBA(),
		BlendType::kNormal
	);

	drawTransform_.translate.x -= drawTransform_.scale.x * 0.5f;
	drawTransform_.translate.z += drawTransform_.scale.z * 0.5f;
	waterSurface_->Draw(
		transform.GetMatrix(),
		cameraMat,
		shaderData_,
		color_.GetColorRGBA(),
		BlendType::kNormal
	);
}

void Water::Debug([[maybe_unused]]const std::string& guiName){
#ifdef USE_DEBUG_CODE
	ImGui::Begin(guiName.c_str());

	if (ImGui::TreeNode("バージョン変更")) {
		// コンボボックスを使ってenumの値を選択する
		if (ImGui::BeginCombo("BlendType", kComboVersionString_[static_cast<size_t>(currentVersion)].c_str()))
		{
			for (uint32_t count = 0; auto & i : kComboVersionString_)
			{
				bool isSelected = (static_cast<size_t>(currentVersion) == count);
				if (ImGui::Selectable(i.c_str(), isSelected))
				{
					currentVersion = static_cast<Version>(count);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
				count++;
			}
			ImGui::EndCombo();
		}

		ImGui::TreePop();
	}

	ImGui::DragFloat("density", &shaderData_.density, 0.01f);

	ImGui::ColorEdit4("color", color_.data());

	if (ImGui::TreeNode("WaterSRT")) {
		ImGui::DragFloat3("pos", transform.translate.data(), 0.01f);
		ImGui::DragFloat3("scale", transform.scale.data(), 0.01f);
		ImGui::DragFloat3("rotate", transform.rotate.data(), 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("ポリゴン分割数")) {
		ImGui::DragInt("edgeDivision", reinterpret_cast<int32_t*>(&shaderData_.edgeDivision), 0.1f, 1, 64);
		ImGui::DragInt("insideDivision", reinterpret_cast<int32_t*>(&shaderData_.insideDivision), 0.1f, 1, 64);
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
