#include "Minimap.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/StageManager/StageManager.h"
#include "Game/Player/Player.h"
#include "Input/Gamepad/Gamepad.h"
#include "Math/Quaternion.h"

#include "imgui.h"
#include "json.hpp"
#include <fstream>
#include <numbers>
#include <cmath>

void Minimap::Initialize(const Vector3& playerPos, const Vector3& goalPos)
{
	line_.Init(false);

	playerFirstPos_ = playerPos;

	isCustomize_ = true;

	currentPlayerMoveHistoryIndex_ = 0;
	savePlayerMoveHistoryIndex_ = 0;
	playerMoveHistory_[currentPlayerMoveHistoryIndex_].push_back(playerFirstPos_);

	localCamera_.reset(new Camera{});
	localCamera_->rotate.x = 1.57f;
	localCamera_->pos.y = 230.0f;
	localCamera_->pos.z = 65.0f;

	addPosLengthBasis_ = 2.0f;

	staticCamera_.reset(new Camera{});
	staticCamera_->pos.z = -10.0f;
	staticCamera_->Update();


	miniMapScaleCusomize_ = { 408.0f, 580.0f };
	miniMapScaleInGame_ = { 305.0f, 403.0f };
	miniMapPosCusomize_ = { 373.0f, -35.0f };
	miniMapPosInGame_ = {422.0f, 73.0f};

	scaleBasis_ = (Vector3::kXIndentity + Vector3::kYIndentity) * 300.0f;
	miniMapScale = miniMapScaleCusomize_;
	miniMapPos = miniMapPosCusomize_;


	pera_.reset(new PeraRender{});
	pera_->Initialize("./Resources/Shaders/PostShader/PostNone.PS.hlsl");

	peraScale_ = Vector2::identity * 512.0f;


	cameraMoveSpeed_ = 1.0f;

	maxZoom_ = 350.0f;
	minZoom_ = 100.0f;
	zoomSpeed_ = 100.0f;

	objectScale_ = 2.0f;
	playerScale_ = 3.0f * objectScale_;

	windAnimationSpeed_ = 0.5f;
	whirpoolRotateSpeed_ = std::numbers::pi_v<float>;

	texturePathes_[Type::kBlock] = std::string{"./Resources/MinimapTextures/Block.png"};
	texturePathes_[Type::kGoal] = std::string{"./Resources/MinimapTextures/Goal.png"};
	texturePathes_[Type::kPlayer] = std::string{ "./Resources/MinimapTextures/Player.png" };
	texturePathes_[Type::kWind] = std::string{"./Resources/MinimapTextures/Wind.png"};
	texturePathes_[Type::kWhirpool] = std::string{"./Resources/MinimapTextures/Whirpool.png"};
	texturePathes_[Type::kDeathPoint] = std::string{"./Resources/MinimapTextures/deathPoint.png"};

	// 色初期化
	colors_[Type::kBlock] = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	colors_[Type::kGoal] = Vector4{ 0.8f,0.2f,0.2f,1.0f };
	colors_[Type::kPlayer] = Vector4{ 0.2f,0.8f,0.2f,1.0f };
	colors_[Type::kWind] = Vector4{ 1.0f,1.0f, 1.0f,0.8f };
	colors_[Type::kWhirpool] = Vector4{ 0.2f,0.2f,0.8f,1.0f };
	colors_[Type::kBeach] = Vector4{ 0.8f,0.8f,0.2f,1.0f };

	isDeathPositionsDraw_ = { false };


	// ブロックのデータロード
	LoadBlockData();

	// ゴールのデータロード
	LoadGoalData();

	// 渦潮のデータロード
	LoadWhirlpool();

	// 風のデータロード
	LoadWind();

	// 描画する板ポリのインスタンスを生成
	CreateTex2D();

	AdjustCameraPos(playerPos, goalPos);

	firstCameraPos_ = localCamera_->pos;

	gridTexture_.reset(new Texture2D{ "./Resources/MinimapTextures/grid.png" });
	gridTexture_->scale *= 200.0f;
	gridTexture_->uvSize *= 15.0f;
	gridTexture_->color = Vector4ToUint({0.0f,0.0f,0.0f, 1.0f});
	gridTexture_->rotate.x = std::numbers::pi_v<float> * 0.5f;
	gridTexture_->Update();

	isStop_ = false;
}

void Minimap::Finalize()
{
	line_.Finalize();
}

void Minimap::Update()
{
	if (isCustomize_) {
		CameraMove();
	}

	if (isCustomize_) {
		localCamera_->scale.x = (peraScale_.x) / Lamb::ClientSize().x * (miniMapScale.x / scaleBasis_.x);
		localCamera_->scale.y = (peraScale_.y) / Lamb::ClientSize().y * (miniMapScale.y / scaleBasis_.y);
	}
	localCamera_->Update();
	pera_->pos = isCustomize_ ? miniMapPosCusomize_ : miniMapPosInGame_;
	pera_->scale = isCustomize_ ? miniMapScaleCusomize_ : miniMapScaleInGame_;
	pera_->Update();

	minimapBackGround_->Update();

	for (auto& i : blockTextures_) {
		i->Update();
	}
	for (auto& i : whirpoolTextures_) {
		i->rotate.y += whirpoolRotateSpeed_ * Lamb::DeltaTime();
		i->Update();
	}
	for (auto& i : windTextures_) {
		i->uvPibot.y += windAnimationSpeed_ * Lamb::DeltaTime();
		i->Update();
	}
	playerTex_->Update();
	goalTex_->Update();
	rightBeachTex_->Update();
	leftBeachTex_->Update();

	for (auto& i : deathPosTextures_) {
		i->scale = playerTex_->scale;
		i->Update();
	}
}

void Minimap::Draw([[maybe_unused]]const Vector3& playerPos)
{
	pera_->PreDraw();

	//minimapBackGround_->Draw(localCamera_->GetViewOthographics(), Pipeline::Normal, false);

	gridTexture_->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);

	for (auto& i : windTextures_) {
		i->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);
	}
	for (auto& i : whirpoolTextures_) {
		i->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);
	}
	for (auto& i : blockTextures_) {
		i->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);
	}
	rightBeachTex_->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);
	leftBeachTex_->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);
	playerTex_->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);
	goalTex_->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);
	
	const Vector3 kEpsilon = { static_cast<float>(1e-1) * 0.5f, 0.0f,static_cast<float>(1e-1) * 0.5f };

	for (size_t count = 0llu; auto & moveHistory : playerMoveHistory_) {
		uint32_t lineColor = Vector4ToUint(Vector4{ 0.5f,0.5f,0.5f,1.0f });
		if (count == currentPlayerMoveHistoryIndex_) {
			lineColor = std::numeric_limits<uint32_t>::max();
		}


		for (size_t i = 0; i < moveHistory.size(); i++) {
			if (i + 1 != moveHistory.size()) {
				line_.Draw(
					moveHistory[i],
					moveHistory[i + 1],
					localCamera_->GetViewProjection(),
					lineColor
				);
				line_.Draw(
					moveHistory[i] - kEpsilon,
					moveHistory[i + 1] - kEpsilon,
					localCamera_->GetViewProjection(),
					lineColor
				);
				line_.Draw(
					moveHistory[i] + kEpsilon,
					moveHistory[i + 1] + kEpsilon,
					localCamera_->GetViewProjection(),
					lineColor
				);
			}
		}
		if (isDeathPositionsDraw_[count]) {
			deathPosTextures_[count]->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);
		}

		count++;
	}
	line_.AllDraw();

	playerTex_->Draw(localCamera_->GetViewProjection(), Pipeline::Normal, false);

	pera_->Draw(staticCamera_->GetViewOthographics(), Pipeline::Normal);
}

void Minimap::Debug()
{
#ifdef _DEBUG
	ImGui::Begin("Minimap");
	localCamera_->Debug("localCamera", true);
	ImGui::DragFloat3("minimap pos", &miniMapPos.x);
	ImGui::DragFloat3("minimap scale", &miniMapScale.x);
	ImGui::DragFloat("カメラの移動速度", &cameraMoveSpeed_,0.01f);
	ImGui::Text("ゴールとプレイヤーの距離 : %f", (playerTex_->pos.x - goalTex_->pos.x)/*.Length()*/);
	ImGui::End();
#endif // _DEBUG
}

void Minimap::SetPlayerState(const Vector3& scale, const Vector3& rotate, const Vector3& pos)
{
	playerTex_->scale.x = scale.x * playerScale_;
	playerTex_->scale.y = scale.z * playerScale_;

	playerTex_->rotate.x = std::numbers::pi_v<float> *0.5f;
	playerTex_->rotate.y = rotate.y;

	playerTex_->pos = pos;
}

void Minimap::SetPlayerDeathPos(const Vector3& deathPos)
{
	if (!isStop_) {
		playerMoveHistory_[savePlayerMoveHistoryIndex_].push_back(deathPos);
		deathPositions_[currentPlayerMoveHistoryIndex_] = deathPos;
		deathPosTextures_[currentPlayerMoveHistoryIndex_]->pos = deathPositions_[currentPlayerMoveHistoryIndex_];
		StopAddPlayerPos();
	}
}

void Minimap::AddPlayerPos(const Vector3& pos)
{
	if (!isCustomize_ && addPosLengthBasis_ < GetPreToCurrentLength(pos) && !isStop_) {
		playerMoveHistory_[savePlayerMoveHistoryIndex_].push_back(pos);
	}
}

void Minimap::NewMoveHistory()
{

	// インデックスを進める
	savePlayerMoveHistoryIndex_++;
	// インデックスが配列数を超えてたら0にする
	if (playerMoveHistory_.size() <= static_cast<size_t>(savePlayerMoveHistoryIndex_)) {
		savePlayerMoveHistoryIndex_ = 0;
	}

	// 次のを初期化
	playerMoveHistory_[savePlayerMoveHistoryIndex_].clear();

	// 最初の位置を追加
	playerMoveHistory_[savePlayerMoveHistoryIndex_].push_back(playerFirstPos_);

	isStop_ = false;
}

void Minimap::SwitchCustomizeAndInGame()
{
	// ステータスを変更
	isCustomize_ = !isCustomize_;

	if (!isCustomize_) {
		NewMoveHistory();
	}
	else {
		isDeathPositionsDraw_[currentPlayerMoveHistoryIndex_] = true;
	}

	localCamera_->pos = firstCameraPos_;
	
	currentPlayerMoveHistoryIndex_ = savePlayerMoveHistoryIndex_;
}

float Minimap::GetPreToCurrentLength(const Vector3& playerPos)
{
	return (playerMoveHistory_[savePlayerMoveHistoryIndex_].back() - playerPos).Length();
}

void Minimap::StopAddPlayerPos()
{
	isStop_ = true;
}

void Minimap::CreateTex2D()
{
	minimapBackGround_.reset(new Texture2D{});
	minimapBackGround_->pos.z = 5.0f;
	minimapBackGround_->color = static_cast<uint32_t>(255.0f *0.3f);
	minimapBackGround_->scale.x = miniMapScale.x * 2.0f;
	minimapBackGround_->scale.y = miniMapScale.y * 2.0f;
	minimapBackGround_->rotate.x = std::numbers::pi_v<float> *0.5f;


	// ブロック用テクスチャ初期化
	blockTextures_.resize(blockData_.size());

	const uint32_t kBlockColor = Vector4ToUint(colors_[Type::kBlock]);
	for (size_t i = 0llu; i < blockTextures_.size(); i++) {
		blockTextures_[i].reset(new Texture2D{ texturePathes_[Type::kBlock]});
		blockTextures_[i]->pos = blockData_[i].pos;
		blockTextures_[i]->rotate.x = std::numbers::pi_v<float> *0.5f;
		blockTextures_[i]->rotate.y = blockData_[i].rotate.y;
		blockTextures_[i]->scale.x = blockData_[i].scale.x;
		blockTextures_[i]->scale.y = blockData_[i].scale.z;
		blockTextures_[i]->color = kBlockColor;
	}
	for (auto& i : blockTextures_) {
		i->scale *= objectScale_;
	}

	// プレイヤー用テクスチャ初期化
	playerTex_ = std::make_unique<Texture2D>(texturePathes_[Type::kPlayer]);
	
	playerTex_->pos = playerData_.pos;
	playerTex_->rotate.x = std::numbers::pi_v<float> *0.5f;
	playerTex_->rotate.y = playerData_.rotate.y;
	playerTex_->scale.x = playerData_.scale.x * playerScale_;
	playerTex_->scale.y = playerData_.scale.z * playerScale_;
	playerTex_->color = Vector4ToUint(colors_[Type::kPlayer]);

	// ゴール用テクスチャ初期化
	goalTex_ = std::make_unique<Texture2D>(texturePathes_[Type::kGoal]);

	goalTex_->pos = goalData_.pos;
	goalTex_->rotate.x = std::numbers::pi_v<float> *0.5f;
	goalTex_->rotate.y = goalData_.rotate.y;
	goalTex_->scale.x = goalData_.scale.x;
	goalTex_->scale.y = goalData_.scale.z;
	goalTex_->color = Vector4ToUint(colors_[Type::kGoal]);


	rightBeachTex_.reset(new Texture2D{});
	leftBeachTex_.reset(new Texture2D{});
	rightBeachTex_->color = Vector4ToUint(colors_[Type::kBeach]);
	leftBeachTex_->color = Vector4ToUint(colors_[Type::kBeach]);

	rightBeachTex_->pos.x += goalTex_->scale.x * 0.5f;
	leftBeachTex_->pos.x -= goalTex_->scale.x * 0.5f;
	rightBeachTex_->scale = Vector2{ 6.22f, 2.5f } *objectScale_;
	leftBeachTex_->scale = Vector2{ 6.22f, 2.5f } *objectScale_;


	rightBeachTex_->pos = rightBeachTex_->pos * Quaternion::MakeRotateYAxis(goalData_.rotate.y) + goalData_.pos;
	leftBeachTex_->pos = leftBeachTex_->pos * Quaternion::MakeRotateYAxis(goalData_.rotate.y) + goalData_.pos;
	rightBeachTex_->rotate.y = goalData_.rotate.y;
	leftBeachTex_->rotate.y = goalData_.rotate.y;
	rightBeachTex_->rotate.x = std::numbers::pi_v<float> * 0.5f;
	leftBeachTex_->rotate.x = std::numbers::pi_v<float> * 0.5f;


	// 渦潮のテクスチャ初期化
	whirpoolTextures_.resize(whirpoolData_.size());
	const uint32_t kWhirpoolColor = Vector4ToUint(colors_[Type::kWhirpool]);

	for (size_t i = 0; i < whirpoolTextures_.size(); i++) {
		whirpoolTextures_[i].reset(new Texture2D{ texturePathes_[Type::kWhirpool] });
		whirpoolTextures_[i]->pos = whirpoolData_[i].pos;
		whirpoolTextures_[i]->rotate.x = std::numbers::pi_v<float> *0.5f;
		whirpoolTextures_[i]->rotate.y = whirpoolData_[i].rotate.y;
		whirpoolTextures_[i]->scale.x = whirpoolData_[i].scale.x;
		whirpoolTextures_[i]->scale.y = whirpoolData_[i].scale.z;
		whirpoolTextures_[i]->color = kWhirpoolColor;
	}

	for (auto& i : whirpoolTextures_) {
		i->scale *= objectScale_;
	}

	// 風のテクスチャ初期化
	windTextures_.resize(windData_.size());
	const uint32_t kWindColor = Vector4ToUint(colors_[Type::kWind]);

	for (size_t i = 0; i < windTextures_.size(); i++) {
		windTextures_[i].reset(new Texture2D{ texturePathes_[Type::kWind] });
		windTextures_[i]->pos = windData_[i].pos;
		windTextures_[i]->rotate.x = std::numbers::pi_v<float> *0.5f;
		windTextures_[i]->rotate.y = windData_[i].rotate.y;
		windTextures_[i]->scale.x = windData_[i].scale.x;
		windTextures_[i]->scale.y = windData_[i].scale.z;
		windTextures_[i]->color = kWindColor;
	}

	for (auto& i : deathPosTextures_) {
		i.reset(new Texture2D{ texturePathes_[Type::kDeathPoint] });
		i->rotate.x = std::numbers::pi_v<float> *0.5f;
		i->scale = playerTex_->scale;
	}

	for (auto& i : windTextures_) {
		i->scale *= objectScale_;
	}
}

void Minimap::LoadBlockData()
{
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
					blockData_.push_back({ scale, rotate, pos });
				}
			}
		}
	}
}

void Minimap::LoadGoalData()
{
	const std::filesystem::path kDirectoryPath = "./Datas/StageDatas/" + StageManager::GetCurrentStageString() + "/Goals/Goals";
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
	nlohmann::json::iterator itGroup = root.find("Goals");
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
				if (objectName.find("Goal") != std::string::npos) {
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
					goalData_ = { scale, rotate, pos };
				}
			}
		}
	}
}

void Minimap::LoadWhirlpool()
{
	const std::filesystem::path kDirectoryPath = "./Datas/StageDatas/" + StageManager::GetCurrentStageString() + "/Whirlpool/Whirlpool";
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
	nlohmann::json::iterator itGroup = root.find("Whirlpool");
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
				if (objectName.find("Whirlpool") != std::string::npos) {
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
					whirpoolData_.push_back({ scale, rotate, pos });
				}
			}
		}
	}
}

void Minimap::LoadWind()
{
	const std::filesystem::path kDirectoryPath = "./Datas/StageDatas/" + StageManager::GetCurrentStageString() + "/Wind/Wind";
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
	nlohmann::json::iterator itGroup = root.find("Wind");
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
				if (objectName.find("Wind") != std::string::npos) {
					Vector3 pos{}, rotate{}, scale{}, vector{};
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
							else if (itemNameObject == "vector") {
								vector = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
						}
					}
					rotate.y = atan2(-vector.z, vector.x) + std::numbers::pi_v<float> * 0.5f;
					std::swap(scale.x, scale.z);
					windData_.push_back({ scale, rotate, pos });
				}
			}
		}
	}
}

void Minimap::AdjustCameraPos(const Vector3& playerPos, const Vector3& goalPos)
{
	Vector3 max;
	Vector3 min;

	auto maxXElement = std::max_element(
		blockData_.begin(), blockData_.end(),
		[](const ObjectData& a, const ObjectData& b)->bool {
			return a.pos.x < b.pos.x;
		}
	);
	auto maxYElement = std::max_element(
		blockData_.begin(), blockData_.end(),
		[](const ObjectData& a, const ObjectData& b)->bool {
			return a.pos.y < b.pos.y;
		}
	);
	auto maxZElement = std::max_element(
		blockData_.begin(), blockData_.end(),
		[](const ObjectData& a, const ObjectData& b)->bool {
			return a.pos.z < b.pos.z;
		}
	);
	auto minXElement = std::min_element(
		blockData_.begin(), blockData_.end(),
		[](const ObjectData& a, const ObjectData& b)->bool {
			return a.pos.x < b.pos.x;
		}
	);
	auto minYElement = std::min_element(
		blockData_.begin(), blockData_.end(),
		[](const ObjectData& a, const ObjectData& b)->bool {
			return a.pos.y < b.pos.y;
		}
	);
	auto minZElement = std::min_element(
		blockData_.begin(), blockData_.end(),
		[](const ObjectData& a, const ObjectData& b)->bool {
			return a.pos.x < b.pos.x;
		}
	);

	max.x = std::max(std::max(maxXElement->pos.x, playerPos.x), goalPos.x);
	max.z = std::max(std::max(maxZElement->pos.z, playerPos.z), goalPos.z);

	min.x =std::min(std::min(minXElement->pos.x, playerPos.x), goalPos.x);
	min.z =std::min(std::min(minZElement->pos.z, playerPos.z), goalPos.z);

	localCamera_->pos = min + ((max - min) * 0.5f);

	max.y = std::max(std::max(maxYElement->pos.y, playerPos.y), goalPos.y);
	min.y = std::min(std::min(minYElement->pos.y, playerPos.y), goalPos.y);

	if (65.0f <= std::abs(max.x - min.x)) {
		localCamera_->pos.y = 200.0f * std::abs(max.x -min.x) / 65.0f;
	}
	else {
		localCamera_->pos.y = 130.0f * (max - min).Length() / 60.0f;
	}

	const float kEpsilon = 2.0f;

	localCamera_->pos.x += kEpsilon;
}

void Minimap::CameraMove()
{
	Gamepad* const gamePad = Gamepad::GetInstance();
	Vector2 rightStick = {
		gamePad->GetStick(Gamepad::Stick::RIGHT_X),
		gamePad->GetStick(Gamepad::Stick::RIGHT_Y)
	};

	rightStick = rightStick.Normalize() * cameraMoveSpeed_;

	localCamera_->pos.x += rightStick.x;
	localCamera_->pos.z += rightStick.y;
}
