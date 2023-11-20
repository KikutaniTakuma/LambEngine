#pragma once
#include "Game/CollisionManager/Collider/Collider.h"
#include "Drawers/Model/Model.h"

class Goal : public Collider {
public:
	Goal();
	Goal(const Goal&) = default;
	Goal(Goal&&) = default;
	~Goal() = default;

	Goal& operator=(const Goal&) = default;
	Goal& operator=(Goal&&) = default;

public:
	void Update();

	void Draw(const Mat4x4& viewProjection, const Vector3& cameraPos);

	void Debug(const std::string& debugName);

private:
	std::unique_ptr<Model> model_;
};