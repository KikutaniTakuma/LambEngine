#include "TransformComp.h"
#include "../Manager/TransformCompUpdater.h"
#ifdef _DEBUG
#include "imgui.h"
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
#ifdef _DEBUG
	if (eulerRotate == Vector3::kZero) {
		eulerRotate = rotate.ToEuler();
	}
	rotate = Quaternion::EulerToQuaternion(eulerRotate);
#endif // _DEBUG

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

void TransformComp::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	if(ImGui::TreeNode(guiName.c_str())) {
		ImGui::DragFloat3("scale", scale.data(), 0.01f);
		eulerRotate *= Lamb::Math::toDegree<float32_t>;
		ImGui::DragFloat3("rotate(Degree)", eulerRotate.data(), 0.01f);
		eulerRotate *= Lamb::Math::toRadian<float32_t>;
		ImGui::DragFloat3("translate", translate.data(), 0.01f);
		rotate = rotate.Normalize();
		for (size_t index = 0; auto & i : children_) {
			i->Debug("children_" + std::to_string(index));
			index++;
		}

		ImGui::TreePop();
	}
#endif // _DEBUG

}
