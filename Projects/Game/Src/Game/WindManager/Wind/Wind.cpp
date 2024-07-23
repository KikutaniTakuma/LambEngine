#include "Wind.h"

#include "Camera/Camera.h"
#include "Game/CollisionManager/Collision/Collision.h"
#include "Game/Player/Player.h"

#include "Drawers/DrawerManager.h"

static const double PI = 3.14159265358979323846;

Wind::Wind() {
	DrawerManager::GetInstance()->LoadModel("./Resources/Block/Block.obj");
	model_ = DrawerManager::GetInstance()->GetModel("./Resources/Block/Block.obj");
	modelInstance_ = std::make_unique<Model::Instance>();
}

void Wind::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos, const Vector3& vector) {
	modelInstance_->transform.scale = scale;
	modelInstance_->transform.rotate = Quaternion::EulerToQuaternion(rotate);
	modelInstance_->transform.translate = pos;
	float angle = 0.0f;
	Vector3 from = { 0.0f,0.0f,1.0f };
	Vector3 to = vector;
	float dot = from.Dot(to);
	Vector2 Vector2From = { from.x ,from.z };
	Vector2 Vector2To = { to.x ,to.z };
	if (dot >= 1.0f) {
		angle = 0.0f;
	}
	else if (dot <= -1.0f) {
		angle = 180.0f * static_cast<float>(PI) / 180.0f;
	}
	else if (Vector2From.Cross(Vector2To) > 0) {
		angle = -std::acosf(dot);
	}
	else {
		angle = std::acosf(dot);
	}
	vector_ = vector;
	Collider::SetColliderType(Collider::Type::kTrriger);
	Collider::SetColliderAttribute(Collider::Attribute::kOther);
	Collider::InitializeCollision(modelInstance_->transform.scale, modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Wind::ResourceUpdate() {
	modelInstance_->transform.CalcMatrix();
}

void Wind::Update() {

	float angle = 0.0f;
	Vector3 from = { 0.0f,0.0f,1.0f };
	Vector3 to = vector_;
	float dot = from.Dot(to);
	Vector2 Vector2From = { from.x ,from.z };
	Vector2 Vector2To = { to.x ,to.z };
	if (dot >= 1.0f) {
		angle = 0.0f;
	}
	else if (dot <= -1.0f) {
		angle = 180.0f * static_cast<float>(PI) / 180.0f;
	}
	else if (Vector2From.Cross(Vector2To) > 0) {
		angle = -std::acosf(dot);
	}
	else {
		angle = std::acosf(dot);
	}
	ResourceUpdate();
	color_.at(0) = Vector4ToUint(Vector4::kIdentity);
	Collider::UpdateCollision(modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Wind::Draw(const Camera& camera) {
	camera;
	//model_->Draw(viewProjection, cameraPos);
	//debugArrowModel_->Draw(camera.GetViewProjection(), camera.GetPos());
#ifdef _DEBUG
	Collider::DebugDraw(camera.GetViewProjection());
#endif // _DEBUG
}

void Wind::SetScale(const Vector3& scale) {
	modelInstance_->transform.scale = scale;
	Collider::SetCollisionScale(scale);
	Collider::UpdateCollision(modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Wind::SetRotate(const Vector3& rotate) {
	modelInstance_->transform.rotate = rotate;
	Collider::UpdateCollision(modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Wind::SetPosition(const Vector3& pos) {
	modelInstance_->transform.translate = pos;
	Collider::UpdateCollision(modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Wind::OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) {
	if (collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kPlayer &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex))) {
		color_.at(myIndex) = Vector4ToUint(Vector4::kZIdentity);
		player_->SetWindDirection(vector_);
	}
}
