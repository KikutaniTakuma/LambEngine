#pragma once
#include <string>
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"
#include "Utils/ConvertString/ConvertString.h"
#include <concepts>

/// <summary>
/// 文字列描画(座標系はスクリーンなので注意)
/// </summary>
class StringOut {
public:
	StringOut();
	StringOut(const StringOut& right);
	StringOut(StringOut&& right) noexcept;
	StringOut(const std::string& formatName);
	StringOut(const std::wstring& formatName);
	~StringOut() = default;

public:
	StringOut& operator=(const StringOut& right);
	StringOut& operator=(StringOut&& right) noexcept;
	StringOut& operator=(const std::string& right);
	StringOut& operator=(const std::wstring& right);

	StringOut& operator+=(const std::string& right);
	StringOut& operator+=(const std::wstring& right);

	StringOut& operator<<(const std::string& right);
	StringOut& operator<<(const std::wstring& right);

	template<std::integral T>
	StringOut& operator<<(const T& right) {
		str_ += std::to_wstring(right);

		return *this;
	}

	template<std::floating_point T>
	StringOut& operator<<(const T& right) {
		str_ += std::to_wstring(right);

		return *this;
	}

	const StringOut& operator>>(std::string& right) const;
	const StringOut& operator>>(std::wstring& right) const;

public:
	void Draw();

	void Debug(const std::string& debugName);


private:
	std::string format_;
	std::wstring str_;

public:
	Vector2 pos_;
	float rotation_;
	Vector2 scale_;
	uint32_t color_;
	bool isHorizontal_;

public:
	inline void SetFormat(const std::string& formatName) {
		format_ = formatName;
	}
	inline void SetFormat(const std::wstring& formatName) {
		format_ = ConvertString(formatName);
	}
};