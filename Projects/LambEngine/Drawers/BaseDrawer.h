#pragma once
#include "Utils/Camera/Camera.h"
#include "Math/Mat4x4.h"
#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <string>
#include "Engine/Graphics/GraphicsStructs.h"

class BaseDrawer {
public:
	BaseDrawer();
	BaseDrawer(const BaseDrawer&) = default;
	BaseDrawer(BaseDrawer&&) = default;
	virtual ~BaseDrawer() = default;

public:
	BaseDrawer& operator=(const BaseDrawer&) = default;
	BaseDrawer& operator=(BaseDrawer&&) = default;

public:
	virtual void Load(const std::string& fileName) = 0;

	virtual void Draw(const Camera* camera, BlendType blend);

	virtual void Debug(const std::string& guiName);

public:
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
	
	uint32_t color;

protected:
	class RenderSet* renderSet;
};

