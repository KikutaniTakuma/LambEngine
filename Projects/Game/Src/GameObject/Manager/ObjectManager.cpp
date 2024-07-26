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
	Clear();
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
		this->Set(i);
	}

	SetCamera();
}

const Mat4x4& ObjectManager::GetCameraMatrix() const
{
	return cameraComp_->GetCameraMatrix();
}

const Vector3& ObjectManager::GetCameraPos() const
{
	return cameraComp_->GetTransformComp().translate;
}

void ObjectManager::Set(const Lamb::SafePtr<Object>& object) {
	auto itr = std::find_if(
		objects_.begin(), objects_.end(),
		[&object](const std::unique_ptr<Object>& element)->bool {
			return object.get() == element.get();
		}
	);

	if (itr ==  objects_.end() and object.have()) {
		objects_.insert(std::unique_ptr<Object>(object.get()));
		if (object->HasComp<ObbPushComp>()) {
			obbObjects_.push_back(object->GetComp<ObbPushComp>());
		}
		for (const auto& i : object->GetTags()) {
			objectTags_.insert(std::make_pair(i, true));
		}
	}
}

void ObjectManager::Erase(const Lamb::SafePtr<Object>& object) {
	auto itr = std::find_if(
		objects_.begin(), objects_.end(),
		[&object](const std::unique_ptr<Object>& element)->bool {
			return object.get() == element.get();
		}
	);
	if (itr != objects_.end()) {
		objects_.erase(itr);
	}
}

void ObjectManager::Clear() {
	objects_.clear();
	obbObjects_.clear();

	cameraComp_ = nullptr;
}

bool ObjectManager::SetCamera() {
	Lamb::SafePtr<Object> cameraObject;

	for (auto& i : objects_) {
		if (i->HasTag("Camera3D")) {
			cameraComp_ = i->GetComp<Camera3DComp>();
			cameraObject = i.get();
			break;
		}
	}

	if (cameraObject.empty()) {
		return false;
	}

	for (auto& i : objects_) {
		if (i.get() == cameraObject.get()) {
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
	Collision();

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
	renderingManager->SetCameraMatrix(cameraComp_->GetCameraMatrix());
	renderingManager->SetCameraPos(cameraComp_->GetTransformComp().translate);
}

void ObjectManager::Draw() {
	// 描画処理
	for (auto& i : objects_) {
		i->Draw();
	}
}

void ObjectManager::Collision() {
	// 当たり判定(押し出し)
	for (auto i = obbObjects_.begin(); i != obbObjects_.end(); i++) {
		// 二重forで全部と当たり判定を取ると同じ組み合わせで2回当たり判定をとってしまうので
		// 2番目のループで1ループの値で初期化する
		for (auto j = i; j != obbObjects_.end(); j++) {
			if (j == i) {
				continue;
			}
			// 当たり判定(押し出し)
			(*i)->Collision(&(*j)->GetObbComp());
			// 当たり判定(押し出さない)
			(*i)->GetObbComp().CollisionHasTag(&(*j)->GetObbComp());
		}
	}
}

void ObjectManager::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Objects");
	RenderingManager::GetInstance()->Debug("RendeirngSetting");

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
	if (ImGui::Button("AddObject")) {
		auto newObject = std::make_unique<Object>();
		newObject->SetCamera(cameraComp_.get());
		objects_.insert(std::move(newObject));
	}

	size_t objectCount = 0;
	bool isErase = false;
	for (auto itr = objects_.begin(); itr != objects_.end(); itr++) {
		for (auto& tag : objectTags_) {
			if (((*itr)->HasTag(tag.first) and tag.second) or (*itr)->GetTags().empty()) {
				bool isCamera = (*itr)->HasComp<Camera3DComp>();
				bool isButton = isCamera ? false : ImGui::Button("erase object");
				if (not isButton) {
					if (not isCamera) { ImGui::SameLine(); }
					(*itr)->Debug("object_" + std::to_string(objectCount));
				}
				objectCount++;

				if (isButton) {
					if ((*itr)->HasComp<ObbPushComp>()) {
						auto obbComp = (*itr)->GetComp<ObbPushComp>();
						auto obbCompItr = std::find_if(
							obbObjects_.begin(),
							obbObjects_.end(),
							[&obbComp](const Lamb::SafePtr<ObbPushComp>& element)->bool {
								return obbComp == element.get();
							}
						);
						obbObjects_.erase(obbCompItr);
					}
					
					objects_.erase(itr);
					isErase = true;
					break;
				}
				
				break;
			}
		}
		if (isErase) {
			break;
		}
	}
	ImGui::End();
#endif // _DEBUG
}
