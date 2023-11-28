#pragma once
#include "Drawers/Model/Model.h"
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easing/Easing.h"
#include "Drawers/Particle/Particle.h"

class Enemy : public Collider {
public:
	Enemy();
	Enemy(const Enemy&) = default;
	~Enemy() = default;
	Enemy& operator=(const Enemy&) = default;

public:
	void Update();

	void Draw();
	void ParticleDraw();

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

	void EnableDamageAccept() {
		isDamageAccept_ = true;
	}

	void Damage() {
		if (isDamageAccept_) {
			hp_--;
			isDamageAccept_ = false;
			if (hp_ <= 0.0f) {
				DeleteStart();
			}
		}
	}

	void DeleteStart();

	bool IsDeath() const {
		return isDeath_;
	}


public:
	Vector3 pos_;
	Vector3 moveVec;
private:
	std::vector<std::unique_ptr<Model>> model;

	Easing ease;
	std::pair<Vector3, Vector3> easeDuration;

	float spd;



	Camera* camera;

	float freqSpd;
	float freq;
	float radius;

	class Player* player_ = nullptr;

	float distanceLimit;
	bool isPlayerCollsion;

	bool isDamageAccept_;
	float hp_;

	bool isDeath_;

	Particle particle_;
};