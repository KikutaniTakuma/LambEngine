#pragma once
#include <memory>

#include "Game/CollisionManager/Collider/Collider.h"
#include "Drawers/Model/Model.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Math/Mat4x4.h"
#include "Drawers/Particle/Particle.h"

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
	const Vector3& GetScale() { return goalInstance_->transform.scale; }
	void SetScale(const Vector3& scale);
	const Quaternion& GetRotate() { return goalInstance_->transform.rotate; }
	void SetRotate(const Vector3& rotate);
	const Vector3& GetPosition() { return goalInstance_->transform.translate; }
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
	Lamb::SafePtr<Model> goal_;
	std::unique_ptr<Model::Instance> goalInstance_;
	Lamb::SafePtr<Model> leftBeach_;
	std::unique_ptr<Model::Instance> leftBeachInstance_;
	Lamb::SafePtr<Model> rightBeach_;
	std::unique_ptr<Model::Instance> rightBeachInstance_;
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