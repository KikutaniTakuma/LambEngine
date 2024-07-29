#include "WindNodeComp.h"
#include "Drawers/Line/Line.h"
#include "../Comp/CameraComp.h"

void WindNodeComp::Start() {
	isActive_ = true;
	ease_->Start(false, 1.0f);
}

void WindNodeComp::Init() {
	ease_ = std::make_unique<Easeing>();
}

void WindNodeComp::Move() {
	if (isActive_) {
		// 方向に速度を足して移動
		start += direction * speed * object_.GetDeltaTime();

		if (ease_->GetIsActive()) {
			// 長さをイージング
			end = start + direction * ease_->Get(0.0f, length);
			// イージング更新
			ease_->Update();
		}
		else {
			// lineの終わりをディレクションを元に計算
			if (IsInRange(end, minRange, maxRange)) {
				end = start + direction * length;
			}
		}


		// もし範囲外なら消す
		if (not IsInRange(start, minRange, maxRange)) {
			isActive_ = false;
		}
	}
}

void WindNodeComp::Draw(CameraComp* cameraComp) {
	if (isActive_) {
		Line::Draw(
			start,
			end,
			cameraComp->GetCameraMatrix(),
			std::numeric_limits<uint32_t>::max(),
			false
		);
	}
}

void WindNodeComp::Save(nlohmann::json& json)
{
	SetCompName<WindNodeComp>(json);
}

void WindNodeComp::Load([[maybe_unused]]nlohmann::json& json)
{
}

bool WindNodeComp::IsInRange(const Vector3& pos, const Vector3& min, const Vector3& max)
{
	return (min.x <= pos.x && pos.x <= max.x) &&
		(min.y <= pos.y && pos.y <= max.y) &&
		(min.z <= pos.z && pos.z <= max.z);
}
