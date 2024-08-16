#pragma once

// 1メートル基準

inline float operator""_mm(long double i) {
	return static_cast<float>(i) * 0.001f;
}

inline float operator""_cm(long double i) {
	return static_cast<float>(i) * 0.01f;
}
inline float operator""_dm(long double i) {
	return static_cast<float>(i) * 0.1f;
}

inline float operator""_m(long double i) {
	return static_cast<float>(i) * 1.0f;
}

inline float operator""_dam(long double i) {
	return static_cast<float>(i) * 10.0f;
}
inline float operator""_hm(long double i) {
	return static_cast<float>(i) * 100.0f;
}
inline float operator""_km(long double i) {
	return static_cast<float>(i) * 1000.0f;
}