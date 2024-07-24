#include "ChildrenObjectComp.h"

const std::unordered_set<Lamb::SafePtr<Object>>& ChildrenObjectComp::GetObjects() const
{
	// TODO: return ステートメントをここに挿入します
}

void ChildrenObjectComp::Init() {

}

void ChildrenObjectComp::AddObject(Object* object) {
	// nullなら早期リターン
	if (object == nullptr) {
		return;
	}
	if (not objects_.contains(object)) {
		objects_.insert(object);
	}
}

void ChildrenObjectComp::EraseObject(Object* object)
{
	if (objects_.contains(object)) {
		objects_.erase(object);
	}
}

void ChildrenObjectComp::FirstUpdate() {
	for (auto& i : objects_) {
		i->FirstUpdate();
	}
}

void ChildrenObjectComp::Move() {
	for (auto& i : objects_) {
		i->Move();
	}
}

void ChildrenObjectComp::Event() {
	for (auto& i : objects_) {
		i->Event();
	}
}

void ChildrenObjectComp::Update() {
	for (auto& i : objects_) {
		i->Update();
	}
}

void ChildrenObjectComp::LastUpdate() {
	for (auto& i : objects_) {
		i->LastUpdate();
	}
}

void ChildrenObjectComp::Draw() {
	for (auto& i : objects_) {
		i->Draw();
	}
}

