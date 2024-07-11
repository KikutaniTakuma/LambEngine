#include "ObjectManager.h"
#include "Utils/EngineInfo.h"

#include "TransformCompUpdater.h"

#include "GameObject/Comp/ObbPushComp.h"
#include "GameObject/Comp/Camera3DComp.h"


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

void ObjectManager::Finalize()
{
	instance_.reset();
}

void ObjectManager::SetLevelData(Lamb::SafePtr<LevelData> levelData) {
	assert(levelData.have());
	for (auto& i : levelData->objects) {
		this->Set(i.get());
	}

	SetCamera();
}

const Mat4x4& ObjectManager::GetCameraMatrix() const
{
	return cameraComp_.have() ? cameraComp_->GetMatrix() : Mat4x4::kIdentity;
}

void ObjectManager::Set(const Lamb::SafePtr<Object>& object) {
	if (not objects_.contains(object) and object.have()) {
		objects_.insert(object);
		if (object->HasComp<ObbPushComp>()) {
			obbObjects_.push_back(object->GetComp<ObbPushComp>());
		}
	}
}

void ObjectManager::Erase(const Lamb::SafePtr<Object>& object) {
	if (objects_.contains(object)) {
		objects_.erase(object);
	}
}

void ObjectManager::Clear() {
	objects_.clear();
	obbObjects_.clear();
}

void ObjectManager::SetCamera(const Lamb::SafePtr<Camera>& camera) {
	for (auto& i : objects_) {
		i->SetCamera(camera.get());
	}
}

bool ObjectManager::SetCamera() {
	Lamb::SafePtr<Object> cameraObject;

	for (auto& i : objects_) {
		if (i->HasTag("Camera3D")) {
			cameraComp_ = i->GetComp<Camera3DComp>();
			cameraObject = i;
			break;
		}
	}

	if (cameraObject.empty()) {
		return false;
	}

	for (auto& i : objects_) {
		if (i == cameraObject) {
			continue;
		}
		else if (cameraObject.have()) {
			i->SetCamera(cameraComp_.get());
		}
		else {
			i->SetCamera(cameraComp_.get());
		}
	}

	return true;
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

	TransformCompUpdater::GetInstance()->UpdateMatrix();

	// 当たり判定
	for (auto i = obbObjects_.begin(); i != obbObjects_.end(); i++) {
		// 二重forで全部と当たり判定を取ると同じ組み合わせで2回当たり判定をとってしまうので
		// 2番目のループで1ループの値で初期化する
		for (auto j = i; j != obbObjects_.end(); j++) {
			if (j == i) {
				continue;
			}
			(*i)->Collision(&(*j)->GetObbComp());
		}
	}

	TransformCompUpdater::GetInstance()->UpdateMatrix();

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
