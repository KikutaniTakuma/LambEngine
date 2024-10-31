#pragma once

#include <memory>

#include "Drawer/Texture2D/Texture2D.h"
#include "Camera/Camera.h"

class Cursor {
public:
	void Initialize();

	void Update();

	void Draw(const Camera& cameracameraMat);
	void Draw(const Mat4x4& cameracameraMat);

private:
	Vector2 ChangeCursorToTexturePos();

	std::unique_ptr<Tex2DInstance> cursor_;

	uint32_t time_;

	Vector2 offset_;

};