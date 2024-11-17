#pragma once
#include "Math/Vector3.h"
#include "Math/Matrix.h"

struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;

	OBB& SetRotate(const Vector3& rotate) {
		Mat4x4 rotateMatrix = Mat4x4::MakeRotateX(rotate.x) * Mat4x4::MakeRotateY(rotate.y) * Mat4x4::MakeRotateZ(rotate.z);

		this->orientations[0].x = rotateMatrix[0][0];
		this->orientations[0].y = rotateMatrix[0][1];
		this->orientations[0].z = rotateMatrix[0][2];

		this->orientations[1].x = rotateMatrix[1][0];
		this->orientations[1].y = rotateMatrix[1][1];
		this->orientations[1].z = rotateMatrix[1][2];

		this->orientations[2].x = rotateMatrix[2][0];
		this->orientations[2].y = rotateMatrix[2][1];
		this->orientations[2].z = rotateMatrix[2][2];

		return *this;
	}
	std::vector<Vector3> GetVertices() {
		Vector3 halfSize = size * 0.5f;
		std::vector<Vector3> vertices = {
				{-halfSize},
				{-halfSize.x, +halfSize.y, -halfSize.z},
				{+halfSize.x, +halfSize.y, -halfSize.z},
				{+halfSize.x, -halfSize.y, -halfSize.z},
				{-halfSize.x, -halfSize.y, +halfSize.z},
				{-halfSize.x, +halfSize.y, +halfSize.z},
				{+halfSize},
				{+halfSize.x, -halfSize.y, +halfSize.z},
		};

		Mat4x4 rotateMatrix{};
		rotateMatrix.Identity();
		for (size_t y = 0; y < 3; y++) {
			for (size_t x = 0; x < orientations[y].size(); x++) {
				rotateMatrix[y][x] = orientations[y][x];
			}
		}
		for (auto& vertex : vertices) {
			vertex = vertex * rotateMatrix;
			vertex = vertex + center;
		}

		return vertices;
	}
};