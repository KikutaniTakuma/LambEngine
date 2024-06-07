#include "ObbComp.h"
#include <climits>
#include <algorithm>

#ifdef _DEBUG
#include "Drawers/Line/Line.h"
#endif // _DEBUG


std::unique_ptr<std::array<const Vector3, 8>> ObbComp::localPositions_ = std::make_unique<std::array<const Vector3, 8>>(
	std::array<const Vector3, 8>{
		Vector3(-0.5f, -0.5f, -0.5f), // 左下手前
		Vector3(-0.5f, -0.5f, +0.5f), // 左下奥
		Vector3(+0.5f, -0.5f, -0.5f), // 右下手前
		Vector3(+0.5f, -0.5f, +0.5f), // 右下奥

		Vector3(-0.5f, +0.5f, -0.5f), // 左上手前
		Vector3(-0.5f, +0.5f, +0.5f), // 左上奥
		Vector3(+0.5f, +0.5f, -0.5f), // 右上手前
		Vector3(+0.5f, +0.5f, +0.5f)  // 右上奥
	}
);
std::unique_ptr<std::array<const Vector3, 3>> ObbComp::localOrientations_ = std::make_unique<std::array<const Vector3, 3>>(
	std::array<const Vector3, 3>{
		Vector3(1.0f, 0.0f, 0.0f),
		Vector3(0.0f, 1.0f, 0.0f),
		Vector3(0.0f, 0.0f, 1.0f)
	}
);

void ObbComp::Init()
{
	positions_ = std::make_unique<std::array<Vector3, 8>>();
	orientations_ = std::make_unique<std::array<Vector3, 3>>();
#ifdef _DEBUG
	color_ = std::numeric_limits<uint32_t>::max();
#endif // _DEBUG
}

void ObbComp::UpdatePosAndOrient()
{
	Mat4x4&& worldMatrix = transform.GetMatrix();

	for (size_t i = 0; i < localPositions_->size(); i++) {
		positions_->at(i) = localPositions_->at(i) * worldMatrix;
	}

	for (size_t i = 0; i < localOrientations_->size(); i++) {
		orientations_->at(i) = localOrientations_->at(i) * transform.rotate;
	}

	isCollision_ = false;
#ifdef _DEBUG
	color_ = std::numeric_limits<uint32_t>::max();
#endif // _DEBUG
}

void ObbComp::Draw() {
#ifdef _DEBUG
	const Mat4x4& viewProjection = object_.GetCamera()->GetViewProjection();

	Line::Draw(
		(*positions_)[0],
		(*positions_)[1],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[0],
		(*positions_)[2],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[0],
		(*positions_)[4],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[3],
		(*positions_)[1],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[3],
		(*positions_)[2],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[3],
		(*positions_)[7],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[5],
		(*positions_)[4],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[5],
		(*positions_)[7],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[5],
		(*positions_)[1],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[6],
		(*positions_)[4],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[6],
		(*positions_)[7],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[6],
		(*positions_)[2],
		viewProjection,
		color_
	);

	Mat4x4&& worldMatrix = transform.GetMatrix();
	for (size_t i = 0llu; i < orientations_->size(); i++) {
		Line::Draw(
			transform.translate,
			localOrientations_->at(i) * 0.5f * worldMatrix,
			viewProjection,
			Vector4{ localOrientations_->at(i), 1.0f }.GetColorRGBA()
		);
	}
#endif // _DEBUG
}

bool ObbComp::IsCollision(Vector3 pos, float radius)
{
	constexpr int32_t min = 0;
	constexpr int32_t max = 1;

	std::array<Vector3, 2> positions = {
		transform.scale * 0.5f, // 左下手前

		-transform.scale * 0.5f // 右上奥
	};

	pos *= Mat4x4::MakeAffin(Vector3::kIdentity, transform.rotate, transform.translate).Inverse();

	Vector3 closestPoint = {
		std::clamp(pos.x, positions[min].x,positions[max].x),
		std::clamp(pos.y, positions[min].y,positions[max].y),
		std::clamp(pos.z, positions[min].z,positions[max].z)
	};

	float distance = (closestPoint - pos).Length();

	if (distance <= radius) {
		isCollision_ = true;
#ifdef _DEBUG
		color_ = 0xff0000ff;
#endif // _DEBUG

		return true;
	}

	return false;
}

bool ObbComp::IsCollision(ObbComp* const other)
{
	float length = 0.0f, otherLength = 0.0f;
	float min = 0.0f, max = 0.0f, otherMin = 0.0f, otherMax = 0.0f;

	std::array<float, 8> projectLength{};
	std::array<float, 8> otherProjectLength{};

	auto isSepatateAxis = [&](const Vector3& separationAxis)->bool {
		for (size_t i = 0; i < 8llu; i++) {
			projectLength[i] = positions_->at(i).Dot(separationAxis);
			otherProjectLength[i] = other->positions_->at(i).Dot(separationAxis);
		}

		min = *std::min_element(projectLength.begin(), projectLength.end());
		max = *std::max_element(projectLength.begin(), projectLength.end());

		otherMin = *std::min_element(otherProjectLength.begin(), otherProjectLength.end());
		otherMax = *std::max_element(otherProjectLength.begin(), otherProjectLength.end());

		length = max - min;
		otherLength = otherMax - otherMin;

		float sumSpan = length + otherLength;
		float longSpan = std::max(max, otherMax) - std::min(min, otherMin);

		if (sumSpan < longSpan) {
			return true;
		}

		return false;
		};

	// 分離軸(面法線)
	for (const auto& separationAxis : *orientations_) {
		if (isSepatateAxis(separationAxis)) {
			return false;
		}
	}
	for (const auto& separationAxis : *(other->orientations_)) {
		if (isSepatateAxis(separationAxis)) {
			return false;
		}
	}

	// 各軸のクロス積
	for (const auto& orientation : *orientations_) {
		for (const auto& otherOrientation : *other->orientations_) {
			if (isSepatateAxis(orientation.Cross(otherOrientation))) {
				return false;
			}
		}
	}

	isCollision_ = true;
	other->isCollision_ = true;

#ifdef _DEBUG
	color_ = 0xff0000ff;
	other->color_ = 0xff0000ff;
#endif // _DEBUG

	return true;
}

bool ObbComp::IsCollision(ObbComp* const other, Vector3& pushVector)
{
	float length = 0.0f, otherLength = 0.0f;
	float min = 0.0f, max = 0.0f, otherMin = 0.0f, otherMax = 0.0f;

	std::array<float, 8> projectLength{};
	std::array<float, 8> otherProjectLength{};

	float minOverLap = std::numeric_limits<float>::max();
	Vector3 overLapAxis;

	auto isSepatateAxisAndClacOverLap = [&](const Vector3& separationAxis)->bool {
		if (separationAxis == Vector3::kZero) {
			return false;
		}

		for (size_t i = 0; i < 8llu; i++) {
			projectLength[i] = positions_->at(i).Dot(separationAxis);
			otherProjectLength[i] = other->positions_->at(i).Dot(separationAxis);
		}

		min = *std::min_element(projectLength.begin(), projectLength.end());
		max = *std::max_element(projectLength.begin(), projectLength.end());

		otherMin = *std::min_element(otherProjectLength.begin(), otherProjectLength.end());
		otherMax = *std::max_element(otherProjectLength.begin(), otherProjectLength.end());

		length = max - min;
		otherLength = otherMax - otherMin;

		float sumSpan = length + otherLength;
		float longSpan = std::max(max, otherMax) - std::min(min, otherMin);
		float overLap = sumSpan - longSpan;

		if (sumSpan < longSpan) {
			return true;
		}

		if (overLap < minOverLap) {
			overLapAxis = separationAxis;
			minOverLap = overLap;
		}

		return false;
		};

	// 分離軸(面法線)
	for (const auto& separationAxis : *orientations_) {
		if (isSepatateAxisAndClacOverLap(separationAxis)) {
			return false;
		}
	}
	for (const auto& separationAxis : *(other->orientations_)) {
		if (isSepatateAxisAndClacOverLap(separationAxis)) {
			return false;
		}
	}

	// 各軸のクロス積
	for (const auto& orientation : *orientations_) {
		for (const auto& otherOrientation : *other->orientations_) {
			Vector3&& separationAxis = orientation.Cross(otherOrientation);
			if (isSepatateAxisAndClacOverLap(separationAxis)) {
				return false;
			}
		}
	}

	float dot = (other->transform.translate - transform.translate).Dot(overLapAxis.Normalize());
	if (not (0.0f < dot)) {
		overLapAxis = -overLapAxis;
	}
	pushVector = overLapAxis.Normalize() * minOverLap;

	isCollision_ = true;
	other->isCollision_ = true;
#ifdef _DEBUG
	color_ = 0xff0000ff;
	other->color_ = 0xff0000ff;
#endif // _DEBUG

	return true;
}
