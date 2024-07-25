#pragma once
#include "../Object.h"
#include "Drawers/Model/Model.h"


class SpriteRenderDataComp : public IComp {
public:
	enum class Offset{
		kMiddle,
		kLeftUpper,
		kRightUpper,
		kLeftUnder,
		kRightUnder,

		kNum
	};

#ifdef _DEBUG
private:
	static const std::array<std::string, uint32_t(Offset::kNum)> kComboEnum_;
#endif // _DEBUG

public:
	static const std::array<Mat4x4, uint32_t(Offset::kNum)> kOffsetMatrix;

public:
	using IComp::IComp;

	~SpriteRenderDataComp() = default;

	void Init() override;

#ifdef _DEBUG
	void FirstUpdate() override;
#endif // _DEBUG


	void Debug(const std::string& guiName) override;

public:
	BlendType type;
	Vector4 color;
	std::string fileName;
	QuaternionTransform uvTransform;
	uint32_t texHandle = 0;
	
	Offset offsetType = Offset::kMiddle;

private:
	Mat4x4 offsetTransform_ = Mat4x4::kIdentity;

private:
#ifdef _DEBUG
	Vector3 euler_;
	std::vector<std::filesystem::path> filePaths_;
#endif // _DEBUG

};