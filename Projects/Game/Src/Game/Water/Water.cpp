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

	waveData_.waveStrength = 0.25f;
	waveData_.ripples = 3.0f;
	waveData_.waveSpeed = 5.0f;
	waveData_.lengthAttenuation = 0.3f;
	waveData_.timeAttenuation = 0.1f;

	nextRipplePointLength_ = 0.2f;
	nextRipplePoint_ = 0.35f;

	density_ = 1.3f * 2.0f;
}


void Water::Update() {
	randomVec_.x += 0.006f * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);
	randomVec_.y += 0.006f * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);

	if (isActiveWave_) {
		time_ += Lamb::DeltaTime();
		waveData_.ripplesPoints[index_] = cameraPos_;
	}


	if (isActiveWave_ and nextRipplePoint_ < time_) {
		time_ = 0.0f;
		waveData_.time[index_] = 0.0f;
		isPoint_[index_] = true;
		index_++;

		if (WaterTex2D::kMaxRipplePoint <= index_) {
			index_ = 0;
		}
	}

	for (size_t index = 0;  bool i : isPoint_) {
		if (i) {
			waveData_.time[index] += Lamb::DeltaTime();
		}
		index++;
	}

}

void Water::Draw(const Mat4x4& cameraMat, [[maybe_unused]]PeraRender* const pera) {
	RenderingManager::GetInstance()->SetWaterMatrix(transform.CreateMatrix());

	waterSurface_->Draw(
		transform.CreateMatrix(),
		cameraMat,
		randomVec_,
		density_,
		cameraPos_,
		divisionMinLength,
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

	if (ImGui::TreeNode("WaterSRT")) {
		ImGui::DragFloat3("pos", transform.translate.data(), 0.01f);
		ImGui::DragFloat3("scale", transform.scale.data(), 0.01f);
		ImGui::DragFloat3("rotate", transform.rotate.data(), 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("ポリゴン分割数")) {
		ImGui::DragInt("edgeDivision", &edgeDivision_, 0.1f, 1, 64);
		ImGui::DragInt("insideDivision", &insideDivision_, 0.1f, 1, 64);
		ImGui::DragFloat("分割が最大になる距離", &divisionMinLength, 0.1f, 1, 1000);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Wave")) {
		ImGui::DragFloat("波の高さm", &waveData_.waveStrength, 0.01f);
		ImGui::DragFloat("波長", &waveData_.ripples, 0.001f);
		ImGui::DragFloat("波の速度m/s", &waveData_.waveSpeed, 0.001f);
		ImGui::DragFloat("時間s", &(time_), 0.01f);
		ImGui::DragFloat("次へ移る時間", &nextRipplePoint_, 0.001f, 0.0f, 1.0f);
		if (ImGui::TreeNode("波源の時間")) {
			for (int i = 0; i < WaterTex2D::kMaxRipplePoint; i++) {
				ImGui::DragFloat("時間s", &(waveData_.time[i]), 0.01f);
			}

			ImGui::TreePop();
		}
		ImGui::DragFloat("距離減衰係数", &waveData_.lengthAttenuation, 0.001f, 0.0, 10.0f);
		ImGui::DragFloat("時間減衰係数", &waveData_.timeAttenuation, 0.001f, 0.0f, 10.0f);
		ImGui::DragFloat("移動時に次の波源へ移る距離", &nextRipplePointLength_, 0.01f, 0.0f, 5.0f);
		ImGui::Checkbox("波有効", &debugIsActiveWave_);

		if (debugIsActiveWave_ and not isActiveWave_) {
			StartWave();
		}
		else if (not debugIsActiveWave_ and isActiveWave_) {
			StopWave();
		}


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

void Water::SetCameraPos(const float32_t3& pos) {
	if (nextRipplePointLength_ < (cameraPos_ - pos).Length() and isActiveWave_) {
		waveData_.ripplesPoints[index_] = pos;
		time_ = 0.0f;
		waveData_.time[index_] = 0.0f;
		isPoint_[index_] = true;
		index_++;

		if (WaterTex2D::kMaxRipplePoint <= index_) {
			index_ = 0;
		}
	}

	cameraPos_ = pos;
}

void Water::StartWave()
{
	if (not isActiveWave_) {
		isPoint_ = { false };
		time_ = 0.0f;
		index_ = 0;

		isActiveWave_ = true;
	}
}

void Water::StopWave()
{
	if (isActiveWave_) {
		time_ = 0.0f;
		index_ = 0;

		isPoint_ = { false };
		isActiveWave_ = false;
	}
}
