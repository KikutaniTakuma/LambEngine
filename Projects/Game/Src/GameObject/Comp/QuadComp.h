#include "../Object.h"

//	このクラスは2Dで使うことが前提
class QuadComp : public IComp {
public:
	using IComp::IComp;

	~QuadComp() = default;

public:
	void Init() override;

	void FirstUpdate() override;

	// xyのみ
	bool IsCollision(const Vector2& pos);
	// マウスカーソルの位置との当たり判定
	bool IsCollisionMouse(const Mat4x4& view, const Mat4x4& ndc);

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

	void Draw(CameraComp* cameraComp) override;

	const Lamb::Flg& GetIsCollision() const;

private:
	Lamb::SafePtr<class TransformComp> transformComp_;
	Lamb::Flg isCollision_;
#ifdef _DEBUG
	uint32_t color_ = 0xffffffff;
#endif // _DEBUG

};
