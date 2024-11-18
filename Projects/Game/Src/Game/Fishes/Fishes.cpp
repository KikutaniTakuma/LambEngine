#include "Fishes.h"
#include "Utils/Random.h"
#include "Utils/EngineInfo.h"

#include "Drawer/DrawerManager.h"

#include "Drawer/Line/Line.h"

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#endif // USE_DEBUG_CODE


const Vector3& Fish::GetDirection() const {
	return direction_;
}

void Fish::Init(const Vector3& min, const Vector3& max) {
	const std::array colors = {
		0x00ffccff_u32, // エメラルドグリーン
		0x0077ffff_u32, // ディープブルー
		0xff6f31ff_u32, // サンゴオレンジ
		0xfff700ff_u32, // レモンイエロー
		0x33ccffff_u32, // アクアブルー
		0xff33ccff_u32, // フクシアピンク
		0x8a2be2ff_u32, // バイオレットパープル
		0x66ff66ff_u32, // グリーンアップル
		0xff0033ff_u32, // チェリーレッド
		0x40e0d0ff_u32  // ターコイズブルー
	};

	color_ = colors[Lamb::Random(0llu, colors.size() - 1)];

	minRange_ = min;
	maxRange_ = max;
	direction_ = Lamb::Random(Vector3(-1.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 1.0f));

	speed_ = 20.0f;

	posision_ = Lamb::Random(min + (Vector3::kXIdentity + Vector3::kZIdentity) * wallCollisionRange_, max - (Vector3::kXIdentity + Vector3::kZIdentity) * wallCollisionRange_);
}

void Fish::Update() {
	collisionCount_ = 0.0f;
	minNearLength_ = std::numeric_limits<float>::max();

	if (direction_.Length() < 0.1f) {
		direction_ = Lamb::Random(Vector3(-1.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 1.0f));
	}

	direction_ = direction_.Normalize();

	rotate_ = Quaternion::DirectionToDirection(Vector3::kZIdentity, direction_.Normalize());

	posision_ += direction_ * speed_ * Lamb::DeltaTime();

	avgDirection_ = Vector3::kZero;
	centerOfGravityDirection_ = Vector3::kZero;
	avoidDirection_ = Vector3::kZero;
}

bool Fish::IsCollision(const Fish& other) const {
	Vector3 toOther = other.posision_ - posision_;

	// 円の範囲内に入っているか
	if (toOther.Length() > collisionRange) {
		return false;
	}

	// なす角cos
	float theata = direction_.Dot(toOther.Normalize());

	// 反対を向いている
	if (theata < 0.0f) {
		return false;
	}

	// 扇自体のcos
	float fanTheata = std::cos(fov);

	// 点が扇の範囲内にあるか
	if (fanTheata > theata) {
		return false;
	}

	return true;
}

void Fish::CalcAvoidWallDirection(const Vector3& min, const Vector3& max) {

	if ((posision_.x - wallCollisionRange_) < min.x) {
		direction_.x = std::abs(direction_.x);
	}
	if ((posision_.z - wallCollisionRange_) < min.z) {
		direction_.z = std::abs(direction_.z);
	}
	if (max.x < (posision_.x + wallCollisionRange_)) {
		direction_.x = -std::abs(direction_.x);
	}
	if (max.z < (posision_.z + wallCollisionRange_)) {
		direction_.z = -std::abs(direction_.x);
	}


}

void Fish::CalcAvoidDirection(const Fish& other) {
	Vector3 toMe = (posision_ - other.posision_);
	float length = toMe.Length();
	if (length < avoidDistance) {
		return;
	}

	toMe /= length;

	avoidDirection_ += toMe / length;
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
	avgDirection_.y = 0.0f;
	avgDirection_ /= collisionCount_;
	centerOfGravityDirection_.y = 0.0f;
	centerOfGravityDirection_ /= collisionCount_;
}

void Fish::CalcDirection() {
	if (centerOfGravityDirection_ != Vector3::kZero) {
		direction_ += (centerOfGravityDirection_ - Vector3(posision_.x, 0.0f, posision_.z)) * weight.x;
	}
	if (avoidDirection_ != Vector3::kZero) {
		direction_ += avoidDirection_ * weight.y;
	}
	if (avgDirection_ != Vector3::kZero) {
		direction_ += (avgDirection_ - direction_) * weight.z;
	}

	CalcAvoidWallDirection(minRange_, maxRange_);
}

Mat4x4 Fish::CreateWorldMatrix() const {
	return Mat4x4::MakeAffin(Vector3::kIdentity, rotate_, posision_);
}

uint32_t Fish::GetColors() const
{
	return color_;
}


void Fishes::Init(size_t numFishes) {
	rangeMin_ = { -200.0f, -3.0f, -100.0f };
	rangeMax_ = { 200.0f, -3.0f, 300.0f };

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
	Debug();

	for (auto& i : fishes_) {
		i->weight = weight_;
		i->avoidDistance = avoidDistance_;
		i->fov = fov_;
		i->collisionRange = collisionRange_;

		i->Update();
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
			}
		}
	}

	// 最終的な方向を決める
	for (auto& i : fishes_) {
		i->CalcAvgAndCenterOfGravityDirection();
		i->CalcDirection();
	}

}

void Fishes::Draw(const Mat4x4& cameraMat) {
	for (auto& i : fishes_) {
		model_->Draw(
			i->CreateWorldMatrix(),
			cameraMat,
			i->GetColors(),
			BlendType::kNone
		);
	}

#ifdef USE_DEBUG_CODE
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
#endif // USE_DEBUG_CODE
}

void Fishes::Debug() {
#ifdef USE_DEBUG_CODE
if(ImGui::TreeNode("Fishes")) {
	ImGui::DragFloat("重心", &weight_[0], 0.01f);
	ImGui::DragFloat("離れる", &weight_[1], 0.01f);
	ImGui::DragFloat("平均方向", &weight_[2], 0.01f);
	ImGui::DragFloat("離れるのを適用する距離", &avoidDistance_, 0.01f);
	fov_ *= Lamb::Math::toDegree<float>;
	ImGui::DragFloat("視野角", &fov_, 0.01f);
	fov_ = std::fmod(fov_, 360.0f);
	fov_ *= Lamb::Math::toRadian<float>;
	ImGui::DragFloat("当たり判定の距離", &collisionRange_, 0.01f);
	ImGui::TreePop();
}
#endif // USE_DEBUG_CODE

}

