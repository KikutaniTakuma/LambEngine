#pragma once
#include "../Object.h"
class PlayerComp : public IComp {
public:
	using IComp::IComp;

	~PlayerComp() = default;

public:
	void Init() override;
	void Finalize() override;


	void FirstUpdate() override;
	void Move() override;
	void Event() override;
	void Update() override;
	void LastUpdate() override;

	void Draw(CameraComp* cameraComp) override;

	void Save(nlohmann::json& json) override;

private:
	Lamb::SafePtr<class ChildrenObjectComp> item_;
	Lamb::SafePtr<class CsvDataComp> itemData_;
};