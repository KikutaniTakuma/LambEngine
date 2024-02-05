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

class DrawerScene : public BaseScene {
public:
	DrawerScene();
	DrawerScene(const DrawerScene&) = delete;
	DrawerScene(DrawerScene&&) = delete;
	~DrawerScene() = default;

	DrawerScene& operator=(const DrawerScene&) = delete;
	DrawerScene& operator=(DrawerScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	std::unique_ptr<Camera> camera2D_;

	std::unique_ptr<Model> model_;
	std::unique_ptr<Texture2D> tex2D_;
	std::unique_ptr<Particle> particle_;
};