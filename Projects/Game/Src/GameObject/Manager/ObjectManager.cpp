#include "ObjectManager.h"
#include "Utils/EngineInfo.h"

#include "TransformCompUpdater.h"

#include "GameObject/Comp/ObbPushComp.h"
#include "GameObject/Comp/Camera3DComp.h"

#include "Engine/Graphics/RenderingManager/RenderingManager.h"
#include <string>
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG



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

void ObjectManager::SetLevelData(Lamb::SafePtr<LevelData> levelData, Lamb::SafePtr<Camera> camera) {
	assert(levelData.have());
	for (auto& i : levelData->objects) {
		this->Set(i.get());
	}

	if (not SetCamera()) {
		SetCamera(camera);
	}

	TransformCompUpdater::GetInstance()->SetCameraMatrix(&cameraComp_->GetViewMatrix(), &cameraComp_->GetProjectionMatrix());
}

const Mat4x4& ObjectManager::GetCameraMatrix() const
{
	return cameraComp_.have() ? cameraComp_->GetMatrix() : camera_->GetViewProjection();
}

const Vector3& ObjectManager::GetCameraPos() const
{
	return cameraComp_.have() ? cameraComp_->GetPos() : camera_->GetPos();
}

void ObjectManager::Set(const Lamb::SafePtr<Object>& object) {
	if (not objects_.contains(object) and object.have()) {
		objects_.insert(object);
		if (object->HasComp<ObbPushComp>()) {
			obbObjects_.push_back(object->GetComp<ObbPushComp>());
		}
		for (const auto& i : object->GetTags()) {
			objectTags_.insert(std::make_pair(i, true));
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

	cameraComp_ = nullptr;
	camera_ = nullptr;
}

void ObjectManager::SetCamera(const Lamb::SafePtr<Camera>& camera) {
	camera_ = camera;
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

	// デバッグ
#ifdef _DEBUG
	TransformCompUpdater::GetInstance()->UpdateMatrix();
	cameraComp_->LastUpdate();
	Debug();
#endif // _DEBUG


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

	Lamb::SafePtr renderingManager = RenderingManager::GetInstance();
	renderingManager->SetCameraMatrix(cameraComp_->GetMatrix());
	renderingManager->SetCameraPos(cameraComp_->GetPos());
}

void ObjectManager::Draw() {
	// 描画処理
	for (auto& i : objects_) {
		i->Draw();
	}
}

void ObjectManager::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Objects");
	RenderingManager::GetInstance()->Debug("rendeirngSetting");

	if (ImGui::TreeNode("sort")) {
		if (ImGui::Button("すべてを選択")) {
			for (auto& i : objectTags_) {
				i.second = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("すべての選択を解除")) {
			for (auto& i : objectTags_) {
				i.second = false;
			}
		}
		for (size_t tagCount = 0, sameLineCount = 0; auto& i : objectTags_) {
			ImGui::Checkbox(i.first.c_str(), &i.second);
			tagCount++;
			sameLineCount++;
			if (sameLineCount < 3 and tagCount < objectTags_.size()) {
				ImGui::SameLine();
			}
			else {
				sameLineCount = 0;
			}
		}
		ImGui::TreePop();
	}


	for (size_t i = 0; auto& object : objects_) {
		for (auto& tag : objectTags_) {
			if (object->HasTag(tag.first) and tag.second) {
				object->Debug("object_" + std::to_string(i));
				break;
			}
		}
		i++;
	}
	ImGui::End();
#endif // _DEBUG
}
