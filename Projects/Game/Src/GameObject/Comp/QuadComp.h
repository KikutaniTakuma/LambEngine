#include "../Object.h"

//	このクラスは2Dで使うことが前提
class QuadComp : public IComp {
public:
	using IComp::IComp;

	~QuadComp() = default;

public:
	void Init() override;

	// xyのみ
	bool IsCollision(const Vector2& pos);
	// マウスカーソルの位置との当たり判定
	bool IsCollisionMouse(const Mat4x4& view, const Mat4x4& ndc);

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

private:
	Lamb::SafePtr<class TransformComp> transformComp_;
};
