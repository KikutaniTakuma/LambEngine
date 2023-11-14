#pragma once
#include "Game/CollisionManager/Collider/Collider.h"
#include "Drawers/Model/Model.h"
#include "Utils/Easeing/Easeing.h"

class MoveFloor {
public:
	MoveFloor();
	MoveFloor(const MoveFloor&) = default;
	MoveFloor(MoveFloor&&) noexcept = default;
	~MoveFloor() = default;

	MoveFloor& operator=(const MoveFloor&) = default;
	MoveFloor& operator=(MoveFloor&&) noexcept = default;

public:
	void Update();

	void Draw(const Mat4x4& viewProjection, const Vector3& cameraPos);

	void Debug(const std::string& debugName);

public:
	std::pair<Vector3, Vector3> moveDuration_;

	Vector3 pos_;

	Vector3 moveVec_;

	Collider colliser_;
private:
	std::unique_ptr<Model> model_;

	Easeing ease_;

};