#pragma once
#include "../Object.h"
#include "Drawers/Model/Model.h"

#include "Utils/Flg.h"

class InputMoveComp : public IComp {
public:
	using IComp::IComp;

	~InputMoveComp() = default;

	void FirstUpdate() override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

	
public:
	void SetSpeed(float32_t speed) {
		speed_ = speed;
	}


public:
	const Vector3& GetDirection() const {
		return direction_;
	}

	const Vector3& GetMoveVector() const {
		return moveVector_;
	}



private:
	float32_t speed_ = 0.0f;
	Vector3 direction_;

	Vector3 moveVector_;

	bool isPad_ = false;
};