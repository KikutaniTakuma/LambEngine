#include "Fishes.h"
#include "Utils/Random.h"
#include "Utils/EngineInfo.h"

#include "Drawer/DrawerManager.h"

#include "Drawer/Line/Line.h"

const Vector3& Fish::GetDirection() const {
	return direction_;
}

void Fish::Init(const Vector3& min, const Vector3& max) {
	direction_ = Lamb::Random(Vector3(-1.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 1.0f));

	speed_ = 30.0f;

	posision_ = Lamb::Random(min, max);
}

void Fish::Update() {
	collisionCount_ = 0.0f;
	minNearLength = std::numeric_limits<float>::max();

	direction_ = direction_.Normalize();

	rotate_ = Quaternion::DirectionToDirection(Vector3::kZIdentity, direction_.Normalize());

	posision_ += direction_ * speed_ * Lamb::DeltaTime();

	avgDirection_ = direction_;
	centerOfGravityDirection_ = Vector3::kZero;
}

bool Fish::IsCollision(const Fish& other) const {
	Vector3 toOther = other.posision_ - posision_;

	// 円の範囲内に入っているか
	if (toOther.Length() > collisionRange_) {
		return false;
	}

	// なす角cos
	float theata = direction_.Dot(toOther.Normalize());

	// 反対を向いている
	if (theata < 0.0f) {
		return false;
	}

	// 扇自体のcos
	float fanTheata = std::cos(fov_);

	// 点が扇の範囲内にあるか
	if (fanTheata > theata) {
		return false;
	}

	return true;
}

void Fish::CalcAvoidWallDirection(const Vector3& min, const Vector3& max) {

	avoidWallDirection_ = direction_;
	if ((posision_.x - wallCollisionRange_) < min.x) {
		speed_ = 0.0f;
		weight_.z = 1.0f - std::max(1.0f, std::abs(posision_.x - min.x) / wallCollisionRange_);
		avoidWallDirection_.x *= -1.0f;
	}
	if ((posision_.z - wallCollisionRange_) < min.z) {
		speed_ = 0.0f;
		weight_.z = std::max(weight_.z, 1.0f - std::max(1.0f, std::abs(posision_.z - min.z) / wallCollisionRange_));
		avoidWallDirection_.z *= -1.0f;
	}
	if (max.x < (posision_.x + wallCollisionRange_)) {
		speed_ = 0.0f;
		weight_.z = std::max(weight_.z, 1.0f - std::max(1.0f, std::abs(max.x - posision_.x) / wallCollisionRange_));
		avoidWallDirection_.x *= -1.0f;
	}
	if (max.z < (posision_.z + wallCollisionRange_)) {
		speed_ = 0.0f;
		weight_.z = std::max(weight_.z, 1.0f - std::max(1.0f, std::abs(max.z - posision_.z) / wallCollisionRange_));
		avoidWallDirection_.z *= -1.0f;
	}

	if (avoidWallDirection_ == direction_) {
		weight_.z = 0.0f;
	}
}

void Fish::CalcAvoidDirection(const Fish& other) {
	Vector3 toOther = other.posision_ - posision_;
	float length = toOther.Length();

	// 最も近いものを入れる
	if (length < minNearLength) {
		minNearLength = length;
	}
	// 早期リターン
	else {
		return;
	}

	avoidDirection_ =  Vector3::ReflectNormal(toOther, other.direction_);
}

void Fish::AddAvgAndCenterOfGravityDirection(const Fish& other) {
	avgDirection_ += other.direction_;
	centerOfGravityDirection_ += other.posision_;
	collisionCount_++;
}

void Fish::CalcAvgAndCenterOfGravityDirection() {
	if (collisionCount_ == 0.0f) {
		return;
	}
	avgDirection_ /= collisionCount_;
	centerOfGravityDirection_ /= collisionCount_;
}

void Fish::CalcDirection() {
	//weight_ = weight_.Normalize();
	/*if (avoidDirection_ != Vector3::kZero and avgDirection_ != Vector3::kZero) {
		direction_ = Vector3::Lerp(avoidDirection_, avgDirection_, weight_.x);
	}
	if (centerOfGravityDirection_ != Vector3::kZero) {
		direction_ = Vector3::Lerp(direction_, centerOfGravityDirection_, weight_.y);
	}*/
	direction_ = Vector3::Lerp(direction_, avoidWallDirection_, weight_.z);
}

Mat4x4 Fish::CreateWorldMatrix() const {
	return Mat4x4::MakeAffin(Vector3::kIdentity, rotate_, posision_);
}


void Fishes::Init(size_t numFishes) {
	rangeMin_ = { -200.0f * 0.25f, 1.0f, -100.0f * 0.25f };
	rangeMax_ = { 200.0f * 0.25f, 1.0f, 300.0f * 0.25f };

	fishes_.reserve(numFishes);
	for (size_t i = 0; i < numFishes; ++i) {
		fishes_.push_back(std::make_unique<Fish>());
		fishes_.back()->Init(rangeMin_, rangeMax_);
	}

	DrawerManager::GetInstance()->LoadModel("./Resources/Fish/Fish.gltf");
	model_ = DrawerManager::GetInstance()->GetModel("./Resources/Fish/Fish.gltf");
}

void Fishes::Update()
{
	for (auto& i : fishes_) {
		i->Update();
		i->CalcAvoidWallDirection(rangeMin_, rangeMax_);
	}

	for (auto y = fishes_.begin(); y != fishes_.end(); ++y) {
		for (auto x = fishes_.begin(); x != fishes_.end(); ++x) {
			if (y == x) {
				continue;
			}

			// 視野範囲ないに入っていたら
			if ((*x)->IsCollision(**y)) {
				// 避ける
				// 最も近いもの進行方向と最も近いものの方向でよける向きを求める
				// 避ける方向が決まったら距離に応じた重みを求める
				(*x)->CalcAvoidDirection(**y);

				// 近くの人の方向平均を求める
				// 近くの人の重心を求める
				(*x)->AddAvgAndCenterOfGravityDirection(**y);


				// 各軸位置の平均を求めればおｋ
				(*x)->CalcAvgAndCenterOfGravityDirection();

				// 最終的な方向を決める
				(*x)->CalcDirection();
			}
		}
	}

}

void Fishes::Draw(const Mat4x4& cameraMat) {
	for (auto& i : fishes_) {
		model_->Draw(
			i->CreateWorldMatrix(),
			cameraMat,
			0xffffffff,
			BlendType::kNone
		);
	}

	Line::Draw(
		rangeMin_,
		Vector3(rangeMax_.x, rangeMin_.y, rangeMin_.z),
		cameraMat
	);
	Line::Draw(
		Vector3(rangeMax_.x, rangeMin_.y, rangeMin_.z),
		Vector3(rangeMax_.x, rangeMin_.y, rangeMax_.z),
		cameraMat
	);

	Line::Draw(
		Vector3(rangeMax_.x, rangeMin_.y, rangeMax_.z),
		Vector3(rangeMin_.x, rangeMin_.y, rangeMax_.z),
		cameraMat
	);

	Line::Draw(
		Vector3(rangeMin_.x, rangeMin_.y, rangeMax_.z),
		rangeMin_,
		cameraMat
	);
}

