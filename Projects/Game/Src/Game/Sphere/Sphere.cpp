#include "Sphere/Sphere.h"
#include "Grid/Grid.h"
#include <Novice.h>
#include <numbers>
#include <cassert>

Sphere::Sphere() :
	radius(1.0f),
	loaclVec(radius,0.0f,0.0f),
	scale(1.0f,1.0f,1.0f),
	worldRotate(0.0f, 0.0f, 0.0f),
	localRotate(0.0f,0.0f,0.0f),
	translation(0.0f,0.0f,0.0f),
	worldMat(),
	kDivision(10),
	sphereLocalPosList(std::make_unique<std::vector<std::vector<Vector3>>>(0)),
	sphereWorldPosList(std::make_unique<std::vector<std::vector<Vector3>>>(0))
{
	assert(sphereLocalPosList);
	assert(sphereWorldPosList);

	Mat4x4 localRotateMatrix;

	for (int y = 0; y <= kDivision; y++) {
		localRotate.z = -std::numbers::pi_v<float> / 2.0f;
		sphereLocalPosList->push_back(std::vector<Vector3>(0));
		for (int z = 0; z <= kDivision; z++) {
			localRotateMatrix = HoriMakeMatrixRotateZ(localRotate.z) * HoriMakeMatrixRotateY(localRotate.y);
			sphereLocalPosList->rbegin()->push_back(loaclVec * localRotateMatrix);
			localRotate.z += (std::numbers::pi_v<float> / static_cast<float>(kDivision));
		}

		localRotate.y += (std::numbers::pi_v<float> / static_cast<float>(kDivision)) * 2.0f;
	}
	sphereWorldPosList->resize(sphereLocalPosList->size());
	auto localStart = sphereLocalPosList->begin();
	for (auto& i : *sphereWorldPosList) {
		i.resize(localStart->size());
		localStart++;
	}
	
}

Sphere::~Sphere() {
}


void Sphere::Update() {
	scale = { radius,radius,radius };
	worldMat.HoriAffin(scale, worldRotate, translation);
	for (size_t i = 0; i < sphereWorldPosList->size() && i < sphereLocalPosList->size();i++) {
		for (size_t j = 0; j < (*sphereWorldPosList)[i].size() && j < (*sphereLocalPosList)[i].size(); j++) {
			(*sphereWorldPosList)[i][j] = (*sphereLocalPosList)[i][j] * worldMat;
		}
	}
}

void Sphere::Draw(const Mat4x4& viewProjectionMatrix, const Mat4x4& viewPortmatrix, uint32_t color) {
	auto vpvpMat = viewProjectionMatrix * viewPortmatrix;

	
	std::vector<Vector3> preHarfCircle;
	std::vector<Vector3>::iterator preItr;
	for (auto i = sphereWorldPosList->begin(); i != sphereWorldPosList->end(); ++i) {
		if (i != sphereWorldPosList->begin()) {
			preItr = preHarfCircle.begin();
		}

		for (auto j = i->begin(); j != i->end();) {
			Vector3 startPos = *j * vpvpMat;
			if (++j != i->rbegin().base()) {
				Vector3 endPos = *(j)*vpvpMat;
				Novice::DrawLine(static_cast<int>(startPos.x), static_cast<int>(startPos.y), static_cast<int>(endPos.x), static_cast<int>(endPos.y), color);

				if (i != sphereWorldPosList->begin()) {
					preItr++;
					Vector3 prePos = (*preItr) * vpvpMat;
					Novice::DrawLine(static_cast<int>(prePos.x), static_cast<int>(prePos.y), static_cast<int>(endPos.x), static_cast<int>(endPos.y), color);
				}
			}
		}
		preHarfCircle = (*i);
	}
}


bool Sphere::IsCollision(const Sphere& sphere) const {
	float distance = (sphere.translation - this->translation).Length();

	return distance < sphere.radius + this->radius ? true : false;
}

bool Sphere::IsCollision(const Plane& plane) const {
	float distance = plane.normal.Dot(translation) - plane.distance;

	return std::abs(distance) < this->radius ? true : false;
}