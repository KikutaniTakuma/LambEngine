#pragma once
#include <memory>
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "./Camera/Camera.h"
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"

class Water final {
private:
	Water() = default;

public:
	~Water() = default;

public:
	static void Initialize();

	static void Finalize();

	static Water* const GetInstance();

private:
	static Water* instance_;


public:
	void Init();

	void Update(const Vector3& cameraPos);

	void Draw(const Mat4x4& cameraMat, PeraRender* const pera = nullptr);

	void Debug(const std::string& guiName);

public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

private:
	std::unique_ptr<PeraRender> pera_;
	std::unique_ptr<PeraRender> luminate_;
	std::unique_ptr<PeraRender> gaussianBlurWidth_;
	std::unique_ptr<PeraRender> gaussianBlurHeight_;
	std::unique_ptr<Texture2D> waterSurface_;
	Transform waterTransform_;
	uint32_t color_;

	class WaterPipeline* waterPipelineObject_ = nullptr;
	class GaussianBlur* gaussianBlurObjectWidth_ = nullptr;
	class GaussianBlur* gaussianBlurObjectHeight_ = nullptr;

	std::unique_ptr<Camera> staticCamera_;
};