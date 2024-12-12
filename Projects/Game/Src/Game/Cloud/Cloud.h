#pragma once
#include "Drawer/Other/CloudTex2D/CloudTex2D.h"
#include "Transform/Transform.h"

/// <summary>
/// 雲描画
/// </summary>
class Cloud {
public:
	Cloud();
	~Cloud() = default;
	
public:
	void Draw(const Mat4x4& camera);

private:
	std::unique_ptr<CloudTex2D> cloudTex_;
	QuaternionTransform transform_;
	QuaternionTransform uvTransform_;
	Vector3 vec_;
};