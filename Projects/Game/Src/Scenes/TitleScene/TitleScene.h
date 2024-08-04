#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Utils/Easeing.h"
#include "Game/Cursor/Cursor.h"

#include "Drawers/Other/WaterTex2D/WaterTex2D.h"

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
};