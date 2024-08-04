#include "QuadComp.h"
#include "TransformComp.h"
#include "Input/Mouse/Mouse.h"

#include "Drawers/Line/Line.h"
#include "CameraComp.h"
#include "Utils/EngineInfo.h"

void QuadComp::Init()
{
	transformComp_ = object_.AddComp<TransformComp>();
}

void QuadComp::FirstUpdate()
{
	isCollision_ = false;
#ifdef _DEBUG
	color_ = 0xffffffff;
#endif // _DEBUG
	Vector2 clientSize = Lamb::ClientSize();
	IsCollisionMouse(Mat4x4::MakeTranslate({ 0.0f, 0.0f, 10.0f }), Mat4x4::MakeOrthographic(clientSize.x, clientSize.y, 1.0f, 100.0f));
}

bool QuadComp::IsCollision(const Vector2& pos)
{
	Vector3 min = transformComp_->translate - (transformComp_->scale * 0.5f);
	Vector3 max = transformComp_->translate + (transformComp_->scale * 0.5f);

	return (min.x < pos.x and pos.x < max.x) and (min.y < pos.y and pos.y < max.y);
}

bool QuadComp::IsCollisionMouse(const Mat4x4& view, const Mat4x4& ndc) {
	Vector2 clientSize = Lamb::ClientSize();
	Vector3 mousePos = Mouse::GetInstance()->GetPos();
	mousePos *= (view * ndc * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f)).Inverse();
	Vector3 min = transformComp_->translate - (transformComp_->scale * 0.5f);
	Vector3 max = transformComp_->translate + (transformComp_->scale * 0.5f);

	isCollision_ = (min.x < mousePos.x and mousePos.x < max.x) and (min.y < mousePos.y and mousePos.y < max.y);

#ifdef _DEBUG
	if (isCollision_) {
		color_ = 0xff0000ff;
	}
#endif // _DEBUG

	return isCollision_.operator bool();
}

void QuadComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
}

void QuadComp::Load([[maybe_unused]]nlohmann::json& json)
{

}

void QuadComp::Draw([[maybe_unused]]CameraComp* cameraComp) {
#ifdef _DEBUG
	Vector3 min = transformComp_->translate - (transformComp_->scale * 0.5f);
	Vector3 max = transformComp_->translate + (transformComp_->scale * 0.5f);

	Vector2 clientSize = Lamb::ClientSize();
	Mat4x4 cameraMatrix = Mat4x4::MakeTranslate(Vector3::kZIdentity * 10.0f) * Mat4x4::MakeOrthographic(clientSize.x, clientSize.y, 1.0f, 100.0f);

	Line::Draw(
		min,
		Vector3(min.x, max.y, min.z),
		cameraMatrix,
		color_,
		false
	);

	Line::Draw(
		Vector3(min.x, max.y, min.z),
		Vector3(max.x, max.y, min.z),
		cameraMatrix,
		color_,
		false
	);
	Line::Draw(
		Vector3(max.x, max.y, min.z),
		Vector3(max.x, min.y, min.z),
		cameraMatrix,
		color_,
		false
	);
	Line::Draw(
		Vector3(max.x, min.y, min.z),
		min,
		cameraMatrix,
		color_,
		false
	);
#endif // _DEBUG
}

const Lamb::Flg& QuadComp::GetIsCollision() const
{
	return isCollision_;
}
