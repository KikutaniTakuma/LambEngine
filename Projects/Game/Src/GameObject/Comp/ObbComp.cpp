#include "ObbComp.h"
#include "../Manager/CompCollisionManager.h"
#include <climits>
#include <algorithm>
#include "CameraComp.h"

#ifdef _DEBUG
#include "Drawers/Line/Line.h"
#endif // _DEBUG


std::array<const Vector3, 8> ObbComp::localPositions_ =
std::array<const Vector3, 8>{
	Vector3(-0.5f, -0.5f, -0.5f), // 左下手前
		Vector3(-0.5f, -0.5f, +0.5f), // 左下奥
		Vector3(+0.5f, -0.5f, -0.5f), // 右下手前
		Vector3(+0.5f, -0.5f, +0.5f), // 右下奥

		Vector3(-0.5f, +0.5f, -0.5f), // 左上手前
		Vector3(-0.5f, +0.5f, +0.5f), // 左上奥
		Vector3(+0.5f, +0.5f, -0.5f), // 右上手前
		Vector3(+0.5f, +0.5f, +0.5f)  // 右上奥
};
std::array<const Vector3, 3> ObbComp::localOrientations_ = {
		Vector3(1.0f, 0.0f, 0.0f),
		Vector3(0.0f, 1.0f, 0.0f),
		Vector3(0.0f, 0.0f, 1.0f)
};

void ObbComp::Init()
{
	CompCollisionManager::GetInstance()->Set(this);
	transformComp_ = object_.AddComp<TransformComp>();
	positions_ = std::make_unique<std::array<Vector3, 8>>();
	orientations_ = std::make_unique<std::array<Vector3, 3>>();
#ifdef _DEBUG
	color_ = std::numeric_limits<uint32_t>::max();
#endif // _DEBUG
}

void ObbComp::Finalize() {
	CompCollisionManager::GetInstance()->Erase(this);
}

void ObbComp::FirstUpdate()
{
	currentCollisionTag_ = "";
	UpdatePosAndOrient();
#ifdef _DEBUG
	color_ = std::numeric_limits<uint32_t>::max();
#endif // _DEBUG
}

void ObbComp::Event() {
	UpdatePosAndOrient();
}

void ObbComp::UpdatePosAndOrient()
{
	Mat4x4 worldMatrix;
	if (isScaleEffect_) {
		worldMatrix = transformComp_->GetWorldMatrix();
	}
	else {
		Vector3 wScale;
		Quaternion rotate;
		Vector3 translate;
		transformComp_->GetWorldMatrix().Decompose(wScale, rotate, translate);
		worldMatrix = Mat4x4::MakeAffin(Vector3::kIdentity, rotate, translate);
	}


	for (size_t i = 0; i < localPositions_.size(); i++) {
		positions_->at(i) = localPositions_.at(i) * Mat4x4::MakeAffin(scale, Vector3(), center) * worldMatrix;
	}

	Quaternion&& rotate = worldMatrix.GetRotate();

	for (size_t i = 0; i < localOrientations_.size(); i++) {
		orientations_->at(i) = localOrientations_.at(i) * rotate;
	}
}

void ObbComp::Draw([[maybe_unused]] CameraComp* cameraComp) {
#ifdef _DEBUG
	const Mat4x4& viewProjection = cameraComp->GetCameraMatrix();

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

	Mat4x4 worldMatrix;
	if (isScaleEffect_) {
		worldMatrix = transformComp_->GetWorldMatrix();
	}
	else {
		Vector3 wScale;
		Quaternion rotate;
		Vector3 translate;
		transformComp_->GetWorldMatrix().Decompose(wScale, rotate, translate);
		worldMatrix = Mat4x4::MakeAffin(Vector3::kIdentity, rotate, translate);
	}

	for (size_t i = 0llu; i < orientations_->size(); i++) {
		Line::Draw(
			transformComp_->translate,
			localOrientations_.at(i) * 0.5f * worldMatrix,
			viewProjection,
			Vector4{ localOrientations_.at(i), 1.0f }.GetColorRGBA()
		);
	}
#endif // _DEBUG
}

bool ObbComp::IsCollision(Vector3 pos, float radius)
{
	if (scale == float32_t3::kZero or (transformComp_->scale == float32_t3::kZero and isScaleEffect_) or radius == 0.0f) {
		return false;
	}

	constexpr int32_t min = 0;
	constexpr int32_t max = 1;

	if (not isCollision_.OnEnter()) {
		isCollision_ = false;
	}

	std::array<Vector3, 2> positions = {
		transformComp_->scale * 0.5f, // 左下手前

		-transformComp_->scale * 0.5f // 右上奥
	};

	const Mat4x4& worldMatrix = transformComp_->GetWorldMatrix();
	Quaternion&& rotate = worldMatrix.GetRotate();
	Vector3&& translate = worldMatrix.GetTranslate();

	pos *= Mat4x4::MakeAffin(Vector3::kIdentity, rotate, translate).Inverse();

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
	if (scale == float32_t3::kZero or (transformComp_->scale == float32_t3::kZero and isScaleEffect_)
		or other->scale == float32_t3::kZero or (other->transformComp_->scale == float32_t3::kZero and other->isScaleEffect_)) {
		return false;
	}

	float length = 0.0f, otherLength = 0.0f;
	float min = 0.0f, max = 0.0f, otherMin = 0.0f, otherMax = 0.0f;

	std::array<float, 8> projectLength{};
	std::array<float, 8> otherProjectLength{};

	if (not isCollision_.OnEnter()) {
		isCollision_ = false;
	}
	if (not other->isCollision_.OnEnter()) {
		other->isCollision_ = false;
	}

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
	if (scale == float32_t3::kZero or (transformComp_->scale == float32_t3::kZero and isScaleEffect_)
		or other->scale == float32_t3::kZero or (other->transformComp_->scale == float32_t3::kZero and other->isScaleEffect_)) {
		pushVector = float32_t3::kZero;
		return false;
	}

	float length = 0.0f, otherLength = 0.0f;
	float min = 0.0f, max = 0.0f, otherMin = 0.0f, otherMax = 0.0f;

	std::array<float, 8> projectLength{};
	std::array<float, 8> otherProjectLength{};

	float minOverLap = std::numeric_limits<float>::max();
	Vector3 overLapAxis;

	if (not isCollision_.OnEnter()) {
		isCollision_ = false;
	}
	if (not other->isCollision_.OnEnter()) {
		other->isCollision_ = false;
	}

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

	float dot = (other->transformComp_->translate - transformComp_->translate).Dot(overLapAxis.Normalize());
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

bool ObbComp::IsCollision(const Vector3& start, const Vector3& end)
{
	if (scale == float32_t3::kZero or (transformComp_->scale == float32_t3::kZero and isScaleEffect_)) {
		return false;
	}

	if (not isCollision_.OnEnter()) {
		isCollision_ = false;
	}

	static constexpr float kEpsilon = static_cast<float32_t>(1.175494e-37);

	Vector3 orientarionLength = Vector3::kIdentity * 0.5f;
	Mat4x4 worldMatrix;
	if (isScaleEffect_) {
		worldMatrix = transformComp_->GetWorldMatrix();
	}
	else {
		Vector3 wScale;
		Quaternion rotate;
		Vector3 translate;
		transformComp_->GetWorldMatrix().Decompose(wScale, rotate, translate);
		worldMatrix = Mat4x4::MakeAffin(Vector3::kIdentity, rotate, translate);
	}
	Mat4x4&& invWorldMat = (Mat4x4::MakeAffin(scale, Vector3(), center) * worldMatrix).Inverse();

	Vector3 localStart = start * invWorldMat;
	Vector3 localEnd = end * invWorldMat;

	Vector3 worldMax = orientarionLength;
	Vector3 worldMin = -orientarionLength;

	Vector3 b = (localEnd - localStart);
	if (b.x == 0.0f && b.y == 0.0f && b.z == 0.0f) {
		return false;
	}

	float tMinX = (worldMin.x - localStart.x) / b.x;
	float tMaxX = (worldMax.x - localStart.x) / b.x;

	float tNearX = std::min(tMinX, tMaxX);
	float tFarX = std::max(tMinX, tMaxX);

	float tMinY = (worldMin.y - localStart.y) / b.y;
	float tMaxY = (worldMax.y - localStart.y) / b.y;

	float tNearY = std::min(tMinY, tMaxY);
	float tFarY = std::max(tMinY, tMaxY);

	float tMinZ = (worldMin.z - localStart.z) / b.z;
	float tMaxZ = (worldMax.z - localStart.z) / b.z;

	float tNearZ = std::min(tMinZ, tMaxZ);
	float tFarZ = std::max(tMinZ, tMaxZ);

	float tMin = std::max(std::max(tNearX, tNearY), tNearZ);
	float tMax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tMin <= tMax and (Lamb::Between(tMin, 0.0f, 1.0f) and Lamb::Between(tMax, 0.0f, 1.0f))) {
		isCollision_ = true;
#ifdef _DEBUG
		color_ = 0xff0000ff;
#endif // _DEBUG
		return true;
	}


	return false;
}

bool ObbComp::Collision(ObbComp* const other) {
	return IsCollision(other);
}


TransformComp& ObbComp::GetTransformComp()
{
	return *transformComp_;
}

const TransformComp& ObbComp::GetTransformComp() const
{
	return *transformComp_;
}

void ObbComp::SetCollisionTag(const std::string& collisionTag) {
	if (not collisionTags_.contains(collisionTag)) {
		collisionTags_.insert(collisionTag);
	}
}

void ObbComp::EraseCollisionTag(const std::string& collisionTag) {
	if (collisionTags_.contains(collisionTag)) {
		collisionTags_.erase(collisionTag);
	}
}

void ObbComp::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {

		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
		for (auto& i : collisionTags_) {
			if (ImGui::Button("erase")) {
				collisionTags_.erase(i);
				break;
			}
			ImGui::SameLine();
			ImGui::Text("tag : % s", i.c_str());
		}
		ImGui::EndChild();

		inputTag_.resize(32);
		ImGui::DragFloat3("scale", scale.data(), 0.01f);
		ImGui::DragFloat3("center", center.data(), 0.01f);
		ImGui::Checkbox("transformの大きさの影響", &isScaleEffect_);
		ImGui::Text("current collision tag : %s", currentCollisionTag_.c_str());
		ImGui::InputText(
			"タグ",
			inputTag_.data(),
			inputTag_.size()
		);
		if (ImGui::Button("タグ追加")) {
			std::string addtag;
			for (auto& i : inputTag_) {
				if (i == '\0') {
					break;
				}
				addtag.push_back(i);
			}
			collisionTags_.insert(addtag);
		}
		ImGui::TreePop();
	}
#endif // _DEBUG
}

void ObbComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["scale"] = nlohmann::json::array();
	for (auto& i : scale) {
		json["scale"].push_back(i);
	}
	json["center"] = nlohmann::json::array();
	for (auto& i : center) {
		json["center"].push_back(i);
	}
	json["collsiionTags"] = nlohmann::json::array();
	for (auto& i : collisionTags_) {
		json["collsiionTags"].push_back(i);
	}
	json["isScaleEffect"] = isScaleEffect_;
}

void ObbComp::Load(nlohmann::json& json)
{
	for (size_t i = 0; i < json["scale"].size(); i++) {
		scale[i] = json["scale"][i].get<float32_t>();
	}
	for (size_t i = 0; i < json["center"].size(); i++) {
		center[i] = json["center"][i].get<float32_t>();
	}
	collisionTags_.clear();
	collisionTags_.reserve(json["collsiionTags"].size());
	for (size_t i = 0; i < json["collsiionTags"].size(); i++) {
		collisionTags_.insert(json["collsiionTags"][i].get<std::string>());
	}
	if (json.contains("isScaleEffect")) {
		isScaleEffect_ = json["isScaleEffect"].get<bool>();
	}
}

const std::string& ObbComp::GetCurrentCollisionTag() const {
	return currentCollisionTag_;
}

void ObbComp::SetEnableScaleEffect(bool isScaleEffect) {
	isScaleEffect_ = isScaleEffect;
}
