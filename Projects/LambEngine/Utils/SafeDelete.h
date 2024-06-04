#pragma once
#include "Utils/Cocepts.h"

namespace Lamb {
	template<IsPtr T>
	void SafeDelete(T& ptr) {
		delete ptr;
		ptr = nullptr;
	}
}