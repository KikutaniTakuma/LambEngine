#include "PlayerComp.h"
#include "../Comp/TransformComp.h"
#include "../Comp/ChildrenObjectComp.h"
#include "../Comp/CsvDataComp.h"

void PlayerComp::Init()
{
    item_ = object_.AddComp<ChildrenObjectComp>();
    itemData_ = object_.AddComp<CsvDataComp>();
    transformComp_ = object_.AddComp<TransformComp>();


    floatParameter_[FloatParameter::kAcceleration] = 3.5;
    floatParameter_[FloatParameter::kAddAngle] = 95.0f;
    floatParameter_[FloatParameter::kCannonPower] = 10.0f;
    floatParameter_[FloatParameter::kCannonSubtractPower] = 5.0f;
    floatParameter_[FloatParameter::kCustomizeAddPower] = 0.0f;
    floatParameter_[FloatParameter::kDeceleration] = 1.0f;
    floatParameter_[FloatParameter::kMaxCannonVelocity] = 10.0f;
    floatParameter_[FloatParameter::kMaxPower] = 100.0f;
    floatParameter_[FloatParameter::kMaxSpeed] = 15.0f;
    floatParameter_[FloatParameter::kMaxWindVelocity] = 20.0f;
    floatParameter_[FloatParameter::kSubtractAngle] = 120.0f;
    floatParameter_[FloatParameter::kSubtractCannonVelocity] = 5.0f;
    floatParameter_[FloatParameter::kSubtractPower] = 4.0f;
    floatParameter_[FloatParameter::kSubtractWindVelocity] = 8.0f;
    floatParameter_[FloatParameter::kTurnSpeed] = 0.23999999463558197f;
    // これは使わないんか？
    //floatParameter_[FloatParameter::kWindDeceleration] = 0.009999999776482582f;
    floatParameter_[FloatParameter::kWindPower] = 40.0f;
}

void PlayerComp::Finalize()
{
}

void PlayerComp::FirstUpdate()
{
}

void PlayerComp::Move()
{
    float32_t deltaTime = object_.GetDeltaTime();

    // パワーがあったら
    if (power_ > 0.0f) {
        float currentMaxSpeed = (floatParameter_.at(kMaxSpeed) - weight_);
        if (velocity_ < currentMaxSpeed) {
            velocity_ += floatParameter_.at(kAcceleration) * deltaTime;
        }
        power_ -= floatParameter_.at(kSubtractPower) * deltaTime;
        power_ = std::clamp(power_, 0.0f, floatParameter_.at(kMaxPower));
    }
    // パワーがなくなったら減速
    if (power_ <= 0.0f) {
        float currentMaxDeceleration = (floatParameter_.at(kDeceleration) + weight_);
        if (velocity_ > 0.0f) {
            velocity_ -= currentMaxDeceleration * deltaTime;
        }
        else {
            velocity_ = 0.0f;
        }
    }

    // 速度をクランプ（最大速度を超えたとき）
    velocity_ = std::clamp(velocity_, 0.0f, floatParameter_.at(kMaxSpeed));

    // 進む方向
    Vector3 vector{};
    vector = { std::cos(currentAngle_ * Lamb::Math::toRadian<float32_t>),0.0f,std::sin(currentAngle_ * Lamb::Math::toRadian<float32_t>) };
    // 自機の回転	
    UpdateRotate(vector);
    prePlayerPos_ = transformComp_->translate;
    transformComp_->translate += ((direction_ * velocity_) + (windVector_ * windVelocity_) + (cannonVector_ * cannonVelocity_)) * deltaTime;
}

void PlayerComp::Event()
{
}

void PlayerComp::Update()
{
}

void PlayerComp::LastUpdate()
{
}

void PlayerComp::Draw([[maybe_unused]]CameraComp* cameraComp) {

}

void PlayerComp::Save(nlohmann::json& json)
{
	SetCompName<PlayerComp>(json);
    json["param"] = nlohmann::json::array();
    for (auto& i : floatParameter_) {
        json["param"].push_back(i);
    }
}

void PlayerComp::Load(nlohmann::json& json) {
    for (size_t i = 0; i < json["param"].size(); i++) {
        floatParameter_[i] = json["param"][i];
    }
}

void PlayerComp::UpdateRotate(const Vector3& direction)
{
    float angle = 0.0f;
    Vector3 from = { 0.0f,0.0f,1.0f };
    Vector3 to = direction_;
    float dot = from.Dot(to);
    Vector2 Vector2From = { from.x ,from.z };
    Vector2 Vector2To = { to.x ,to.z };
    if (dot >= 1.0f) {
        angle = 0.0f;
    }
    else if (dot <= -1.0f) {
        angle = 180.0f * std::numbers::pi_v<float> / 180.0f;
    }
    else if (Vector2From.Cross(Vector2To) > 0) {
        angle = -std::acosf(dot);
    }
    else {
        angle = std::acosf(dot);
    }
    angle = Lamb::Math::LerpShortAngle(transformComp_->rotate.ToEuler().y, angle, floatParameter_.at(FloatParameter::kTurnSpeed));
    transformComp_->rotate = Quaternion::MakeRotateYAxis(angle);
    direction_ = { std::sin(angle),0.0f,std::cos(angle) };
    direction_.Normalize();
}
