#pragma once
#include "Game/Command/ICommand/ICommand.h"

class MoveRightCommand : public ICommand {
public:
	void Exec(class Player& player) override;
};