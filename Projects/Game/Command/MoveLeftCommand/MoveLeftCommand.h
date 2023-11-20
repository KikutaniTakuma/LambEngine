#pragma once
#include "Game/Command/ICommand/ICommand.h"

class MoveLeftCommand : public ICommand {
	void Exec(class Player& player) override;
};