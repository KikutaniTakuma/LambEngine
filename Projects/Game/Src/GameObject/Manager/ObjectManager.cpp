#include "ObjectManager.h"
#include "Utils/EngineInfo.h"

std::unique_ptr<ObjectManager> ObjectManager::instance_;

ObjectManager::~ObjectManager()
{
	instance_.reset();
}

ObjectManager* const ObjectManager::GetInstance()
{
	return instance_.get();
}

void ObjectManager::Initialize() {
	if (instance_) {
		instance_.reset();
	}

	instance_.reset(new ObjectManager());
}

void ObjectManager::Set(const Lamb::SafePtr<Object>& object) {
	if (not objects_.contains(object) and object.have()) {
		objects_.insert(object);
	}
}

void ObjectManager::Erase(const Lamb::SafePtr<Object>& object) {
	if (objects_.contains(object)) {
		objects_.erase(object);
	}
}

void ObjectManager::SetCamera(const Lamb::SafePtr<Camera>& camera) {
	for (auto& i : objects_) {
		i->SetCamera(camera.get());
	}
}

void ObjectManager::Update() {
	// すべてに関数呼び出しするのはなんか不健全なのでバッファする
	float32_t deltaTime = Lamb::DeltaTime();

	// デルタタイムセット
	for (auto& i : objects_) {
		i->SetDeltaTime(deltaTime);
	}

	// 最初の処理
	for (auto& i : objects_) {
		i->FirstUpdate();
	}

	// 移動
	for (auto& i : objects_) {
		i->Move();
	}

	// ここに当たり判定が入る



	// 移動後、当たり判定からの何かしらの処理
	for (auto& i : objects_) {
		i->Event();
	}

	// 通常の更新処理
	for (auto& i : objects_) {
		i->Update();
	}

	// 最後の更新処理
	for (auto& i : objects_) {
		i->LastUpdate();
	}
}

void ObjectManager::Draw() {
	// 描画処理
	for (auto& i : objects_) {
		i->Draw();
	}
}
