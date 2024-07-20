#include "Object.h"
#include "Utils/EngineInfo.h"
#include "../GameObject/Comp/Camera3DComp.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void Object::Init() {
	/*for (auto& i : components_) {
		i.second->Init();
	}*/
}

void Object::FirstUpdate()
{
	float32_t deltaTime = Lamb::DeltaTime();
	this->SetDeltaTime(deltaTime);
	for (auto& i : components_) {
		i.second->FirstUpdate();
	}
}

void Object::Move()
{
	for (auto& i : components_) {
		i.second->Move();
	}
}

void Object::Event() {
	for (auto& i : components_) {
		i.second->Event();
	}
}

void Object::Update() {
	for (auto& i : components_) {
		i.second->Update();
	}
}

void Object::LastUpdate()
{
	for (auto& i : components_) {
		i.second->LastUpdate();
	}
}

void Object::Draw() const
{
	for (auto& i : components_) {
		i.second->Draw();
	}
}

void Object::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::Text("tags : ");
		for (auto& i : tags_) {
			ImGui::SameLine();
			ImGui::Text("%s, ", i.c_str());
		}
		if (ImGui::TreeNode("componets")) {
			for (auto& i : components_) {
				i.second->Debug(i.first);
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
#endif // _DEBUG
}

const Mat4x4& Object::GetCameraMatrix() const
{
	return cameraComp_.have() ? cameraComp_->GetMatrix() : camera_->GetViewProjection();
}
