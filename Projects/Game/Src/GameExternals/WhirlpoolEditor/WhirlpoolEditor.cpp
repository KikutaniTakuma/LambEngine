#include "WhirlpoolEditor.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <Windows.h>

#include "imgui.h"
#include "json.hpp"

#include "GameExternals/WhirlpoolManager/WhirlpoolManager.h"
#include "GameExternals/StageManager/StageManager.h"
#include "Camera/Camera.h"
#include "Utils/ExecutionLog.h"

WhirlpoolEditor::WhirlpoolEditor() {}

void WhirlpoolEditor::Initialize() {
	scale_ = Vector3::kIdentity;
	rotate_ = Vector3::kZero;
	position_ = Vector3::kZero;

	model_ = std::make_unique<ModelInstance>();
	model_->Load("./Resources/Cube.obj");

	gameOverSpeed_ = 0.0f;

	fileName_ = "Whirlpool";
	stageIndex_ = StageManager::GetStage();
	saveString_.resize(128);
	saveString_ = StageManager::GetStageString(stageIndex_);
	loadString_.resize(128);
	loadString_ = StageManager::GetStageString(stageIndex_);

	isCreate_ = false;
}

void WhirlpoolEditor::Update() {
	if (ImGui::TreeNode("WhirlpoolEditor")) {
		Vector3  whirlpoolScale{}, whirlpoolRotate{}, whirlpoolPos{};
		float gameOverSpeed = 0.0f;
		for (size_t i = 0; auto & whirlpool : whirlpoolManager_->GetWhirlpools()) {
			if (whirlpool.get() == nullptr) {
				continue;
			}
			if (ImGui::TreeNode(("Whirlpool:" + std::to_string(i)).c_str())) {
				whirlpoolScale = whirlpool->GetScale();
				whirlpoolRotate = whirlpool->GetRotate();
				whirlpoolPos = whirlpool->GetPosition();
				gameOverSpeed = whirlpool->GetGameOverSpeed();

				ImGui::DragFloat3(("scale:" + std::to_string(i)).c_str(), &whirlpoolScale.x, 1.0f);
				ImGui::DragFloat3(("rotate:" + std::to_string(i)).c_str(), &whirlpoolRotate.x, 0.01f);
				ImGui::DragFloat3(("position:" + std::to_string(i)).c_str(), &whirlpoolPos.x, 1.0f);
				ImGui::DragFloat(("gameOverSpeed:" + std::to_string(i)).c_str(), &gameOverSpeed, 1.0f);

				whirlpool->SetScale(whirlpoolScale);
				whirlpool->SetRotate(whirlpoolRotate);
				whirlpool->SetPosition(whirlpoolPos);
				whirlpool->SetGameOverSpeed(gameOverSpeed);
				if (ImGui::Button("Delete")) {
					whirlpoolManager_->DeleteWhirlpool(i);
				}
				ImGui::TreePop();
			}
			i++;
		}
		if (ImGui::TreeNode("CreateWhirlpool")) {
			ImGui::DragFloat3("scale", &scale_.x, 0.25f);
			ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
			ImGui::DragFloat3("position", &position_.x, 0.25f);
			ImGui::DragFloat("gameOverSpeed", &gameOverSpeed_, 1.0f);
			if (ImGui::Button("Create")) {
				whirlpoolManager_->Create(scale_, rotate_, position_, gameOverSpeed_);
			}
			ImGui::TreePop();
			model_->scale = scale_;
			model_->rotate = rotate_;
			model_->pos = position_;
			isCreate_ = true;
		}
		else {
			isCreate_ = false;
		}
		ImGui::TreePop();
	}
	model_->Update();
}

void WhirlpoolEditor::Draw(const Camera& camera) {
	if (isCreate_) {
		model_->Draw(camera.GetViewProjection());
	}
}

void WhirlpoolEditor::LoadFile(const std::string& stageName) {
	std::string stageNameTmp;
	for (auto& i : stageName) {
		if (i == '\0') {
			break;
		}
		stageNameTmp += i;
	}
	std::string fileNameTmp;
	for (auto& i : fileName_) {
		if (i == '\0') {
			break;
		}
		fileNameTmp += i;
	}
	const std::filesystem::path kDirectoryPath = "./Datas/" + StageManager::GetStageDataString() + "/" + stageNameTmp + "/" + fileName_ + "/" + fileName_;
	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath.string() + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Map Editor - Load", 0);
		return;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(fileName_);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
	}
	int loop = 0;
	// アイテム
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());//アイテム名がオブジェクトデータだった場合、登録


		if (itemName == "objectData") {

			//各オブジェクトについて
			for (nlohmann::json::iterator itObjectData = itObject->begin(); itObjectData != itObject->end(); ++itObjectData) {

				//アイテム名を取得
				const std::string& objectName = itObjectData.key();

				//グループを検索
				nlohmann::json::iterator itData = itObject->find(objectName);

				//未登録チェック
				if (itData == itObject->end()) {
					MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
				}

				//保険
				assert(itData != itObject->end());

				std::vector<Vector3>scale{}, rotate{}, pos{};
				std::vector<float> gameOverSpeed{};
				for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

					//アイテム名を取得
					const std::string& itemNameObject = itItemObject.key();

					//要素数3の配列であれば
					if (itItemObject->is_array() && itItemObject->size() == 3) {

						//名前がpositionだった場合、positionを登録
						if (itemNameObject == "position") {
							//float型のjson配列登録
							pos.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がrotationだった場合、rotationを登録
						else if (itemNameObject == "rotate") {
							//float型のjson配列登録
							rotate.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}
						//名前がscaleだった場合、scaleを登録
						else if (itemNameObject == "scale") {
							//float型のjson配列登録
							scale.emplace_back(Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
						}

					}
					else {
						if (itemNameObject == "gameOverSpeed") {
							float speed = itItemObject->get<float>();
							gameOverSpeed.emplace_back(speed);
						}
					}
				}

				// 生成
				for (size_t i = 0; i < pos.size(); i++) {
					whirlpoolManager_->Create(scale.at(i), rotate.at(i), pos.at(i), gameOverSpeed.at(i));
				}
			}
		}

		loop++;
	}
}
void WhirlpoolEditor::Clear() {
	for (int i = 0; i < whirlpoolManager_->GetWhirlpools().size(); i++) {
		whirlpoolManager_->DeleteWhirlpool(i);
	}

}
void WhirlpoolEditor::SaveFile(const std::string& stageName) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	for (size_t i = 0; auto & whirlpoolData: whirlpoolManager_->GetWhirlpools()) {
		root[fileName_]["objectData"][("Whirlpool:" + std::to_string(i)).c_str()]["scale"] = nlohmann::json::array({ whirlpoolData->GetScale().x, whirlpoolData->GetScale().y, whirlpoolData->GetScale().z });
		root[fileName_]["objectData"][("Whirlpool:" + std::to_string(i)).c_str()]["rotate"] = nlohmann::json::array({ whirlpoolData->GetRotate().x, whirlpoolData->GetRotate().y, whirlpoolData->GetRotate().z });
		root[fileName_]["objectData"][("Whirlpool:" + std::to_string(i)).c_str()]["position"] = nlohmann::json::array({ whirlpoolData->GetPosition().x, whirlpoolData->GetPosition().y, whirlpoolData->GetPosition().z });
		root[fileName_]["objectData"][("Whirlpool:" + std::to_string(i)).c_str()]["gameOverSpeed"] = whirlpoolData->GetGameOverSpeed();

		i++;
	}

	std::string stageNameTmp;
	for (auto& i : stageName) {
		if (i == '\0') {
			break;
		}
		stageNameTmp += i;
	}

	const std::filesystem::path kDirectoryPath = "./Datas/" + StageManager::GetStageDataString() + "/" + stageNameTmp + "/" + fileName_ + "/";

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	std::string fileNameTmp;
	for (auto& i : fileName_) {
		if (i == '\0') {
			break;
		}
		fileNameTmp += i;
	}
	std::string filePath = kDirectoryPath.string() + fileNameTmp + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}