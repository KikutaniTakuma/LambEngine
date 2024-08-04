#include "../Object.h"
#include "CannonComp.h"
#include "LoopCannonComp.h"
#include "SailComp.h"

class CustomizeComp : public IComp {
public:
	using IComp::IComp;

	~CustomizeComp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;
	void Load() override;

public:
	void Init() override;

	void Update();

private:
	Lamb::SafePtr<class ChildrenObjectComp> items_;
	Lamb::SafePtr<class CsvDataComp> csvData_;
	Lamb::SafePtr<class JsonComp> jsonData_;

	std::vector<Lamb::SafePtr<CannonComp>> cannonComps_;
	std::vector<Lamb::SafePtr<LoopCannonComp>> loopCannonComps_;
	std::vector<Lamb::SafePtr<SailComp>> sailComps_;

	// 大砲の数
	int32_t maxCannonNum_ = 0;
	// ループ大砲の数
	int32_t maxLoopCannonNum_ = 0;
	// 帆の数
	int32_t maxSailNum_ = 0;
};
