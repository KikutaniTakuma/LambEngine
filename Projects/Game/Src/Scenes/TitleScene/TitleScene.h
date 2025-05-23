#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawer/Model/Model.h"
#include "Drawer/Texture2D/Texture2D.h"
#include "Drawer/PeraRender/PeraRender.h"
#include "Drawer/Particle/Particle.h"
#include "Drawer/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Utils/Easeing.h"
#include "GameExternals/Cursor/Cursor.h"

#include "Drawer/Other/WaterTex2D/WaterTex2D.h"

#include "Game/Corals/Corals.h"
#include "Game/Cloud/Cloud.h"
#include "Game/Fishes/Fishes.h"
#include <array>

/// <summary>
/// タイトル
/// </summary>
class TitleScene : public BaseScene {
public:
	TitleScene();
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	~TitleScene() = default;

	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene &&) = delete;

public:
	void Load() override;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:
	std::unique_ptr<Camera> staticCamera_;
	std::unique_ptr<Tex2DInstance> title_;
	std::unique_ptr<Cursor> cursor_;

	class Water* water_;

	// 仮
	std::unique_ptr<Tex2DInstance> titleString_;

	// hud(仮)
	std::unique_ptr<Tex2DInstance> hud_;
	std::unique_ptr<Tex2DInstance> hudShadow_;

	// 点滅速度
	float blinkingSpeed_;
	// 点滅
	float blinking_;

	// 舟のモデル
	std::unique_ptr<ModelInstance> ship_;
	std::unique_ptr<ModelInstance> screw_;

	float screwRotate_;
	float screwRotateSpeed_;

	// 舟の動き
	float rotate_;
	float rotateSpeed_;
	float swaying_;
	float swayingSpeed_;

	Mat4x4 rotateMatrix_;

	Vector3 shipPos_;

	bool isSceneChange_;
	float sceneChangeRotate_;
	float sceneChangeRotateBasis_;
	float sceneChangeRotateSpeed_;

	std::unique_ptr<Easeing> easing_;
	std::unique_ptr<Easeing> volumeEasing_;

	class Audio* seaSE_ = nullptr;
	class Audio* moveShipSE_ = nullptr;

	std::pair<float, float> moveShipSEVolume_;

	WaterTex2D::WaveData waveData_;

	Corals corals_;

	std::unique_ptr<Cloud> cloud_;
	std::unique_ptr<Fishes> fishes_;

	Texture2D::Data tex2DData_;
};