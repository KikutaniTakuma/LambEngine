#pragma once

#include "../Object.h"
#include "TransformComp.h"
#include "Utils/Flg.h"

#include <memory>


class ObbComp : public IComp {
public:
	using IComp::IComp;

	~ObbComp() = default;

	void Init() override;

	void Finalize() override;

	void FirstUpdate() override;

	void Event() override;

	/// <summary>
	/// 移動したら呼び出すこと
	/// </summary>
	void UpdatePosAndOrient();

	void Draw(CameraComp* cameraComp) override;

	bool IsCollision(Vector3 pos, float radius);
	bool IsCollision(ObbComp* const other);
	bool IsCollision(ObbComp* const other, Vector3& pushVector);
	bool IsCollision(const Vector3& start, const Vector3& end);

	bool Collision(ObbComp* const other);

	TransformComp& GetTransformComp();
	const TransformComp& GetTransformComp() const;

	void SetCollisionTag(const std::string& collisionTag);
	void EraseCollisionTag(const std::string& collisionTag);

	void Debug(const std::string& guiName);

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

	const std::string& GetCurrentCollisionTag() const;
	const std::unordered_set<std::string>& GetCollisionTagList() const {
		return collisionTags_;
	}

	// transformのスケールの影響を受けないようにする
	void SetEnableScaleEffect(bool isScaleEffect);

public:
	Vector3 scale = Vector3::kIdentity;
	Vector3 center;


private:
	Lamb::SafePtr<TransformComp> transformComp_;

	std::unordered_set<std::string> collisionTags_;

#ifdef _DEBUG
private:
	uint32_t color_;
	std::string inputTag_;
#endif // _DEBUG

private:
	static std::array<const Vector3, 8> localPositions_;
	static std::array<const Vector3, 3> localOrientations_;
private:
	std::unique_ptr<std::array<Vector3, 8>> positions_;
	std::unique_ptr<std::array<Vector3, 3>> orientations_;

	Lamb::Flg isCollision_;

	std::string currentCollisionTag_;

	bool isScaleEffect_ = true;

	/// <summary>
	/// ゲッター
	/// </summary>
public:
	const Lamb::Flg& GetIsCollision() const {
		return isCollision_;
	}
};