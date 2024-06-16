#pragma once
#include "../Object.h"
#include "ObbComp.h"

class ObbPushComp : public IComp {
public:
	using IComp::IComp;

	~ObbPushComp() = default;

	void Init() override;

	void Collision(Lamb::SafePtr<ObbComp> other);

	// 押し出すタグ
	// なければ""をいれて
	void SetPushTag(const std::string& pushTag);
	void ErasePushTag(const std::string& pushTag);

	ObbComp& GetObbComp() {
		return *obbComp_;
	}
	const ObbComp& GetObbComp() const {
		return *obbComp_;
	}

private:
	std::unordered_set<std::string> pushTags_;
	Lamb::SafePtr<ObbComp> obbComp_;
};