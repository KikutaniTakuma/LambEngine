#include "../Object.h"
class UIComp : public IComp {
public:
	using IComp::IComp;

	~UIComp() = default;

public:
	void Init() override;

	void Event() override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

private:
	Lamb::SafePtr<class ChildrenObjectComp> userInterface_;
};
