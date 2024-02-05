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
#include "Game/CollisionManager/Sphere/Sphere.h"

class PrimitiveScene : public BaseScene {
public:
	PrimitiveScene();
	PrimitiveScene(const PrimitiveScene&) = delete;
	PrimitiveScene(PrimitiveScene&&) = delete;
	~PrimitiveScene() = default;

	PrimitiveScene& operator=(const PrimitiveScene&) = delete;
	PrimitiveScene& operator=(PrimitiveScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	std::unique_ptr<Collider> collider_;
	std::unique_ptr<Sphere> sphere_;
};