#include "LineCollisionComp.h"
#include "LineRenderDataComp.h"
#include "../Manager/CompCollisionManager.h"

#include <format>

void LineCollisionComp::Init()
{
	CompCollisionManager::GetInstance()->Set(this);
	lineComp_ = object_.AddComp<LineComp>();
	transFormComp_ = object_.AddComp<TransformComp>();
}

void LineCollisionComp::Finalize() {
	CompCollisionManager::GetInstance()->Erase(this);
}

void LineCollisionComp::FirstUpdate() {
	isCollision_ = false;
	mostNearCollisionObjectPtr_ = nullptr;
}

bool LineCollisionComp::Collision(ObbComp* obbComp)
{
	// 毎フレームリセットする
	if (not isCollision_.OnEnter()) {
#ifdef USE_IMGUI
		color_ = 0xffffffff;
#endif // USE_IMGUI
	}

	// 当たり判定
	bool isCollision = obbComp->IsCollision(lineComp_->start, lineComp_->end);

	if (isCollision) {
		// 当たり判定フラグ追加
		isCollision_ = true;
		
		// もっとも近いオブジェクトのポインタを保存
		// emptyだった場合はそのまま保存
		if (mostNearCollisionObjectPtr_.empty()) {
			mostNearCollisionObjectPtr_ = &(obbComp->getObject());
		}
		else {
			float32_t preMostNearObjectLength = (mostNearCollisionObjectPtr_->GetComp<TransformComp>()->translate - lineComp_->start).Length();
			float32_t currentMostNearObjectLength = (obbComp->GetTransformComp().translate - lineComp_->start).Length();
			// いま持っているオブジェクトより近かったらポインタを代入
			if (currentMostNearObjectLength < preMostNearObjectLength) {
				mostNearCollisionObjectPtr_ = &(obbComp->getObject());
			}
		}

		// 当たってたら色変更
#ifdef USE_IMGUI
		color_ = 0xff0000ff;
#endif // USE_IMGUI
	}

	return isCollision;
}

void LineCollisionComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["CollisionTags"] = nlohmann::json::array();
	for (auto& i : collisionTags_) {
		json["CollisionTags"].push_back(i);
	}
}

void LineCollisionComp::Load([[maybe_unused]]nlohmann::json& json) {
	for (auto& i : json["CollisionTags"]) {
		collisionTags_.insert(i);
	}
}

void LineCollisionComp::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef USE_IMGUI
	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::Text(std::format("isCollision : {}", !!isCollision_).c_str());

		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
		for (auto& i : collisionTags_) {
			if (ImGui::Button("erase")) {
				collisionTags_.erase(i);
				break;
			}
			ImGui::SameLine();
			ImGui::Text("tag : % s", i.c_str());
		}
		ImGui::EndChild();

		inputTag_.resize(32);
		ImGui::Text("current collision tag : %s", currentCollisionTag_.c_str());
		ImGui::InputText(
			"タグ",
			inputTag_.data(),
			inputTag_.size()
		);
		if (ImGui::Button("タグ追加")) {
			std::string addtag;
			for (auto& i : inputTag_) {
				if (i == '\0') {
					break;
				}
				addtag.push_back(i);
			}
			collisionTags_.insert(addtag);
		}
		ImGui::TreePop();
}
#endif // USE_IMGUI

}

const LineComp& LineCollisionComp::GetLineComp() const
{
	return *lineComp_;
}

const Lamb::Flg& LineCollisionComp::GetIsCollision() const
{
	return isCollision_;
}

const Lamb::SafePtr<const Object>& LineCollisionComp::GetMostNearCollisionObjectPtr() const
{
	return mostNearCollisionObjectPtr_;
}
