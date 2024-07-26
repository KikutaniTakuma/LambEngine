#include "TransformComp.h"
#include "../Manager/TransformCompUpdater.h"
#ifdef _DEBUG
#include "CameraComp.h"
#include "imgui.h"
#endif // _DEBUG

#ifdef _DEBUG
const std::array<std::pair<std::string, ImGuizmo::OPERATION>, 5> TransformComp::kGuizmoMode_ = {
	std::make_pair<std::string, ImGuizmo::OPERATION>("TRANSLATE", ImGuizmo::TRANSLATE ),
	std::make_pair<std::string, ImGuizmo::OPERATION>("ROTATE", ImGuizmo::ROTATE ),
	std::make_pair<std::string, ImGuizmo::OPERATION>("SCALE", ImGuizmo::SCALE ),
	std::make_pair<std::string, ImGuizmo::OPERATION>("SCALEU", ImGuizmo::SCALEU ),
	std::make_pair<std::string, ImGuizmo::OPERATION>("UNIVERSAL", ImGuizmo::UNIVERSAL )
};
#endif // _DEBUG



TransformComp::TransformComp(Object* const object):
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

void TransformComp::UpdateMatrix()
{
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

void TransformComp::SetParent(Lamb::SafePtr<TransformComp>& parent)
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
#ifdef _DEBUG
	// コンボボックスを使ってenumの値を選択する
	if (ImGui::BeginCombo("BlendType", kGuizmoMode_[guimoType_].first.c_str()))
	{
		for (uint32_t count = 0; auto& i : kGuizmoMode_)
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

	ImGuizmo::SetID(guizmoID_);
	const auto& camera = *object_.GetCameraComp();
	if (ImGuizmo::Manipulate(camera.GetViewMatrix().data(), camera.GetToNdcMatrix().data(), kGuizmoMode_[guimoType_].second, ImGuizmo::WORLD, worldMatrix_.data())) {
		worldMatrix_.Decompose(scale, rotate, translate);
		rotate = rotate.Normalize();
		TransformCompUpdater::GetInstance()->SetCurretnGuizmoID(guizmoID_);
	}

	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::DragFloat3("scale", scale.data(), 0.01f);

		if (eulerRotate == Vector3::kZero) {
			eulerRotate = rotate.ToEuler();
		}
		eulerRotate *= Lamb::Math::toDegree<float32_t>;
		ImGui::DragFloat3("rotate(Degree)", eulerRotate.data(), 1.0f);
		eulerRotate *= Lamb::Math::toRadian<float32_t>;

		rotate = Quaternion::EulerToQuaternion(eulerRotate);
		rotate = rotate.Normalize();


		ImGui::DragFloat3("translate", translate.data(), 0.01f);
		for (size_t index = 0; auto & i : children_) {
			i->Debug("children_" + std::to_string(index));
			index++;
		}

		ImGui::TreePop();
	}
#endif // _DEBUG

}

void TransformComp::SetGuizmoID(uint32_t id) {
	guizmoID_ = id;
}
