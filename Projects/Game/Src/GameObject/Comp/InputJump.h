#pragma once
#include "../Object.h"
#include "Drawers/Model/Model.h"

#include "Utils/Flg.h"

class InputJump : public IComp {
public:
	using IComp::IComp;

	~InputJump() = default;

	void FirstUpdate() override;


public:
	void SetSpeed(float32_t speed) {
		speed_ = speed;
	}


public:
	float32_t GetJumpStrength() {
		return strength_;
	}



private:
	float32_t speed_ = 0.0_f32;
	float32_t strength_ = 0.0_f32;
};