#pragma once
#include <type_traits>

namespace Lamb {
	template<class T>
	concept IsPtr = std::is_pointer_v<T>;

	template<class T>
	concept IsNotPtr = !std::is_pointer_v<T>;

	template<class T>
	concept IsContainer = requires(T a) {
		{ a.size() } -> std::convertible_to<std::size_t>;
		{ a.begin() } -> std::input_or_output_iterator;
		{ a.end() } -> std::input_or_output_iterator;
	};

	template<class T>
	concept IsReference = std::is_reference_v<T>;

	template<class T>
	concept IsNotReference = !std::is_reference_v<T>;

	template<class T>
	concept IsNotReferenceAndPtr = requires { IsNotPtr<T> && IsNotReference<T>; };

	template<typename T>
	concept IsNumber = requires{ std::is_integral_v<T>&& std::is_floating_point_v<T>; };

	template<class T>
	concept IsNotNumber = !IsNumber<T>;

	template<class T>
	concept IsInt = std::is_integral_v<T>;

	template<class T>
	concept IsNotInt = !std::is_integral_v<T>;

	template<class T>
	concept IsFloat = std::is_floating_point_v<T>;

	template<class T>
	concept IsNotFloat = !std::is_floating_point_v<T>;

	template<class T>
	concept IsIntSmallerThan32bit = requires {
		IsInt<T> && sizeof(T) <= 32llu;
	};
}