#pragma once

#include <memory>

#include "AudioManager/AudioManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Math/Mat4x4.h"

class Camera;
class Player;
class Whirlpool : public Collider {
public:
	Whirlpool();

	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos,float gameOverSpeed);
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
	const float GetGameOverSpeed() { return gameOverSpeed_; }
	void SetGameOverSpeed(float gameOverSpeed) { gameOverSpeed_ = gameOverSpeed; }
private:
	void OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) override;

	Player* player_;
	std::unique_ptr<ModelInstance> model_;
	std::unique_ptr<ModelInstance> debugModel_;
	std::unique_ptr<Tex2DInstance> whirlpool_;

	Audio* deathSE_;

	float gameOverSpeed_;
};