#pragma once

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include <vector>
#include <memory>

class Sphere {
	friend class MyModel;

public:
	Sphere();
	~Sphere();


public:
	void Update();
	void Draw(const Mat4x4& viewProjectionMatrix, const Mat4x4& viewPortmatrix, uint32_t color);

public:
	float radius;
	Vector3 translation;
	Vector3 worldRotate;

private:
	Vector3 loaclVec;

	Vector3 scale;
	Vector3 localRotate;

	Mat4x4 worldMat;

	const int kDivision;

	std::unique_ptr<std::vector<std::vector<Vector3>>> sphereLocalPosList;
	std::unique_ptr<std::vector<std::vector<Vector3>>> sphereWorldPosList;

public:
	bool IsCollision(const Sphere& sphere) const;
	bool IsCollision(const struct Plane& plane) const;
};