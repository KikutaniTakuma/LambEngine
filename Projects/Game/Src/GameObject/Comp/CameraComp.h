#include "../Object.h"

#include "TransformComp.h"

class CameraComp : public IComp {
public:
	using IComp::IComp;

	virtual ~CameraComp() = default;

public:
	void Init() override;

	const Mat4x4& GetCameraMatrix() const {
		return cameraMatrix_;
	}
	const Mat4x4& GetViewMatrix() const;
	virtual const Mat4x4& GetToNdcMatrix() const = 0;

	TransformComp& GetTransformComp();
	const TransformComp& GetTransformComp() const;

protected:
	Lamb::SafePtr<TransformComp> transform_;
	Mat4x4 cameraMatrix_ = Mat4x4::kIdentity;
	Mat4x4 viewMatrix_ = Mat4x4::kIdentity;
};
