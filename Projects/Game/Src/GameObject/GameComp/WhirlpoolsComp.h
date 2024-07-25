#include "../Object.h"
class WhirlpoolsComp : public IComp {
public:
	using IComp::IComp;

	~WhirlpoolsComp() = default;

public:
	void Init();

	void Update();

	// 渦を通れる速度か？
	bool CanPathThrough(float32_t speed);

private:
	Lamb::SafePtr<class TransformComp> transformComp_;

	float32_t pathThroughSpeed_ = 0.0f;

	Lamb::SafePtr<class ObbPushComp> obbComp_;
};
