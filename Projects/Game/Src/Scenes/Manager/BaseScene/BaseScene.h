#pragma once
#include "Camera/Camera.h"

#include "Drawers/DrawerManager.h"
#include "AudioManager/AudioManager.h"
#include "Input/Input.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "Engine/Graphics/AnimationManager/AnimationManager.h"
#include "Engine/Graphics/RenderingManager/RenderingManager.h"

#include "Utils/SafePtr.h"
#include "Utils/Flg.h"

#include <memory>

class BaseScene {
	friend class SceneManager;

public:
	enum class ID {
		Title,
		Game,
		StageSelect,
		Result,

		Test,

		kNum
	};

	static std::array<std::string, static_cast<uint32_t>(ID::kNum)> kSceneStrings;

public:
	BaseScene(BaseScene::ID sceneID);
	BaseScene(const BaseScene&) = delete;
	BaseScene(BaseScene&&) = delete;
	virtual ~BaseScene() = default;

	BaseScene& operator=(const BaseScene&) = delete;
	BaseScene& operator=(BaseScene&&) = delete;

public:
	void SceneInitialize(class SceneManager* sceneManager);

	virtual void Load() = 0;

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void Update() = 0;
	virtual void Draw() = 0;

	void ChangeCamera();

	inline BaseScene::ID GetID() const {
		return sceneID_;
	}

	const class Camera& GetCamera() const;

protected:
	Lamb::SafePtr<class SceneManager> sceneManager_;

	Lamb::SafePtr<DrawerManager> drawerManager_;

	Lamb::SafePtr<AudioManager> audioManager_;

	Lamb::SafePtr<FrameInfo> frameInfo_;

	Lamb::SafePtr<Input> input_;

	Lamb::SafePtr<StringOutPutManager> stringOutPutManager_;

	BaseScene::ID sceneID_;

	Lamb::SafePtr<AnimationManager> animationManager_;

	Lamb::SafePtr<RenderingManager> renderingManager_;

private:
	std::unique_ptr<Camera> camera_;

protected:
	Lamb::SafePtr<Camera> currentCamera_;
};

template<class T>
concept IsBasedBaseScene = std::is_base_of_v<BaseScene, T>;