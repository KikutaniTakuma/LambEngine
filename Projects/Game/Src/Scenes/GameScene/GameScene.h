#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easing/Easing.h"
#include "Game/SkyDome/SkyDome.h"
#include "Math/Matrix.h"

class GameScene : public BaseScene {
public:
	GameScene();
	GameScene(const GameScene&) = delete;
	GameScene(GameScene&&) = delete;
	~GameScene() = default;

	GameScene& operator=(const GameScene&) = delete;
	GameScene& operator=(GameScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	Matrix<float, 4, 4> mat;
	Matrix<float, 4, 4> mat2;

	Matrix<float, 4, 4> result;
	Mat4x4 result2;

	Matrix<float, 4, 4> inverse;
	Matrix<float, 4, 4> inverse2;

	Mat4x4 mat4x4;
	Mat4x4 mat4x4_2;

	Mat4x4 mat4x4inverse;
	Mat4x4 mat4x4inverse2;
};