#include "../Object.h"

#include "TransformComp.h"

class CameraComp : public IComp {
public:
	using IComp::IComp;

	virtual ~CameraComp() = default;

public:
	void Init() override;

	void LastUpdate() override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

	const Mat4x4& GetCameraMatrix() const {
		return cameraMatrix_;
	}
	const Mat4x4& GetViewMatrix() const;
	const Mat4x4& GetToNdcMatrix() const;

	TransformComp& GetTransformComp();
	const TransformComp& GetTransformComp() const;

	void SetNdcMatix(const Mat4x4& ndc);

	void Debug(const std::string& guiName);

private:
	Lamb::SafePtr<TransformComp> transform_;
	Mat4x4 cameraMatrix_ = Mat4x4::kIdentity;
	Mat4x4 viewMatrix_ = Mat4x4::kIdentity;
	Mat4x4 ndcMatrix_ = Mat4x4::kIdentity;


#ifdef _DEBUG
	float32_t scaleSpeed_ = 0.001f;
	float32_t rotateSpeed_ = 0.3f;
	float32_t translateSpeed_ = 1.5f;

	bool isRotateInverse_ = true;
	bool isTranslateInverse_ = false;

	bool isDebug_ = true;

	Vector3 eulerRotate_;

#endif // _DEBUG
};
