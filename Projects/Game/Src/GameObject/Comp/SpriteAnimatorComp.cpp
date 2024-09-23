#include "SpriteAnimatorComp.h"
#include "SpriteRenderDataComp.h"

void SpriteAnimatorComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
	json["startPos"] = nlohmann::json::array();
	for (auto& i : startPos_) {
		json["startPos"].push_back(i);
	}
	json["isLoop"] = isLoop_;
	json["animationNumber"] = animationNumber_;
	json["duration"] = duration_;
}

void SpriteAnimatorComp::Load(nlohmann::json& json) {
	for (size_t i = 0; i < startPos_.size(); i++) {
		startPos_[i] = json["startPos"][i];
	}
	isLoop_ = json["isLoop"].get<bool>();
	animationNumber_ = json["animationNumber"].get<int32_t>();
	duration_ = json["duration"].get<float32_t>();

	SetAnimationNumber(animationNumber_);
}

void SpriteAnimatorComp::Init()
{
	spriteRenderDataComp_ = object_.AddComp<SpriteRenderDataComp>();

	Start();
}

void SpriteAnimatorComp::Update() {
	if ((duration_ * static_cast<float>(currentAnimationNumber_) < animationTime_)) {
		currentAnimationNumber_++;
		currentPos_.x += std::abs(scale_.x);
	}

	if (isActive_) {
		animationTime_ += object_.GetDeltaTime();
	}

	if (animationNumber_ <= currentAnimationNumber_) {
		if (isLoop_) {
			Reset();
		}
		else {
			isActive_ = false;
		}
	}

	animationMatrix_ = Mat4x4::MakeAffin(scale_, Vector3::kZero, currentPos_);
}

void SpriteAnimatorComp::LastUpdate() {
	auto& transform = spriteRenderDataComp_->uvTransform;
	animationMatrix_.Decompose(transform.scale, transform.rotate, transform.translate);
}

void SpriteAnimatorComp::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::DragFloat2("startPos", startPos_.data(), 0.001f);
		ImGui::Checkbox("isLoop", &isLoop_);
		ImGui::DragInt("アニメーション枚数", &animationNumber_, 0.01f, 1, 100);
		ImGui::DragFloat("アニメーション間隔", &duration_, 0.01f, 0.0001f, 100.0f);
		ImGui::Checkbox("向き反転", &isDirectionInverse_);

		if (ImGui::Button("start")) {
			Start();
		}
		if (ImGui::Button("stop")) {
			Stop();
		}
		if (ImGui::Button("Restart")) {
			Restart();
		}
		if (ImGui::Button("Pause")) {
			Pause();
		}

		SetAnimationNumber(animationNumber_);
		DirectionInverse(isDirectionInverse_);
		ImGui::TreePop();
	}
#endif // _DEBUG
}

void SpriteAnimatorComp::Start() {
	Reset();
	Restart();
}

void SpriteAnimatorComp::Stop() {
	Pause();
	Reset();
}

void SpriteAnimatorComp::Reset() {
	currentAnimationNumber_ = 0;
	currentPos_ = startPos_;
	animationTime_ = 0.0f;
}

void SpriteAnimatorComp::Restart() {
	isActive_ = true;
}

void SpriteAnimatorComp::Pause() {
	isActive_ = false;
}

void SpriteAnimatorComp::SetAnimationNumber(uint32_t animationNumber) {
	animationNumber_ = std::max(1u, animationNumber);
	scale_.x = 1.0f / static_cast<float>(animationNumber_);
}

void SpriteAnimatorComp::DirectionInverse(bool isSigned) {
	if (isSigned) {
		scale_.x = -1.0f * std::abs(scale_.x);
	}
	else {
		scale_.x = std::abs(scale_.x);
	}
}
