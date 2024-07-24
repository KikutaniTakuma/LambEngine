#pragma once
#include "../Object.h"

class ChildrenObjectComp : public IComp {
public:
	using IComp::IComp;

	~ChildrenObjectComp() = default;

public:
	void Init() override;

	void AddObject(Object* object);
	void EraseObject(Object* object);

	void FirstUpdate() override;
	void Update() override;

	void Move() override;

	void Event() override;
	void Update() override;
	void LastUpdate() override;

	void Draw() override;

public:
	const std::unordered_set<Lamb::SafePtr<Object>>& GetObjects() const;

private:
	std::unordered_set<Lamb::SafePtr<Object>> objects_;
};