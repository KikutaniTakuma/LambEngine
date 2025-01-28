#include "TransformComp.h"
#include "../Manager/TransformCompUpdater.h"
#ifdef USE_DEBUG_CODE
#include "CameraComp.h"
#include "imgui.h"
#endif // USE_DEBUG_CODE

#ifdef USE_DEBUG_CODE
const std::array<std::pair<std::string, ImGuizmo::OPERATION>, 5> TransformComp::kGuizmoMode_ = {
	std::make_pair<std::string, ImGuizmo::OPERATION>("TRANSLATE", ImGuizmo::TRANSLATE),
	std::make_pair<std::string, ImGuizmo::OPERATION>("ROTATE", ImGuizmo::ROTATE),
	std::make_pair<std::string, ImGuizmo::OPERATION>("SCALE", ImGuizmo::SCALE),
	std::make_pair<std::string, ImGuizmo::OPERATION>("SCALEU", ImGuizmo::SCALEU),
	std::make_pair<std::string, ImGuizmo::OPERATION>("UNIVERSAL", ImGuizmo::UNIVERSAL)
};
#endif // USE_DEBUG_CODE



TransformComp::TransformComp(Object* const object) :
	IComp(object),
	scale(Vector3::kIdentity),
	rotate(),
	translate(),
	worldMatrix_(Mat4x4::kIdentity),
	parent_(nullptr),
	children_()
{
	TransformCompUpdater::GetInstance()->Set(this);
}

TransformComp::~TransformComp()
{
	TransformCompUpdater::GetInstance()->Erase(this);
}

void TransformComp::Init()
{

}

void TransformComp::UpdateMatrix() {
#ifdef USE_DEBUG_CODE
	rotate.SetEuler(eulerRotate);
#endif // USE_DEBUG_CODE
	rotate = rotate.Normalize();
	worldMatrix_ = Mat4x4::MakeAffin(scale, rotate, translate);
}

void TransformComp::UpdateChildrenMatrix() {
	for (auto& i : children_) {
		i->UpdateParentMatrix();
		if (i->HaveChildren()) {
			i->UpdateChildrenMatrix();
		}
	}
}

void TransformComp::UpdateParentMatrix()
{
	if (parent_.have()) {
		worldMatrix_ *= parent_->worldMatrix_;
	}
}

void TransformComp::SetParent(Lamb::SafePtr<TransformComp> parent)
{
	// もしparent_がnullじゃないかつ、今もっているやつと違うものかつ、親のchildrenに自身を持っているか
	if (parent_.have() and parent_ != parent and parent_->children_.contains(this)) {
		parent_->children_.erase(this);
	}
	parent_ = parent;
	if (parent_.have()) {
		parent_->children_.insert(this);
	}
}

void TransformComp::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef USE_DEBUG_CODE
	if (ImGui::TreeNode((guiName + " Guizmo").c_str())) {
		// コンボボックスを使ってenumの値を選択する
		if (ImGui::BeginCombo("BlendType", kGuizmoMode_[guimoType_].first.c_str()))
		{
			for (uint32_t count = 0; auto & i : kGuizmoMode_)
			{
				bool isSelected = (guimoType_ == count);
				if (ImGui::Selectable(i.first.c_str(), isSelected))
				{
					guimoType_ = count;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
				count++;
			}
			ImGui::EndCombo();
		}
		isGuizmo_ = true;

		ImGui::TreePop();
	}
	else {
		isGuizmo_ = false;
	}
	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::DragFloat3("scale", scale.data(), 0.01f);

		eulerRotate = rotate.ToEuler();
		eulerRotate *= Lamb::Math::toDegree<float32_t>;
		ImGui::DragFloat3("rotate(Degree)", eulerRotate.data(), 1.0f);
		eulerRotate *= Lamb::Math::toRadian<float32_t>;
		rotate = Quaternion::EulerToQuaternion(eulerRotate);

		ImGui::DragFloat3("translate", translate.data(), 0.01f);
		for (size_t index = 0; auto & i : children_) {
			i->Debug("children_" + std::to_string(index));
			index++;
		}

		ImGui::TreePop();
	}
#endif // USE_DEBUG_CODE

}

#ifdef USE_DEBUG_CODE
void TransformComp::SetGuizmoID(uint32_t id) {
	guizmoID_ = id;
}

void TransformComp::Guizmo(CameraComp* cameraComp) {
	if (isGuizmo_) {
		ImGuizmo::SetID(guizmoID_);
		const Mat4x4& view = cameraComp->GetViewMatrix();
		const Mat4x4& ndc = cameraComp->GetToNdcMatrix();
		if (ImGuizmo::Manipulate(view.data(), ndc.data(), kGuizmoMode_[guimoType_].second, ImGuizmo::WORLD, worldMatrix_.data())) {
			if (parent_) {
#ifdef USE_DEBUG_CODE
				(worldMatrix_ * parent_->worldMatrix_.Inverse()).Decompose(scale, eulerRotate, translate);
#else
				(worldMatrix_ * parent_->worldMatrix_.Inverse()).Decompose(scale, rotate, translate);
#endif // USE_DEBUG_CODE
			}
			else {
#ifdef USE_DEBUG_CODE
				worldMatrix_.Decompose(scale, eulerRotate, translate);
#else
				worldMatrix_.Decompose(scale, rotate, translate);
#endif // USE_DEBUG_CODE

			}
			TransformCompUpdater::GetInstance()->SetCurretnGuizmoID(guizmoID_);
		}
	}
}
#endif // USE_DEBUG_CODE

void TransformComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["scale"] = nlohmann::json::array();
	for (auto& i : scale) {
		json["scale"].push_back(i);
	}
	json["rotate"] = nlohmann::json::array();
	for (auto& i : rotate) {
		json["rotate"].push_back(i);
	}
	json["translate"] = nlohmann::json::array();
	for (auto& i : translate) {
		json["translate"].push_back(i);
	}
}

void TransformComp::Load(nlohmann::json& json)
{
	for (size_t i = 0; i < json["scale"].size(); i++) {
		scale[i] = json["scale"][i];
	}
	for (size_t i = 0; i < json["rotate"].size(); i++) {
		rotate.m[i] = json["rotate"][i];
	}
	for (size_t i = 0; i < json["translate"].size(); i++) {
		translate[i] = json["translate"][i];
	}

#ifdef USE_DEBUG_CODE
	eulerRotate = rotate.ToEuler();
#endif // USE_DEBUG_CODE

}

/// 課題用
/// 3-8確認課題
/// スクリーンショット2枚目
