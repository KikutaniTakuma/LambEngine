#include "WindNode.h"
#include "Drawers/Line/Line.h"
#include "Utils/Camera/Camera.h"
#include "Utils/EngineInfo/EngineInfo.h"

#include <numbers>

void WindNode::Initialize(const Vector3& start, float speed, float length)
{
	start_ = start;
	end_ = start_;
	speed_ = speed;
	length_ = length;

	isActive_ = true;

	ease_.Start(false, 1.0f);
}

void WindNode::Update(
	const Vector3& direction,
	const Vector3& minRange,
	const Vector3& maxRange
){
	if (isActive_) {
		// 方向に速度を足して移動
		start_ += direction * speed_ * Lamb::DeltaTime();

		if(ease_.GetIsActive()){
			// 長さをイージング
			end_ = start_ + direction * ease_.Get(0.0f, length_);
			// イージング更新
			ease_.Update();
		}
		else {
			// lineの終わりをディレクションを元に計算
			if (IsInRange(end_, minRange, maxRange)) {
				end_ = start_ + direction * length_;
			}
		}


		// もし範囲外なら消す
		if (!IsInRange(start_, minRange, maxRange)) {
			isActive_ = false;
		}
	}
}

void WindNode::Draw(const Camera& camera)
{
	if (isActive_) {
		Line::Draw(
			start_, 
			end_, 
			camera.GetViewProjection(), 
			std::numeric_limits<uint32_t>::max()
		);
	}
}

bool WindNode::IsInRange(
	const Vector3& pos,
	const Vector3& minRange, 
	const Vector3& maxRange
){
	return (minRange.x <= pos.x && pos.x <= maxRange.x) &&
		(minRange.y <= pos.y && pos.y <= maxRange.y) && 
		(minRange.z <= pos.z && pos.z <= maxRange.z);
}
