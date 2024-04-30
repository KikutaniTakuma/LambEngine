#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easeing/Easeing.h"
#include "Game/SkyDome/SkyDome.h"
#include "Game/CollisionManager/Sphere/Sphere.h"
#include "Engine/Graphics/Tex2DAniamtor/Tex2DAniamtor.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Drawers/Other/WaterTex2D/WaterTex2D.h"
#include "Game/SkyBlock/SkyBlock.h"
#include "Game/Player/Player.h"

class TitleScene : public BaseScene {
public:
	TitleScene();
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	~TitleScene() = default;

	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	class Water* water_;
	std::unique_ptr<Sphere> sphere_;
	std::unique_ptr<WaterTex2D> watertsetUgoitekure_;
	std::unique_ptr<SkyBlock> skyBlock_;
	std::unique_ptr<Player> player_;

	Transform waterPos_;
	Vector3 random_;
	Vector4 color_;
};