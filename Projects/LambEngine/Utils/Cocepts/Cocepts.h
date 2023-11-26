#pragma once
#include <type_traits>

namespace Lamb {
	template<class T>
	concept IsNotPtr = !std::is_pointer_v<T>;

	template<class T>
	concept IsPtr = std::is_pointer_v<T>;
}