#pragma once
#include <numbers>
#include <limits>
#include <initializer_list>

namespace Lamb {
	namespace Math {
		template<std::floating_point Floating>
		constexpr Floating toRadian = static_cast<Floating>(std::numbers::pi_v<double> / 180.0);

		template<std::floating_point Floating>
		constexpr Floating toDegree = static_cast <Floating>(180.0 / std::numbers::pi_v<double>);

		float LengthSQ(const std::initializer_list<float>& data);
		float Length(const std::initializer_list<float>& data);
	}
}

inline uint16_t operator""_u16(size_t i) {
	return static_cast<uint16_t>(i);
}

inline uint32_t operator""_u32(size_t i) {
	return static_cast<uint32_t>(i);
}

inline uint64_t operator""_u64(size_t i) {
	return i;
}

inline int16_t operator""_16(size_t i) {
	return static_cast<uint16_t>(i);
}

inline int32_t operator""_32(size_t i) {
	return static_cast<uint32_t>(i);
}

inline int64_t operator""_64(size_t i) {
	return i;
}

inline float operator""_32f(long double i) {
	return static_cast<float>(i);
}

inline double operator""_64f(long double i) {
	return static_cast<double>(i);
}
