#include "InputHandler.h"
#include "Game/Command/ICommand/ICommand.h"
#include "Game/Command/MoveLeftCommand/MoveLeftCommand.h"
#include "Game/Command/MoveRightCommand/MoveRightCommand.h"
#include "Input/Input.h"


InputHandler::~InputHandler() {
	delete pressKeyA_;
	delete pressKeyD_;
}

void InputHandler::AssignMoveLeftCommandToPressKeyA() {
	pressKeyA_ = new MoveLeftCommand{};
}
void InputHandler::AssignMoveRightCommandToPressKeyD() {
	pressKeyD_ = new MoveRightCommand{};
}

ICommand* InputHandler::HandleInput() {
	if (KeyInput::GetInstance()->GetKey(DIK_A)) {
		return pressKeyA_;
	}
	else if (KeyInput::GetInstance()->GetKey(DIK_D)) {
		return pressKeyD_;
	}

	return nullptr;
}