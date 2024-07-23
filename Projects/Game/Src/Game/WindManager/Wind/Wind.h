#pragma once
#include <memory>

#include "Game/CollisionManager/Collider/Collider.h"
#include "Drawers/Model/Model.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Math/Mat4x4.h"

class Camera;
class Player;
class Wind : public Collider {
public:
	Wind();
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos, const Vector3& vector = Vector3::kZero);
	void ResourceUpdate();
	void Update();
	void Draw(const Camera& camera);

	void SetPlayer(Player* player) { player_ = player; }
	const Vector3& GetScale() { return modelInstance_->transform.scale; }
	void SetScale(const Vector3& scale);
	const Quaternion& GetRotate() { return modelInstance_->transform.rotate; }
	void SetRotate(const Vector3& rotate);
	const Vector3& GetPosition() { return modelInstance_->transform.translate; }
	void SetPosition(const Vector3& pos);
	const Vector3& GetVector() { return vector_; }
	void SetVector(const Vector3& vector) { vector_ = vector; }
private:
	void OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) override;
	Lamb::SafePtr<Model> model_;
	std::unique_ptr<Model::Instance> modelInstance_;

	Player* player_;

	Vector3 vector_;
};