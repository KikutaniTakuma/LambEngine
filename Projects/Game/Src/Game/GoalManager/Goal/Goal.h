#pragma once
#include <memory>

#include "Game/CollisionManager/Collider/Collider.h"
#include "Drawer/Model/Model.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Math/Mat4x4.h"
#include "Drawer/Particle/Particle.h"

class Camera;
class Player;
class Goal : public Collider {
public:
	Goal();

	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos);
	void ResourceUpdate();
	void Update();
	void Draw(const Camera& camera);

	void SetPlayer(Player* player) { player_ = player; }
	const Vector3& GetScale() { return goal_->scale; }
	void SetScale(const Vector3& scale);
	const Vector3& GetRotate() { return goal_->rotate; }
	void SetRotate(const Vector3& rotate);
	const Vector3& GetPosition() { return goal_->pos; }
	void SetPosition(const Vector3& pos);
	void SetCamera(Camera* camera) { camera_ = camera; }

	void SetIsHit(bool flag) { isHit_ = flag; }
	
private:
	void ParticleUpdate();
	void ParticleDraw(const Camera& camera);
	void OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) override;
	void UpdateCollision();
	Vector3 MakeEulerAngle(const Mat4x4& mat);
	float overSize_;
	Player* player_;
	std::unique_ptr<ModelInstance> goal_;
	std::unique_ptr<ModelInstance> leftBeach_;
	std::unique_ptr<ModelInstance> rightBeach_;
	Camera* camera_;
	Vector3 pos_;

	std::array<std::unique_ptr<Particle>, 4llu> particles_;
	float particleCoolTime_;
	float particleCount_;

	std::pair<float, float> particleCoolTimeRange_;
	std::pair<Vector3, Vector3> emitterRange_;

	bool isParticleStart_;
	class Audio* fireworkSound_ = nullptr;

	float subtractionVelocity_;
	float subtractionCannonVelocity_;
	float subtractionSailVelocity_;

	bool isHit_;
};