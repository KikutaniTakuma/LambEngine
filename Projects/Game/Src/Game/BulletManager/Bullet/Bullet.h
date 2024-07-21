#pragma once

#include <memory>

#include "AudioManager/AudioManager.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Math/Mat4x4.h"
#include "Math/Vector3.h"

class Camera;
class Bullet : public Collider {
public:
	void Initialize();

	void Update();

	void ModelUpdate();

	void Draw(const Camera& camera);

	void SetBullet(const Vector3& position, const Vector3& vector);
	bool GetIsAlive() { return isAlive_; }
	void SetIsAlive(bool flag) { isAlive_ = flag; }
private:
	void OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) override;
	std::unique_ptr<Model> model_;
	Vector3 velocity_;
	bool isAlive_;
};