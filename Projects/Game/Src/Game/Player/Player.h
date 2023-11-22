#pragma once
#include "Drawers/Model/Model.h"
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easeing/Easeing.h"

class Player : public Collider {
public:
	enum class Behavior {
		Normal,
		Attack,
	};

public:
	static void Initialize(class GlobalVariables* globalVariables);

public:
	Player() = delete;
	Player(class GlobalVariables* globalVariables);
	Player(const Player&) = default;
	~Player() = default;
	Player& operator=(const Player&) = default;

public:
	void Move();

	void Update();

	void Draw();

	inline Vector3 GetPos() const {
		return (*model_.begin())->pos_;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

	void Debug();

	Collider& GetWeaponCollider() {
		return weaponColliser_;
	}

	const Behavior& GetBehavior() const {
		return behavior;
	}

private:
	void Animation();

	void ApplyGlobalVariables();


public:
	Vector3 pos_;

	Vector3 moveVec_;

private:
	class GlobalVariables* globalVariables_ = nullptr;

	std::vector<std::unique_ptr<Model>> model_;
	std::unique_ptr<Model> weapon_;
	Collider weaponColliser_;

	float spd;

	Camera* camera;

	float freqSpd;
	float freq;

	float armFreqSpd;
	float armFreq;

	Behavior behavior;

	float attack;
	float attackSpd;

	float cameraRotate_;
	float cmaeraRotateSpd_;

	std::chrono::steady_clock::time_point dashStartTime_;
	std::chrono::milliseconds dashCoolTime_;
	Lamb::Flg isDash_;

	float dashScale_;

	Easeing cameraEaseing_;
	Vector3 preCameraPos_;

	float delayCaemraSpeed_;

	Vector3 preMoveVec_;
};