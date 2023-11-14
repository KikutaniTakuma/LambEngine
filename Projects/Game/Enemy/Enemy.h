#pragma once
#include "Drawers/Model/Model.h"
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easeing/Easeing.h"

class Enemy {
public:
	Enemy();
	Enemy(const Enemy&) = default;
	~Enemy() = default;
	Enemy& operator=(const Enemy&) = default;

public:
	void Update();

	void Draw();

	inline Vector3 GetPos() const {
		return (*model.begin())->pos_;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

	inline void SetPlayer(class Player* player) {
		player_ = player;
	}

	bool GetIsPlayerCollision() const {
		return isPlayerCollsion;
	}

	void Move();

public:
	Vector3 pos_;
	Vector3 moveVec;
	Collider colliser_;
private:
	std::vector<std::unique_ptr<Model>> model;

	Easeing ease;
	std::pair<Vector3, Vector3> easeDuration;

	float spd;


	Camera* camera;

	float freqSpd;
	float freq;
	float radius;

	class Player* player_=nullptr;

	float distanceLimit;
	bool isPlayerCollsion;
};