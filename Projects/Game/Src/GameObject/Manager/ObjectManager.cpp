#include "ObjectManager.h"
#include "Utils/EngineInfo.h"

#include "TransformCompUpdater.h"

#include "GameObject/Comp/ObbPushComp.h"
#include "GameObject/Comp/CameraComp.h"

#include "Engine/Graphics/RenderingManager/RenderingManager.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include <string>
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG
#include <fstream>

#include "Utils/FileUtils.h"


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

#ifdef _DEBUG
	instance_->levelDataFilePathes_ = Lamb::GetFilePathFormDir("./SceneData/", ".json");
#endif // _DEBUG
}

void ObjectManager::Finalize()
{
	instance_.reset();
}

void ObjectManager::SetLevelData(Lamb::SafePtr<LevelData> levelData) {
	assert(levelData.have());
	currentScene_ = levelData->name;
	levelDatas_[currentScene_].reset(levelData.get());
	for (auto& i : levelDatas_[currentScene_]->objects) {
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
	for (auto& i : objects_) {
		if (i->HasComp<CameraComp>()) {
			cameraComp_ = i->GetComp<CameraComp>();
			break;
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
	TransformCompUpdater::GetInstance()->UpdateMatrix();

	// デバッグ
#ifdef _DEBUG
	cameraComp_->LastUpdate();
	isLoad_ = false;
	Debug();
	if (isLoad_) {
		return;
	}
	TransformCompUpdater::GetInstance()->Guizmo(cameraComp_.get());
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

	TransformCompUpdater::GetInstance()->UpdateMatrix();

	Lamb::SafePtr renderingManager = RenderingManager::GetInstance();
	renderingManager->SetCameraMatrix(cameraComp_->GetCameraMatrix());
	renderingManager->SetProjectionInverseMatrix(cameraComp_->GetToNdcMatrix().Inverse());
	renderingManager->SetCameraPos(cameraComp_->GetTransformComp().translate);
}

void ObjectManager::Draw() {
#ifdef _DEBUG
	if (isLoad_) {
		return;
	}
#endif // _DEBUG

	// 描画処理
	for (auto& i : objects_) {
		i->Draw(cameraComp_.get());
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
	inputSceneName_.resize(32);
	ImGui::InputText("fileName .json", inputSceneName_.data(), inputSceneName_.size());
	if (ImGui::Button("保存")) {
		Save();
	}
	ImGui::Text("current scene : %s", currentScene_.c_str());
	if (ImGui::Button("ファイルパス再読み込み")) {
		levelDataFilePathes_ = Lamb::GetFilePathFormDir("./SceneData/", ".json");
	}
	if (ImGui::TreeNode("ロード")) {
		for (auto& i : levelDataFilePathes_) {
			if (ImGui::Button(i.string().c_str())) {
				Load(i.string());
				ImGui::TreePop();
				ImGui::End();

				isLoad_ = true;
				return;
			}
		}

		ImGui::TreePop();
	}


	RenderingManager::GetInstance()->Debug("RendeirngSetting");

	if (ImGui::TreeNode("sort")) {
		for (auto& object : objects_) {
			for (const auto& i : object->GetTags()) {
				if (not objectTags_.contains(i)) {
					objectTags_.insert(std::make_pair(i, true));
				}
			}
		}
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
		objects_.insert(std::move(newObject));
	}

	size_t objectCount = 0;
	bool isErase = false;
	for (auto itr = objects_.begin(); itr != objects_.end(); itr++) {
		for (auto& tag : objectTags_) {
			if (((*itr)->HasTag(tag.first) and tag.second) or (*itr)->GetTags().empty()) {
				bool isCamera = (*itr)->HasComp<CameraComp>();
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

void ObjectManager::Save() {
	nlohmann::json root;
	root = nlohmann::json::object();

	std::string fileName;

	for (auto& i : inputSceneName_) {
		if (i == '\0') {
			break;
		}
		fileName += i;
	}

	if (fileName.empty()) {
		fileName = currentScene_;
	}

	std::string filePath = "./SceneData/" + fileName + ".json";
	auto windowHandle = WindowFactory::GetInstance()->GetHwnd();

	if (std::filesystem::exists(filePath)) {
		int32_t button = MessageBoxA(
			windowHandle,
			("this file exists. -> " + filePath + "\nDo you want to overwrite?").c_str(), "ObjectManager",
			MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONINFORMATION
		);

		if (button == IDCANCEL) {
			return;
		}
		else if (button == IDNO) {
			fileName += "_new";
			filePath = "./SceneData/" + fileName + ".json";

			if (MessageBoxA(
				windowHandle,
				("Would you like to save it as " + filePath + " ?").c_str(), "ObjectManager",
				MB_OKCANCEL | MB_APPLMODAL | MB_ICONINFORMATION
			) == IDCANCEL
				) {
				return;
			}
		}
	}

	// シーンを名前
	root["scene"] = fileName;
	// オブジェクト
	root["objects"] = nlohmann::json::array();

	for (auto& i : objects_) {
		// オブジェクト毎に追加
		root["objects"].push_back(nlohmann::json::object());

		// 今追加したjso::objectを渡す
		// 内部のコンポーネントを出力していく
		i->Save(root["objects"].back());
	}

	std::ofstream outputFile("./SceneData/" + fileName + ".json");
	if (outputFile.is_open()) {
		outputFile << std::setw(4) << root << std::endl;
		outputFile.close();

		MessageBoxA(
			windowHandle,
			"Save file succeeded", "ObjectManager",
			MB_OK | MB_APPLMODAL | MB_ICONINFORMATION
		);
	}
	else {
#ifdef _DEBUG
		assert(!"file can not open");
#else
		throw Lamb::Error::Code<ObjectManager>("file can not open", ErrorPlace);
#endif // _DEBUG
	}
}

void ObjectManager::Load(const std::string& jsonFileName) {
	objects_.clear();
	obbObjects_.clear();
	objectTags_.clear();
	cameraComp_ = nullptr;

	auto jsonFile = Lamb::LoadJson(jsonFileName);

	currentScene_ = jsonFile["scene"].get<std::string>();
	levelDatas_[currentScene_].reset(new LevelData());
	LevelData& levelData = *levelDatas_[currentScene_];
	levelData.name = currentScene_;
	levelData.objects.reserve(jsonFile["objects"].size());
	
	// オブジェクトを追加
	for (auto& objectData : jsonFile["objects"]) {
		if (objectData["type"].get<std::string>() == "Object") {
			levelData.objects.push_back(Lamb::MakeSafePtr<Object>());
			Object& object = *levelData.objects.back();
			object.Load(objectData["Comps"]);
		}
	}

	// オブジェクトをセット
	for (auto& i : levelDatas_[currentScene_]->objects) {
		this->Set(i);
	}

	SetCamera();

#ifdef _DEBUG
	MessageBoxA(
		WindowFactory::GetInstance()->GetHwnd(),
		("Load " + jsonFileName + " succeeded").c_str(), "ObjectManager",
		MB_OK | MB_APPLMODAL | MB_ICONINFORMATION
	);
#endif // _DEBUG
}
