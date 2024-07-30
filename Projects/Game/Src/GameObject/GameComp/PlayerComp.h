#pragma once
#include "../Object.h"
class PlayerComp : public IComp {
public:
	enum FloatParameter {
		kTurnSpeed,
		kAddAngle,
		kSubtractAngle,
		kAcceleration,
		kDeceleration,
		kWindPower,
		kCannonPower,
		kMaxSpeed,
		kMaxPower,
		kSubtractPower,
		kCannonSubtractPower,
		kMaxCannonVelocity,
		kSubtractCannonVelocity,
		kMaxWindVelocity,
		kSubtractWindVelocity,
		kCustomizeAddPower,
		kFloatParameterCount,
	};

public:
	using IComp::IComp;

	~PlayerComp() = default;

public:
	void Init() override;
	void Finalize() override;


	void FirstUpdate() override;
	void Move() override;
	void Event() override;
	void Update() override;
	void LastUpdate() override;

	void Draw(CameraComp* cameraComp) override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

private:
	void UpdateRotate(const Vector3& direction);

private:
	Lamb::SafePtr<class ChildrenObjectComp> item_;
	Lamb::SafePtr<class CsvDataComp> itemData_;

	Lamb::SafePtr<class TransformComp> transformComp_;

	std::array<float, FloatParameter::kFloatParameterCount> floatParameter_;

	float32_t power_ = 0.0f;
	float32_t velocity_ = 0.0f;
	float32_t weight_ = 0.0f;
	float32_t currentAngle_ = 0.0f;


	Vector3 prePlayerPos_;
	Vector3 direction_;

	Vector3 windVector_;
	float32_t windVelocity_;

	Vector3 cannonVector_;
	float32_t cannonVelocity_;
};