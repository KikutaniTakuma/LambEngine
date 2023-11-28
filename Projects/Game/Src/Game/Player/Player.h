#pragma once
#include "Drawers/Model/Model.h"
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easing/Easing.h"

class Player : public Collider {
public:
	enum class Behavior {
		Normal,
		Attack,
	};

	struct ComboAttack {
		// 溜め時間
		std::chrono::milliseconds chargeTime_;

		// 攻撃振り速度
		float swingSpeed_;
	};

	struct WorkAttack {
		std::chrono::steady_clock::time_point attackStartTime_;

		uint32_t currentComboAttack_;

		bool isNext_;
	};

	static constexpr int32_t kMaxComboNum_ = 3;

private:
	static const std::array<Player::ComboAttack, Player::kMaxComboNum_> kComboAttacks_;

public:
	static void Initialize(class GlobalVariables* globalVariables);

public:
	Player() = delete;
	Player(class GlobalVariables* globalVariables);
	Player(const Player&) = default;
	~Player() = default;
	Player& operator=(const Player&) = default;

public:
	void Move(const Mat4x4& cameraRoatate);

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
	
	const Lamb::Flg& GetIsDash() const {
		return isDash_;
	}

	bool isWeaopnCollsion() const {
		return isWeaopnCollsion_;
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

	WorkAttack workAttack_;
	std::chrono::steady_clock::time_point attackStartTime_;

	std::chrono::steady_clock::time_point dashStartTime_;
	std::chrono::milliseconds dashCoolTime_;
	Lamb::Flg isDash_;

	float dashScale_;

	Vector3 preMoveVec_;

	float jumpVelocity_;
	float gravity_;

	bool isWeaopnCollsion_;
};