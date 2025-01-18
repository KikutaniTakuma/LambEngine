#pragma once
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Utils/SafePtr.h"
#include "Transform/Transform.h"

#include <array>


// オブジェクトに水からの反応を作る
class WaterInteractive {
private:
	// オブジェクトの大きさに合わせて変化
	struct Quad {
		Quad() : 
			positions{
				Vector3(-0.5f, 0.0f,  0.5f),
				Vector3( 0.5f, 0.0f,  0.5f),
				Vector3( 0.5f, 0.0f, -0.5f),
				Vector3(-0.5f, 0.0f, -0.5f)
			}
		{}

		static Quad CalcWorldPositions(const Mat4x4& worldMatrix) {
			Quad resullt;

			for (auto& p : resullt.positions) {
				p *= worldMatrix;
			}

			return resullt;
		}

		std::array<Vector3, 4> positions;
	};

public:
	WaterInteractive();
	WaterInteractive(const WaterInteractive&) = default;
	WaterInteractive(WaterInteractive&&) = default;
	~WaterInteractive() = default;

public:
	void Update();

	void SetMatrix(const Mat4x4& worldMatrix);

	void Debug(const Mat4x4& camera, const std::string& guiName);

	float GetHeight() const;

	const Quaternion& GetQuaternion() const;
private:
	Lamb::SafePtr<class Water> pWater_;

	Mat4x4 worldMatrix_;

	Quad quad_;
	
	Quaternion dir_;
	Quaternion pre_;
	Quaternion next_;

	float height_ = 0.0f;
	float preHeight_ = 0.0f;
	float nextHeight_ = 0.0f;

	float nextTime_ = 0.0f;
	float count_ = 0.0f;

#ifdef USE_DEBUG_CODE
	Vector3 debugNormal_;
#endif // USE_DEBUG_CODE

};