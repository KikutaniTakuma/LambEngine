#include "WindParticle.h"
#include "Utils/Random.h"
#include "Camera/Camera.h"

void WindParticle::Initialize(
	const Vector3& pos,
	const Vector3& size,
	const Vector3& direction
)
{
	pos_ = pos;
	size_ = size;
	direction_ = direction.Normalize();

	lengthRange_.first = 1.0f;
	lengthRange_.second = size_.Length() * 0.5f;

	speedRange_.first = 3.0f;
	speedRange_.second = 6.0f;

	appearNumberRange_.first = 2;
	appearNumberRange_.second = 3;

	appearFrequencyRange_.first = 0;
	appearFrequencyRange_.second = 99;
	appearNum_ = 2;

	positionRange_.first = pos_ + -size_;
	positionRange_.second = pos_ + size_;

	if (direction_.z == 1.0f) {
		positionRange_.second.z = positionRange_.first.z;
	}
	else if (direction_.z == -1.0f) {
		positionRange_.first.z = positionRange_.second.z;
	}
	else if(direction_.x == 1.0f){
		positionRange_.second.x = positionRange_.first.x;
	}
	else if (direction_.x == -1.0f) {
		positionRange_.first.x = positionRange_.second.x;
	}

	particle_.clear();
}

void WindParticle::Update()
{
	// 生成するか
	if (Lamb::Random(appearFrequencyRange_.first, appearFrequencyRange_.second) < appearNum_) {
		CreatParticle();
	}

	// パーティクル更新
	Vector3 maxRange = positionRange_.second;
	maxRange.z = (pos_ + size_).z;
	for (auto& i : particle_) {
		i->Update(direction_, pos_ + -size_, pos_ + size_);
	}

	// パーティクル削除
	DeleteParticle();
}

void WindParticle::Draw(const Camera& camera)
{
	for (auto& i : particle_) {
		i->Draw(camera);
	}
}

void WindParticle::DeleteParticle()
{
	for (auto& i : particle_) {
		if (!i->GetIsActive()) {
			i.reset();
		}
	}

	std::erase_if(particle_, [](const std::unique_ptr<WindNode>& i)->bool {
		return !static_cast<bool>(i);
		}
	);
}

void WindParticle::CreatParticle()
{
	//生成する数
	uint32_t appearNum = Lamb::Random(appearNumberRange_.first, appearNumberRange_.second);

	/*Mat4x4 appearPosRotate = Mat4x4::DirectionToDirection(
		(positionRange_.first - pos_).Normalize(),
		-direction_
	);*/

	for (uint32_t i = 0; i < appearNum; i++) {
		Vector3 appearPos = Lamb::Random(positionRange_.first, positionRange_.second);
		//appearPos *= appearPosRotate;

		auto particle = std::make_unique<WindNode>();
		particle->Initialize(
			appearPos,
			Lamb::Random(speedRange_.first, speedRange_.second),
			Lamb::Random(lengthRange_.first, lengthRange_.second)
		);
		particle_.push_back(std::move(particle));
	}
}
