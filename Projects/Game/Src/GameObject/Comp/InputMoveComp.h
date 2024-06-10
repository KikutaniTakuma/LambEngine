#pragma once
#include "../Object.h"
#include "Drawers/Model/Model.h"

#include "Utils/Flg.h"

class InputMoveComp : public IComp {
public:
	using IComp::IComp;

	~InputMoveComp() = default;

	void FirstUpdate() override;;

private:
	float32_t speed_;


};