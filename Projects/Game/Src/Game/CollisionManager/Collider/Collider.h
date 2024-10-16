#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "../AABB/AABB.h"
#include "../Obb/Obb.h"
#include "Drawer/Line/Line.h"

#include <vector>
#include <bitset>
#include <memory>

/// <summary>
/// 当たり判定(AABB)
/// </summary>
class Collider {
public:
	enum Type {
		kTrriger,
		kCollion,

		kTypeCount,
	};

	enum Attribute {
		kPlayer,
		kPlayerBullet,
		kOther,

		kAttributeCount,
	};

	void InitializeCollision(const Vector3& scale, const Vector3& rotate, const Vector3& pos);

	void UpdateCollision(const Vector3& rotate, const Vector3& pos, uint32_t index = 0);

	virtual void OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) = 0;

	void DebugDraw(const Mat4x4& viewProjection, uint32_t index = 0);

	void Debug(const std::string& guiName, uint32_t index = 0);

	void SetCollisionScale(const Vector3& aabb, uint32_t index = 0);

	const AABB& GetAABB(uint32_t index = 0) const { return aabb_.at(index); }

	const OBB& GetOBB(uint32_t index = 0)const { return obb_.at(index); }

	void SetCollisionColor(uint32_t color) { color_.emplace_back(color); }
	void SetCollisionColor(uint32_t color,uint32_t index) { color_.at(index) = color; }

	void SetColliderType(const Type& type) { colliderType_.emplace_back(type); }

	const Type& GetColliderType(uint32_t index = 0) const { return colliderType_.at(index); }

	void SetColliderAttribute(const Attribute& type) { colliderAttribute_.emplace_back(type); }

	const Attribute& GetColliderAttribute(uint32_t index = 0) const { return colliderAttribute_.at(index); }

	uint32_t GetColliderSize() { return uint32_t(aabb_.size()); }
protected:
	std::vector<Vector3>aabbMin_;
	std::vector<Vector3>aabbMax_;
	std::vector<Vector3>obbSize_;

	std::vector<AABB>aabb_;
	std::vector<OBB>obb_;

	std::vector<uint32_t> color_;

	std::vector<Type> colliderType_;

	std::vector<Attribute> colliderAttribute_;

	std::vector<std::array<std::unique_ptr<Line>, 12>> lines_;
};