#include "Object.h"
#include "Utils/EngineInfo.h"
#include "../GameObject/Comp/Camera3DComp.h"

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

const Mat4x4& Object::GetCameraMatrix() const
{
	return cameraComp_.have() ? cameraComp_->GetMatrix() : camera_->GetViewProjection();
}
