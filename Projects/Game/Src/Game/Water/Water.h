#pragma once
#include <memory>
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Camera/Camera.h"
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"

class Water final {
public:
	Water()= default;

	~Water() = default;

public:
	void Init();

	void Update();

	void Draw(const Mat4x4& cameraMat, const Vector3& cameraPos);

	void Debug(const std::string& guiName);

public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

private:
	std::unique_ptr<PeraRender> pera_;
	std::unique_ptr<PeraRender> luminate_;
	std::unique_ptr<PeraRender> bloom_;
	std::unique_ptr<Texture2D> waterSurface_;

	class WaterPipeline* waterPipelineObject_ = nullptr;

	std::unique_ptr<Camera> staticCamera_;
};