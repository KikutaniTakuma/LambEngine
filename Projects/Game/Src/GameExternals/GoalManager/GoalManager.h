#pragma once

#include <memory>
#include <vector>

#include "Goal/Goal.h"
#include "Math/Matrix.h"
#include "Math/Vector3.h"

class Camera;
class Player;
class GoalManager {
public:
	GoalManager();
	void Initialize();
	void ResourceUpdate();
	void Update();
	void Draw(const Camera& camera);
	void Create(const Vector3& scale, const Vector3& rotate, const Vector3& pos);

	void SetPlayer(Player* player) { player_ = player; }
	const std::vector<std::unique_ptr<Goal>>& GetGoals() const { return goals_; }
	void DeleteGoal(size_t index);
	void SetCamera(Camera* camera) { camera_ = camera; }
	void LoadJson();
private:
	Camera* camera_;
	Player* player_;
	std::vector<std::unique_ptr<Goal>> goals_;
};