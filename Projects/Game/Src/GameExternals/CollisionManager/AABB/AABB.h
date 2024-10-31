#pragma once

#include "Math/Vector3.h"

struct AABB {
	Vector3 center; //!< 真ん中
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
};