#include "../Object.h"
#include "../Comp/SpriteRenderComp.h"

class WhirlpoolsComp : public IComp {
public:
	using IComp::IComp;

	~WhirlpoolsComp() = default;

public:
	void Init();

	void Update();

	// 渦を通れる速度か？
	bool CanPathThrough(float32_t speed);

	void Debug(const std::string& guiName) override; 

	void Save(nlohmann::json& json) override;

private:
	Lamb::SafePtr<class TransformComp> transformComp_;

	float32_t pathThroughSpeed_ = 0.0f;

	float32_t rotate_ = 0.0f;
	float32_t rotateSpeed_ = 0.0f;

	Lamb::SafePtr<class ObbPushComp> obbComp_;
	Lamb::SafePtr<SpriteRenderComp> spriteRenderComp_;
};
