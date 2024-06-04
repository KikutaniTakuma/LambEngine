#pragma once
#include "Utils/Thread.h"
#include <memory>
#include "Drawers/Texture2D/Texture2D.h"
#include "Engine/Graphics/Tex2DAniamtor/Tex2DAniamtor.h"
#include "Utils/SafePtr.h"


// ロード中の描画クラス
class SceneLoad {
public:
	struct Desc {
		std::string fileName = "./Resources/EngineResources/Load.png";
		uint32_t animationNumber = 4;
		float animationSpeed = 0.5f;
	};
public:
	static Desc setting;

public:
	SceneLoad();
	SceneLoad(const SceneLoad&) = delete;
	SceneLoad(SceneLoad&&) = delete;
	~SceneLoad() = default;

	SceneLoad& operator=(const SceneLoad&) = delete;
	SceneLoad& operator=(SceneLoad&&) = delete;

public:
	void Start();

	void Stop();

private:
	void CreateLoad();

private:
	std::unique_ptr<Lamb::Thread> thread_;
	bool isLoad_;

	std::unique_ptr<Tex2DAniamtor> tex2Danimator_;
	Lamb::SafePtr<Texture2D> loadTex_;
	uint32_t textureID_;

	Mat4x4 cameraMatrix_;

	class RenderContextManager* renderContextManager_;
};