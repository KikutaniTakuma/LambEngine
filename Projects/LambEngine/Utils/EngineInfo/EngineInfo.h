#pragma once
#include <chrono>
#include "Utils/Math/Vector2.h"

namespace Lamb {
	float DeltaTime();

	Vector2 ClientSize();

	Vector2 WindowSize();

	std::chrono::steady_clock::time_point ThisFrameTime();

	bool IsEngineFianlize();
}