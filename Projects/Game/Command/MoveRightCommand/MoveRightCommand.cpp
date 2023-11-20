#include "MoveRightCommand.h"
#include "Game/Player/Player.h"

void MoveRightCommand::Exec(class Player& player) {
	player.MoveRight();
}