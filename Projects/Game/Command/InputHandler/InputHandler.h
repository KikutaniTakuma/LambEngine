#pragma once

class InputHandler {
public:
	InputHandler() = default;
	InputHandler(const InputHandler&) = delete;
	InputHandler(InputHandler&&) = delete;
	~InputHandler();

	InputHandler& operator=(const InputHandler&) = delete;
	InputHandler& operator=(InputHandler&&) = delete;

public:
	class ICommand* HandleInput();

	void AssignMoveLeftCommandToPressKeyA();
	void AssignMoveRightCommandToPressKeyD();

private:
	class ICommand* pressKeyA_;
	class ICommand* pressKeyD_;
};