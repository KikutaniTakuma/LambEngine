#include "BlockManager.h"

#include <cassert>
#include <filesystem>
#include <fstream>

#include "json.hpp"

#include "Game/Player/Player.h"
#include "Camera/Camera.h"
#include "Game/StageManager/StageManager.h"

BlockManager::BlockManager() {}

void BlockManager::Initialize() {
	blocks_.clear();
	LoadJson();
}

void BlockManager::Update() {
	for (auto& block : blocks_) {
		block->Update();
	}
}

void BlockManager::ResourceUpdate() {
	for (auto& block : blocks_) {
		block->ResourceUpdate();
	}
}

void BlockManager::Draw(const Camera& camera) {
	for (auto& block : blocks_) {
		block->Draw(camera);
	}
}

void BlockManager::Create(const Vector3& scale, const Vector3& rotate, const Vector3& position) {
	Block* block = new Block();
	block->SetPlayer(player_);
	block->Initialize(scale, rotate, position);
	blocks_.emplace_back(std::move(block));
}

void BlockManager::DeleteBlock(size_t index) {
	if (index < blocks_.size()) {
		// 指定された index の要素を削除
		blocks_.erase(blocks_.begin() + index);
	}
}

void BlockManager::LoadJson() {
	const std::filesystem::path kDirectoryPath = "./Datas/StageDatas/" + StageManager::GetCurrentStageString() + "/Blocks/Blocks";
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
	nlohmann::json::iterator itGroup = root.find("Blocks");
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
	}
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
				if (objectName.find("Block") != std::string::npos) {
					Vector3 pos{}, rotate{}, scale{};
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();
						//要素数3の配列であれば
						if (itItemObject->is_array() && itItemObject->size() == 3) {
							//名前がpositionだった場合、positionを登録
							if (itemNameObject == "position") {
								//float型のjson配列登録
								pos = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がrotationだった場合、rotationを登録
							else if (itemNameObject == "rotate") {
								//float型のjson配列登録
								rotate = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がscaleだった場合、scaleを登録
							else if (itemNameObject == "scale") {
								//float型のjson配列登録
								scale = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
						}
					}
					Create(scale,rotate,pos);
				}
			}
		}
	}
}
