#include "ChildrenObjectComp.h"



void ChildrenObjectComp::Init() {

}

void ChildrenObjectComp::Finalize() {
	objects_.clear();
}

void ChildrenObjectComp::AddObject(std::unique_ptr<Object>&& object) {
	// nullなら早期リターン
	if (not object) {
		return;
	}
	if (not objects_.contains(object)) {
		objects_.insert(object);
	}
}

void ChildrenObjectComp::EraseObject(Object* object) {
	auto itr = std::find_if(objects_.begin(), objects_.end(), 
		[&object](const std::unique_ptr<Object>& element)->bool {
			return object == element.get();
		}
	);
	if (itr != objects_.end()) {
		objects_.erase(itr);
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

const std::unordered_set<std::unique_ptr<Object>>& ChildrenObjectComp::GetObjects() const
{
	return objects_;
}

