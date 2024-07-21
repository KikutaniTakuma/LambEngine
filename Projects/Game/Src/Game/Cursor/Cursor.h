#pragma once

#include <memory>

#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Camera/Camera.h"

class Cursor {
public:
	void Initialize();

	void Update();

	void Draw(const Camera& cameracameraMat);
	void Draw(const Mat4x4& cameracameraMat);

private:
	Vector2 ChangeCursorToTexturePos();

	std::unique_ptr<Texture2D> cursor_;

	uint32_t time_;

	Vector2 offset_;

};