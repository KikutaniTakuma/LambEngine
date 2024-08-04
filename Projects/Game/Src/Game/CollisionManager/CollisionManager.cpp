#include "CollisionManager.h"

#include "Collider/Collider.h"
#include "Collision/Collision.h"
#include "Math/Vector4.h"

void CollisionManager::SetCollider(class Collider* collider, uint32_t index) {
	ColliderInfo* tmp = new ColliderInfo();
	collider->SetCollisionColor(Vector4ToUint(Vector4::kIdentity),index);
	tmp->collider = collider;
	tmp->index = index;
	colliders_.push_back(tmp);
}

void CollisionManager::Update() {
	for (auto i = colliders_.begin(); i != colliders_.end(); i++) {
		for (auto j = colliders_.begin(); j != colliders_.end(); j++) {
			if (i == j) {
				continue;
			}
			else {
				if ((*i)->collider->GetColliderAttribute((*i)->index) != (*j)->collider->GetColliderAttribute((*j)->index) &&
					BroadPhase((*i), (*j))) {
					(*i)->collider->OnCollision((*j)->collider, (*i)->index, (*j)->index);
					(*j)->collider->OnCollision((*i)->collider, (*j)->index, (*i)->index);
				}
			}
		}
	}
	colliders_.clear();
}

bool CollisionManager::BroadPhase(ColliderInfo* a, ColliderInfo* b) {
	if (Collision::IsCollision(a->collider->GetAABB(a->index), b->collider->GetAABB(b->index))) {
		return true;
	}
	return false;
}
