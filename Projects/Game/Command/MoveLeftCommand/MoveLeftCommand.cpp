#include "MoveLeftCommand.h"
#include "Game/Player/Player.h"

void MoveLeftCommand::Exec(class Player& player) {
	player.MoveLeft();
}