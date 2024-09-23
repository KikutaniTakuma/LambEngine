#pragma once
#include "../Object.h"
#include "ObbComp.h"

class ObbPushComp : public IComp {
public:
	using IComp::IComp;

	~ObbPushComp() = default;

	void Init() override;
	void Finalize() override;

	void Collision(Lamb::SafePtr<ObbPushComp> other);

	// 押し出すタグ
	void SetPushTag(const std::string& pushTag);
	void ErasePushTag(const std::string& pushTag);

	ObbComp& GetObbComp() {
		return *obbComp_;
	}
	const ObbComp& GetObbComp() const {
		return *obbComp_;
	}

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

	void Debug(const std::string& guiName) override;

	const std::unordered_set<std::string>& GetCollisionTagList() const {
		return pushTags_;
	}

private:
	std::unordered_set<std::string> pushTags_;
	Lamb::SafePtr<ObbComp> obbComp_;
#ifdef _DEBUG
	std::string inputTag_;
#endif // _DEBUG

};