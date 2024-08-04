#pragma once
#include "../Object.h"

class ChildrenObjectComp : public IComp {
public:
	using IComp::IComp;

	~ChildrenObjectComp() = default;

public:
	void Init() override;
	void Finalize() override;

	void AddObject(Lamb::SafePtr<Object> object);
	void EraseObject(Object* object);

	void ClearObjects();

	void FirstUpdate() override;

	void Move() override;

	void Event() override;
	void Update() override;
	void LastUpdate() override;

	void Draw(CameraComp* cameraComp) override;

	void Debug(const std::string& guiName) override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	const std::unordered_set<std::unique_ptr<Object>>& GetObjects() const;

private:
	std::unordered_set<std::unique_ptr<Object>> objects_;
	Lamb::SafePtr<class TransformComp> transformComp_;
};