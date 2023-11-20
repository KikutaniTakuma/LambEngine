#pragma once
#include "Drawers/Texture2D/Texture2D.h"
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>
#include "Game/CollisionManager/Collider/Collider.h"

class Player {
public:
	enum class Behavior {
		Normal,
		Attack,
	};

public:
	Player();
	Player(const Player&) = default;
	~Player() = default;
	Player& operator=(const Player&) = default;

public:
	void ResetMoveVec();

	void Update();

	void Draw(const Mat4x4& cameraMat);

	void Debug();

public:
	void MoveRight();
	void MoveLeft();

public:
	Vector3 pos_;
	Vector3 rotate_;
	Vector2 scale_;

	Vector3 moveVec_;

private:
	Texture2D tex2D_;

	float speed_;
};