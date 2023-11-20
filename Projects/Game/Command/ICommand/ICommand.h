#pragma once

class ICommand {
public:
	virtual ~ICommand() {};
	virtual void Exec(class Player&) = 0;
};