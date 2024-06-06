#include "Object.h"
#include "Utils/EngineInfo.h"

void Object::Init() {
	/*for (auto& i : components_) {
		i.second->Init();
	}*/
}

void Object::Update() {
	float32_t deltaTime = Lamb::DeltaTime();
	for (auto& i : components_) {
		i.second->SetDeltaTime(deltaTime);
	}
	for (auto& i : components_) {
		i.second->Update0();
	}
	for (auto& i : components_) {
		i.second->Update1();
	}
	for (auto& i : components_) {
		i.second->Update2();
	}
	for (auto& i : components_) {
		i.second->Update3();
	}
	for (auto& i : components_) {
		i.second->Update4();
	}
}

void Object::Draw() const
{
	for (auto& i : components_) {
		i.second->Draw();
	}
}
