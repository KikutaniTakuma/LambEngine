#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easeing.h"
#include "Game/SkyDome/SkyDome.h"
#include "Utils/SafePtr.h"

#include "Drawers/SkyBox/SkyBox.h"
#include "Drawers/Other/WaterTex2D/WaterTex2D.h"

class TitleScene : public BaseScene {
public:
	TitleScene();
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	~TitleScene() = default;

	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;

public:
	void Load() override;
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	class Water* water_;

	Lamb::SafePtr<struct LevelData> levelData_;
};