#pragma once
#include "../Object.h"
class PlayerComp : public IComp {
public:
	using IComp::IComp;

	~PlayerComp() = default;

private:
	void Init() override;
	void Finalize() override;


	void FirstUpdate() override;
	void Move() override;
	void Event() override;
	void Update() override;
	void LastUpdate() override;

	void Draw() override;
};