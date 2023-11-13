#pragma once
#include <string>
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"
#include "Utils/ConvertString/ConvertString.h"

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

public:
	void Draw();

#ifdef _DEBUG
	void Debug(const std::string& debugName);
#endif // _DEBUG


private:
	std::string format_;
public:
	std::wstring str_;

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