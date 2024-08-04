#include "../Object.h"
class CustomizeComp : public IComp {
public:
	using IComp::IComp;

	~CustomizeComp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	void Init() override;

	void Update();

private:
	Lamb::SafePtr<class ChildrenObjectComp> items_;
	Lamb::SafePtr<class CsvDataComp> csvData_;
};
