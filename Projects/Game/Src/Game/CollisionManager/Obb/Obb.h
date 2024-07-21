#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"

struct OBB {
	Vector3 center;          
	Vector3 orientations[3]; 
	Vector3 size;

	OBB& SetRotate(const Vector3& rotate) {
		Mat4x4 rotateMatrix = Mat4x4::MakeRotateX(rotate.x)* Mat4x4::MakeRotateY(rotate.y)* Mat4x4::MakeRotateZ(rotate.z);

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
};