#include "StringOut.h"
#include "Engine/EngineParts/DirectXCommon/DirectXCommon.h"
#include "Engine/EngineParts/StringOutPutManager/StringOutPutManager.h"
#include "Engine/Engine.h"
#include "../externals/imgui/imgui.h"
#include "Utils/ConvertString/ConvertString.h"


StringOut::StringOut():
	format_(),
	str_(),
	pos_(),
	rotation_(0.0f),
	scale_(Vector2::identity),
	color_(0xffffffff),
	isHorizontal_(false)
{
	str_.reserve(0x40);
}

StringOut::StringOut(const StringOut& right) {
	*this = right;
}

StringOut::StringOut(StringOut&& right) noexcept {
	*this = std::move(right);
}

StringOut::StringOut(const std::string& formatName) :
	format_(formatName),
	str_(),
	pos_(),
	rotation_(0.0f),
	scale_(Vector2::identity),
	color_(0xffffffff),
	isHorizontal_(false)
{
	str_.reserve(0x40);
}

StringOut::StringOut(const std::wstring& formatName) :
	format_(ConvertString(formatName)),
	str_(),
	pos_(),
	rotation_(0.0f),
	scale_(Vector2::identity),
	color_(0xffffffff),
	isHorizontal_(false)
{
	str_.reserve(0x40);
}

StringOut& StringOut::operator=(const StringOut& right) {
	format_ = right.format_;
	str_ = right.str_;
	pos_ = right.pos_;
	rotation_ = right.rotation_;
	scale_ = right.scale_;
	color_ = right.color_;
	isHorizontal_ = right.isHorizontal_;

	return *this;
}

StringOut& StringOut::operator=(StringOut && right) noexcept{
	format_ = std::move(right.format_);
	str_ = std::move(right.str_);
	pos_ = std::move(right.pos_);
	rotation_ = std::move(right.rotation_);
	scale_ = std::move(right.scale_);
	color_ = std::move(right.color_);
	isHorizontal_ = std::move(right.isHorizontal_);

	return *this;
}

StringOut& StringOut::operator=(const std::string& right) {
	str_ = ConvertString(right);

	return *this;
}

StringOut& StringOut::operator=(const std::wstring& right) {
	str_ = right;

	return *this;
}

StringOut& StringOut::operator+=(const std::string& right) {
	str_ += ConvertString(right);

	return *this;
}
StringOut& StringOut::operator+=(const std::wstring& right) {
	str_ += right;

	return *this;
}

StringOut& StringOut::operator<<(const std::string& right) {
	str_ += ConvertString(right);

	return *this;
}
StringOut& StringOut::operator<<(const std::wstring& right) {
	str_ += right;

	return *this;
}

const StringOut& StringOut::operator>>(std::wstring& right) const {
	right = str_;

	return *this;
}

const StringOut& StringOut::operator>>(std::string& right) const {
	right = ConvertString(str_);

	return *this;
}

void StringOut::Draw() {
	if (str_.empty()) {
		return;
	}

	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	auto  stringOutPutManager = StringOutPutManager::GetInstance();
	
	auto batch = stringOutPutManager->GetBatch(format_);

	batch->Begin(commandList);

	stringOutPutManager->GetFont(format_)->DrawString(
		stringOutPutManager->GetBatch(format_),
		str_.c_str(),
		DirectX::XMFLOAT2(pos_.x, pos_.y),
		UintToVector4(color_).m128,
		rotation_,
		DirectX::XMFLOAT2(0.0f, 0.0f),
		DirectX::XMFLOAT2(scale_.x, scale_.y)
	);
	batch->End();
}

void StringOut::Debug([[maybe_unused]]const std::string& debugName) {
#ifdef _DEBUG
	static Vector4 debugColor;
	debugColor = UintToVector4(color_);
	static std::string debugStr;
	debugStr.resize(32);
	debugStr = ConvertString(str_);

	ImGui::Begin(debugName.c_str());
	ImGui::InputText("text", debugStr.data(), debugStr.size());
	ImGui::DragFloat2("pos", &pos_.x);
	ImGui::DragFloat("rotation", &rotation_, 0.01f);
	ImGui::DragFloat2("scale", &scale_.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &debugColor.color.r);
	ImGui::Checkbox("isHorizontal", &isHorizontal_);
	ImGui::End();

	str_ = ConvertString(debugStr);

	color_ = Vector4ToUint(debugColor);
#endif // _DEBUG
}