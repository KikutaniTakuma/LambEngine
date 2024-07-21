#pragma once

#include <memory>

#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"

#include "Game/Customize/Customize.h"
#include "Game/CustomizeCamera/CustomizeCamera.h"
#include "Game/FollowCamera/FollowCamera.h"

#include "Game/Minimap/Minimap.h"

class CustomizeScene : public BaseScene {
public:
	CustomizeScene();
	CustomizeScene(const CustomizeScene&) = delete;
	CustomizeScene(CustomizeScene&&) = delete;
	~CustomizeScene() = default;

	CustomizeScene& operator=(const CustomizeScene&) = delete;
	CustomizeScene& operator=(CustomizeScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;
private:
	std::unique_ptr<Customize> customize_;
	std::unique_ptr<CustomizeCamera> customizeCamera_;

	std::unique_ptr<Minimap> miniMap_;
};