#include "Wind.h"

#include "Camera/Camera.h"
#include "Game/CollisionManager/Collision/Collision.h"
#include "Game/Player/Player.h"

static const double PI = 3.14159265358979323846;

Wind::Wind() {
	model_ = std::make_unique<ModelInstance>();
	model_->Load("./Resources/Block/Block.obj");
	debugArrowModel_ = std::make_unique<ModelInstance>();
	debugArrowModel_->Load("./Resources/DebugArrow/DebugArrow.obj");
}

void Wind::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos, const Vector3& vector) {
	model_->scale = scale;
	model_->rotate = rotate;
	model_->pos = pos;
	debugArrowModel_->SetParent(model_.get());
	debugArrowModel_->pos;
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
	debugArrowModel_->rotate.y = angle;
	vector_ = vector;
	Collider::SetColliderType(Collider::Type::kTrriger);
	Collider::SetColliderAttribute(Collider::Attribute::kOther);
	Collider::InitializeCollision(model_->scale, model_->rotate, model_->pos);
}

void Wind::ResourceUpdate() {
	model_->Update();
	debugArrowModel_->Update();
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
	debugArrowModel_->rotate.y = angle;
	ResourceUpdate();
	color_.at(0) = Vector4ToUint(Vector4::kIdentity);
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Wind::Draw([[maybe_unused]]const Camera& camera) {
	//model_->Draw(viewProjection, cameraPos);
	//debugArrowModel_->Draw(camera.GetViewProjection(), camera.GetPos());
#ifdef USE_IMGUI
	Collider::DebugDraw(camera.GetViewProjection());
#endif // USE_IMGUI
}

void Wind::SetScale(const Vector3& scale) {
	model_->scale = scale;
	Collider::SetCollisionScale(scale);
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Wind::SetRotate(const Vector3& rotate) {
	model_->rotate = rotate;
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Wind::SetPosition(const Vector3& pos) {
	model_->pos = pos;
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Wind::OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) {
	if (collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kPlayer &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex))) {
		color_.at(myIndex) = Vector4ToUint(Vector4::kZIdentity);
		player_->SetWindDirection(vector_);
	}
}
