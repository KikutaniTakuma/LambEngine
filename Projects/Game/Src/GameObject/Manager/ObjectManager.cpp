#include "ObjectManager.h"
#include "Utils/EngineInfo.h"

#include "TransformCompUpdater.h"

#include "GameObject/Comp/ObbPushComp.h"
#include "GameObject/Comp/CameraComp.h"

#include "Engine/Graphics/RenderingManager/RenderingManager.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include <string>
#ifdef USE_DEBUG_CODE
#include "imgui.h"
#endif // USE_DEBUG_CODE
#include <fstream>

#include "Utils/FileUtils.h"
#include "CompCollisionManager.h"


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

#ifdef USE_DEBUG_CODE
	instance_->levelDataFilePathes_ = Lamb::GetFilePathFormDir("./SceneData/", ".json");
#endif // USE_DEBUG_CODE

	CompCollisionManager::Initialize();
	instance_->collisionManager_ = CompCollisionManager::GetInstance();
}

void ObjectManager::Finalize()
{
	instance_.reset();
}

void ObjectManager::SetLevelData(Lamb::SafePtr<LevelData> levelData) {
	assert(levelData.have());
	currentSceneName_ = levelData->name;
	levelDatas_[currentSceneName_].reset(levelData.get());
	for (auto& i : levelDatas_[currentSceneName_]->objects) {
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

	if (itr == objects_.end() and object.have()) {
		objects_.insert(std::unique_ptr<Object>(object.get()));
		for (const auto& i : object->GetTags()) {
			objectTags_.insert(std::make_pair(i.second, true));
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
	objectTags_.clear();
	objectTagKeys_.clear();
	collisionManager_->Clear();
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
	static constexpr float32_t deltaTime = 1.0f / 60.0f;

	// デルタタイムセット
	for (auto& i : objects_) {
		i->SetDeltaTime(deltaTime);
	}
	TransformCompUpdater::GetInstance()->UpdateMatrix();

	// デバッグ
#ifdef USE_DEBUG_CODE
	cameraComp_->LastUpdate();
	isLoad_ = false;
	Debug();
	if (isLoad_) {
		return;
	}
	TransformCompUpdater::GetInstance()->Guizmo(cameraComp_.get());
#endif // USE_DEBUG_CODE


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
	collisionManager_->Collision();

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
#ifdef USE_DEBUG_CODE
	if (isLoad_) {
		return;
	}
#endif // USE_DEBUG_CODE

	// 描画処理
	for (auto& i : objects_) {
		i->Draw(cameraComp_.get());
	}
}

void ObjectManager::Debug() {
#ifdef USE_DEBUG_CODE
	auto windowHandle = WindowFactory::GetInstance()->GetHwnd();

	ImGui::Begin("Objects");
	ImGui::Text("current scene : %s", currentSceneName_.c_str());
	ImGui::Text("select file name : %s", currentSceneFilePath_.c_str());
	if (ImGui::TreeNode("ファイル")) {
		inputSceneName_.resize(32);
		ImGui::Text("新規ファイル : ");
		ImGui::SameLine();
		ImGui::InputText(".json", inputSceneName_.data(), inputSceneName_.size());
		if (ImGui::Button("新規ファイルを作成して保存する")) {
			std::string newFilePath;
			for (auto& i : inputSceneName_) {
				if (i == '\0') {
					break;
				}
				newFilePath += i;
			}
			if (newFilePath.empty()) {
				newFilePath = "newfile";
			}
			newFilePath = "./SceneData/" + newFilePath + ".json";
			if (std::filesystem::exists(newFilePath)) {
				int32_t button = MessageBoxA(
					windowHandle,
					("this file exists. -> " + newFilePath + "\nDo you want to overwrite?").c_str(), "ObjectManager",
					MB_OKCANCEL | MB_APPLMODAL | MB_ICONINFORMATION
				);

				if (button == IDOK) {
					currentSceneFilePath_ = newFilePath;
					Save();
				}
			}
			else {
				currentSceneFilePath_ = newFilePath;
				Save();
			}
		}
		if (ImGui::Button("ファイルパス再読み込み")) {
			levelDataFilePathes_ = Lamb::GetFilePathFormDir("./SceneData/", ".json");
		}
		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
		for (auto& i : levelDataFilePathes_) {
			if (ImGui::Button(i.string().c_str())) {
				currentSceneFilePath_ = i.string();
			}
		}
		ImGui::EndChild();

		if (ImGui::Button("ロード")) {
			Load(currentSceneFilePath_);
			ImGui::TreePop();
			ImGui::End();

			isLoad_ = true;
			return;
		}
		if (ImGui::Button("保存")) {
			Save();
		}
		ImGui::TreePop();
	}


	RenderingManager::GetInstance()->Debug("RenderSetting");

	if (ImGui::TreeNode("sort")) {
		for (auto& object : objects_) {
			for (const auto& i : object->GetTags()) {
				if (not objectTags_.contains(i.second)) {
					objectTags_.insert(std::make_pair(i.second, true));

					objectTagKeys_.reserve(objectTags_.size());
					objectTagKeys_.push_back(i.second);

					std::sort(objectTagKeys_.begin(), objectTagKeys_.end());
				}
			}
		}
		ImGui::Checkbox("選択しているものを表示しない", &isSelectInverse_);
		ImGui::SameLine();
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
		for (size_t tagCount = 0, sameLineCount = 0; auto & i : objectTagKeys_) {
			ImGui::Checkbox(i.c_str(), &objectTags_[i]);
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
	if (ImGui::Button("当たり判定のペア更新")) {
		collisionManager_->MakeCollisionPair();
	}
	if (ImGui::Button("AddObject")) {
		auto newObject = std::make_unique<Object>();
		objects_.insert(std::move(newObject));
	}
	if (ImGui::Button("ClearObject")) {
		int32_t button = MessageBoxA(
			windowHandle,
			"All Objects erase. Accepted?", "ObjectManager",
			MB_OKCANCEL | MB_APPLMODAL | MB_ICONINFORMATION
		);

		if (button == IDOK) {
			Lamb::SafePtr<Object> cameraObject;
			for (auto i = objects_.begin(); i != objects_.end();) {
				if ((*i)->HasComp<CameraComp>()) {
					cameraObject = i->get();
					i++;
				}
				else {
					i = objects_.erase(i);
				}
			}

			objectTags_.clear();
			collisionManager_->Clear();
			objectTagKeys_.clear();

			for (auto& i : cameraObject->GetTags()) {
				objectTags_.insert(std::make_pair(i.second, true));
			}
			objectTagKeys_.reserve(objectTags_.size());
			for (auto& i : objectTags_) {
				objectTagKeys_.push_back(i.first);
			}

			std::sort(objectTagKeys_.begin(), objectTagKeys_.end());

			if (cameraObject->HasComp<ObbComp>()) {
				collisionManager_->Set(cameraObject->GetComp<ObbComp>());
			}
			if (cameraObject->HasComp<ObbPushComp>()) {
				collisionManager_->Set(cameraObject->GetComp<ObbPushComp>());
			}
		}
	}

	ImGui::BeginChild(ImGui::GetID((void*)0), { 0.0f, 200.0f }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoTitleBar);
	size_t objectCount = 0;
	bool isErase = false;
	bool isAddComp = false;
	for (auto itr = objects_.begin(); itr != objects_.end(); itr++) {
		for (auto& tag : objectTags_) {
			if (((*itr)->HasTag(tag.first) and (isSelectInverse_ ? not tag.second : tag.second)) or (*itr)->GetTags().empty()) {
				bool isCamera = (*itr)->HasComp<CameraComp>();
				bool isButton = isCamera ? false : ImGui::Button("erase object");
				if (not isButton) {
					if (not isCamera) { ImGui::SameLine(); }
					if ((*itr)->Debug("object_" + std::to_string(objectCount)) and not isAddComp) {
						isAddComp = true;
					}
				}
				objectCount++;

				if (isButton) {
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

	ImGui::EndChild();
	ImGui::End();
#endif // USE_DEBUG_CODE
}

void ObjectManager::Save() {
	nlohmann::json root;
	root = nlohmann::json::object();

	std::filesystem::path filePath = currentSceneFilePath_;
	std::string fileName = filePath.stem().string();

	auto windowHandle = WindowFactory::GetInstance()->GetHwnd();

	if (std::filesystem::exists(filePath)) {
		int32_t button = MessageBoxA(
			windowHandle,
			("this file exists. -> " + filePath.string() + "\nDo you want to overwrite?").c_str(), "ObjectManager",
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
				("Would you like to save it as " + filePath.string() + " ?").c_str(), "ObjectManager",
				MB_OKCANCEL | MB_APPLMODAL | MB_ICONINFORMATION
			) == IDCANCEL
				) {
				return;
			}
		}
	}

	// シーンを名前
	root["scene"] = fileName;

	RenderingManager::GetInstance()->Save(root);

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
#ifdef USE_DEBUG_CODE
		assert(!"file can not open");
#else
		throw Lamb::Error::Code<ObjectManager>("file can not open", ErrorPlace);
#endif // USE_DEBUG_CODE
	}
}

void ObjectManager::Load(const std::string& jsonFileName) {
	for (auto& i : objects_) {
		i->Finalize();
	}

	Clear();

	auto jsonFile = Lamb::LoadJson(jsonFileName);
	currentSceneFilePath_ = jsonFileName;

	currentSceneName_ = jsonFile["scene"].get<std::string>();
	if (jsonFile.contains("RederingSetting")) {
		RenderingManager::GetInstance()->Load(jsonFile);
	}

	levelDatas_[currentSceneName_].reset(new LevelData());
	LevelData& levelData = *levelDatas_[currentSceneName_];
	levelData.name = currentSceneName_;
	levelData.objects.reserve(jsonFile["objects"].size());

	// オブジェクトを追加
	for (auto& objectData : jsonFile["objects"]) {
		levelData.objects.push_back(Lamb::MakeSafePtr<Object>());
		Object& object = *levelData.objects.back();
		object.Load(objectData["Comps"]);
	}

	// オブジェクトをセット
	for (auto& i : levelDatas_[currentSceneName_]->objects) {
		this->Set(i);
	}

	objectTagKeys_.reserve(objectTags_.size());
	for (auto& i : objectTags_) {
		objectTagKeys_.push_back(i.first);
	}

	std::sort(objectTagKeys_.begin(), objectTagKeys_.end());

	SetCamera();

	collisionManager_->MakeCollisionPair();

#ifdef USE_DEBUG_CODE
	MessageBoxA(
		WindowFactory::GetInstance()->GetHwnd(),
		("Load " + jsonFileName + " succeeded").c_str(), "ObjectManager",
		MB_OK | MB_APPLMODAL | MB_ICONINFORMATION
	);
#endif // USE_DEBUG_CODE
}
