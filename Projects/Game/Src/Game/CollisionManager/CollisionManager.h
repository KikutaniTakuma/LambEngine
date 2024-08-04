#pragma once
#include <list>

#include "../CollisionManager/AABB/AABB.h"

/// <summary>
/// コリジョンマネージャー
/// </summary>
class CollisionManager {
private:
	struct ColliderInfo {
		class Collider* collider;
		uint32_t index;
	};

public:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager(CollisionManager&&) = delete;
	~CollisionManager() = default;

	CollisionManager& operator=(const CollisionManager&) = delete;
	CollisionManager& operator=(CollisionManager&&) = delete;

public:
	void SetCollider(class Collider* collider,uint32_t index);

	void Update();

private:
	bool BroadPhase(ColliderInfo* a, ColliderInfo* b);
	std::list<ColliderInfo*> colliders_;
};