#pragma once
#include <memory>

#include "AudioManager/AudioManager.h"
#include "Drawers/Model/Model.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Math/Mat4x4.h"
#include "Utils/Flg/Flg.h"

class Camera;
class Player;
class Block : public Collider {
public:
	Block();
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos);
	void ResourceUpdate();
	void Update();
	void Draw(const Camera& camera);

	void SetPlayer(Player* player) { player_ = player; }
	const Vector3& GetScale() { return model_->scale; }
	void SetScale(const Vector3& scale);
	const Vector3& GetRotate() { return model_->rotate; }
	void SetRotate(const Vector3& rotate);
	const Vector3& GetPosition() { return model_->pos; }
	void SetPosition(const Vector3& pos);
	void SetHitFlag(bool flag) { hitFlag_ = flag; }
private:
	void OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) override;
	std::unique_ptr<Model> model_;
	Player* player_;
	Audio* hitBlockSound_;
	bool hitFlag_;
	float subtractionVelocity_;
	float subtractionCannonVelocity_;
	float subtractionSailVelocity_;
};