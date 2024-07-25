#include "WindNodeComp.h"
#include "Drawers/Line/Line.h"

void WindNodeComp::Start() {
	isActive_ = true;
	ease_.Start(false, 1.0f);
}

void WindNodeComp::Move() {
	if (isActive_) {
		// 方向に速度を足して移動
		start += direction * speed * object_.GetDeltaTime();

		if (ease_.GetIsActive()) {
			// 長さをイージング
			end = start + direction * ease_.Get(0.0f, length);
			// イージング更新
			ease_.Update();
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

void WindNodeComp::Draw() {
	if (isActive_) {
		Line::Draw(
			start,
			end,
			object_.GetCameraMatrix(),
			std::numeric_limits<uint32_t>::max(),
			true
		);
	}
}

bool WindNodeComp::IsInRange(const Vector3& pos, const Vector3& minRange, const Vector3& maxRange)
{
	return (minRange.x <= pos.x && pos.x <= maxRange.x) &&
		(minRange.y <= pos.y && pos.y <= maxRange.y) &&
		(minRange.z <= pos.z && pos.z <= maxRange.z);
}
