#include "../Object.h"
class WindComp : public IComp {
	static constexpr size_t kMaxWindowNodeSize_ = 512;

public:
	using IComp::IComp;

	~WindComp() = default;

public:
	void Init() override;

	void FirstUpdate();

	Vector3 GetDirection() const;

private:
	Lamb::SafePtr<class ChildrenObjectComp> childrenComp_;
	std::unordered_set<Lamb::SafePtr<class WindNodeComp>> windNodeComp_;

	Lamb::SafePtr<class ObbPushComp> obbComp_;
	Lamb::SafePtr<class TransformComp> transformComp_;

private:
	// 風の向き(デフォルトはZ方向)
	Vector3 direction_ = Vector3::kXIdentity;


private:
	// 生成場所ランダム範囲
	std::pair<Vector3, Vector3> positionRange_;

	// 長さランダム範囲
	std::pair<float32_t, float32_t> lengthRange_;

	// 速度ランダム範囲
	std::pair<float32_t, float32_t> speedRange_;

	//一度に出る個数のランダム範囲
	std::pair<uint32_t, uint32_t> appearNumberRange_;

	// 出現頻度のランダム範囲
	std::pair<uint32_t, uint32_t> appearFrequencyRange_;
	// 出現頻度のランダム範囲がこの値以下なら生成する
	uint32_t appearNum_;
};
