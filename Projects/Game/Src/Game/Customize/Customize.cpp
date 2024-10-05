#include "Customize.h"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <numbers>

#include "imgui.h"
#include "json.hpp"

#include "Input/Input.h"
#include "Camera/Camera.h"
#include "Scenes/Manager/SceneManager.h"
#include "Game/CameraManager/CameraManager.h"
#include "Game/StageManager/StageManager.h"

#include "Utils/FileUtils.h"
Customize::Customize() {}

void Customize::Initialize() {
	maxInput_ = 15;
	isInput_ = true;

	isTriggerInput_ = true;
	triggerInputCount_ = 0;
	triggerMaxInput_ = 10;

	decisionSize_ = 1.5f;

	angle_ = 0.0f;
	currentType_ = Type::kCursor;
	cursor_ = Input::GetInstance()->GetMouse()->GetPos();

	cursorBone_ = std::make_unique<ModelInstance>();
	cursorSail_ = std::make_unique<ModelInstance>();
	cursorCannon_ = std::make_unique<ModelInstance>();
	cursorLoopCannon_ = std::make_unique<ModelInstance>();

	cursorBone_->SetParent(player_->GetBody());
	cursorBone_->Load("./Resources/Player/bone.obj");
	cursorBone_->SetParent(player_->GetBody());
	cursorSail_->Load("./Resources/Player/sail.obj");
	cursorSail_->SetParent(player_->GetBody());
	cursorCannon_->Load("./Resources/Player/cannon.obj");
	cursorCannon_->SetParent(player_->GetBody());
	cursorLoopCannon_->Load("./Resources/Player/loopCannon.obj");
	cursorLoopCannon_->SetParent(player_->GetBody());

	customizeBackGroundTexture_ = std::make_unique<Tex2DInstance>();
	customizeBackGroundTexture_->Load("./Resources/InGame/customizeBackGround.png");
	customizeBackGroundTexture_->scale = { 1280.0f,720.0f };

	customizeBaseTexture_ = std::make_unique<Tex2DInstance>();
	cursorTexture_ = std::make_unique<Tex2DInstance>();
	cursorSailTexture_ = std::make_unique<Tex2DInstance>();
	cursorCannonTexture_ = std::make_unique<Tex2DInstance>();
	cursorLoopCannonTexture_ = std::make_unique<Tex2DInstance>();

	customizeBaseTexture_->Load("./Resources/InGame/customizeBase.png");
	cursorTexture_->Load("./Resources/InGame/cursor.png");
	cursorSailTexture_->Load("./Resources/InGame/customizeSail.png");
	cursorCannonTexture_->Load("./Resources/InGame/customizeCannon.png");
	cursorLoopCannonTexture_->Load("./Resources/InGame/customizeLoopCannon.png");
	cursorSailTexture_->pos.z = 0.0f;
	cursorSailTexture_->scale = { player_->GetTileMapSize() * 3.0f,player_->GetTileMapSize() };
	cursorCannonTexture_->pos.z = 0.0f;
	cursorCannonTexture_->scale = { player_->GetTileMapSize() ,player_->GetTileMapSize() * 2.0f };
	cursorLoopCannonTexture_->pos.z = 0.0f;
	cursorLoopCannonTexture_->scale = { player_->GetTileMapSize() ,player_->GetTileMapSize() * 2.0f };

	cursorTexture_->scale = { player_->GetTileMapSize() ,player_->GetTileMapSize() };
	customizeBaseTexture_->scale = { 1280.0f,720.0f };
	tileMapOffset_ = { -390.0f,-84.0f };
	fileName_ = "Object";

	setColor_ = { 0.4f, 0.4f, 0.4f, 1.0f };
	setIntervalTextureColor_ = { 0.0f, 0.0f, 0.0f, 0.6f };

	cursorNumbers_ = std::make_unique<Tex2DInstance>();
	cursorNumbers_->Load("./Resources/outLineNumber.png");
	cursorNumbers_->scale = { player_->GetNumberTextureSize() ,player_->GetNumberTextureSize() };
	cursorNumbers_->pos = ChangeCursorToTexturePos();
	interval_ = 1;

	explanationCursorTexture_ = std::make_unique<Tex2DInstance>();
	explanationCannonTexture_ = std::make_unique<Tex2DInstance>();
	explanationLoopCannonTexture_ = std::make_unique<Tex2DInstance>();
	explanationSailTexture_ = std::make_unique<Tex2DInstance>();

	explanationCursorTexture_->Load("./Resources/InGame/explanationCursor.png");
	explanationCannonTexture_->Load("./Resources/InGame/explanationCannon.png");
	explanationLoopCannonTexture_->Load("./Resources/InGame/explanationLoopCannon.png");
	explanationSailTexture_->Load("./Resources/InGame/explanationSail.png");

	explanationTextureSize_ = { 480.0f,160.0f };
	explanationTexturePos_ = { -400.0f,190.0f };

	explanationCursorTexture_->scale = explanationTextureSize_;
	explanationCannonTexture_->scale = explanationTextureSize_;
	explanationLoopCannonTexture_->scale = explanationTextureSize_;
	explanationSailTexture_->scale = explanationTextureSize_;

	explanationCursorTexture_->pos = explanationTexturePos_;
	explanationCannonTexture_->pos = explanationTexturePos_;
	explanationLoopCannonTexture_->pos = explanationTexturePos_;
	explanationSailTexture_->pos = explanationTexturePos_;

	startTexture_ = std::make_unique<Tex2DInstance>();
	startTexture_->Load("./Resources/InGame/start.png");
	startTexture_->scale = { 220.0f,100.0f };
	startTexture_->pos = { 530.0,-320.0f,0.0f };

	gearTexture_ = std::make_unique<Tex2DInstance>();
	gearTexture_->Load("./Resources/InGame/gear.png");
	gearTexture_->scale = { 280.0f,280.0f };
	gearTexture_->pos = { 0.0,365.0f,0.0f };

	maxSailCountTexture_ = std::make_unique<Tex2DInstance>();
	maxCannonCountTexture_ = std::make_unique<Tex2DInstance>();
	maxLoopCannonCountTexture_ = std::make_unique<Tex2DInstance>();
	maxSailCountTexture_->Load("./Resources/outLineNumber.png");
	maxCannonCountTexture_->Load("./Resources/outLineNumber.png");
	maxLoopCannonCountTexture_->Load("./Resources/outLineNumber.png");

	toStageSelectTexture_ = std::make_unique<Tex2DInstance>();
	toStageSelectTexture_->Load("./Resources/OutGame/toStageSelect.png");
	toStageSelectTexture_->scale = { 220.0f,100.0f };
	toStageSelectTexture_->pos = { -530.0,-312.0f,0.0f };

	auto csv = Lamb::LoadCsv("./Datas/CustomizeMapData/" + StageManager::GetCurrentStageString() + "/CustomizeMapData.csv");
	for (int y = 0; y < height_; y++) {
		for (int x = 0; x < width_; x++) {
			if (csv.at(y).at(x) == 1) {
				ModelInstance* model = new ModelInstance();
				model->Load("./Resources/Player/barrel.obj");
				model->SetParent(player_->GetBody());
				model->pos = { (float(x) - int(float(width_) * 0.5f)) * player_->GetObjectDistance().x,0.0f,(float((height_ - 1) - y) - int(float(height_) * 0.5f)) * player_->GetObjectDistance().y };
				player_->AddBarrel(std::move(model));
				player_->SetMapDataType(x, (height_ - 1) - y, Player::Type::kNotPut);
			}
			else {
				player_->SetMapDataType(x, (height_ - 1) - y, Player::Type::kNone);
			}
		}
	}

	maxCountTexturePos_ = { -510.0f ,6.0f };
	maxCountTextureScale_ = { 32.0f ,32.0f };
	maxCountTextureDistance_ = 32.0f;

	setObjectScale_ = 1.2f;

	isLoop_ = false;
	stageIndex_ = StageManager::GetStage();

	saveString_.resize(128);
	saveString_ = StageManager::GetStageString(stageIndex_);

	loadString_.resize(128);
	loadString_ = StageManager::GetStageString(stageIndex_);
	LoadJson(loadString_);

	AudioManager::GetInstance()->Load("./Resources/Sound/set.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/delete.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/count.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/rotate.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/changeItem.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/ready.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/moveTile.wav");

	pushA_ =	 AudioManager::GetInstance()->Get("./Resources/Sound/set.wav");
	pushB_ =	 AudioManager::GetInstance()->Get("./Resources/Sound/delete.wav");
	pushY_ =	 AudioManager::GetInstance()->Get("./Resources/Sound/count.wav");
	pushLBRB_ =  AudioManager::GetInstance()->Get("./Resources/Sound/rotate.wav");
	pushLTRT_ =  AudioManager::GetInstance()->Get("./Resources/Sound/changeItem.wav");
	pushStart_ = AudioManager::GetInstance()->Get("./Resources/Sound/ready.wav");
	moveTile_ =  AudioManager::GetInstance()->Get("./Resources/Sound/moveTile.wav");

	isCustomizeEnd_ = false;
}

void Customize::Update() {

	// クリア
	for (int y = 0; y < height_; y++) {
		for (int x = 0; x < width_; x++) {
			if (player_->GetMapData(x, y).index == -1 && player_->GetMapData(x, y).type != Player::Type::kNotPut) {
				player_->SetMapDataType(x, y, Player::Type::kNone);
			}
		}
	}
	Mouse();
	//GamePad();
	SetObject();
	DeleteObject();
	SetColor();
	cursorTexture_->pos = ChangeCursorToTexturePos();
	Vector3 cursorModelPos = { (ChangeCursorToTileMap().x - int(float(width_ * 0.5f))) * player_->GetObjectDistance().x , 0.0f, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetObjectDistance().y };
	cursorTexture_->scale = { player_->GetTileMapSize() ,player_->GetTileMapSize() };
	cursorBone_->scale.x = std::lerp(cursorBone_->scale.x, 1.0f, player_->GetObjectShrinkTime());
	cursorBone_->scale.y = std::lerp(cursorBone_->scale.y, 1.0f, player_->GetObjectShrinkTime());
	cursorBone_->scale.z = std::lerp(cursorBone_->scale.z, 1.0f, player_->GetObjectShrinkTime());
	cursorBone_->pos = cursorModelPos;
	cursorBone_->rotate.y = angle_ * std::numbers::pi_v<float> / 180.0f;
	cursorCannon_->scale.x = std::lerp(cursorCannon_->scale.x, 1.0f, player_->GetObjectShrinkTime());
	cursorCannon_->scale.y = std::lerp(cursorCannon_->scale.y, 1.0f, player_->GetObjectShrinkTime());
	cursorCannon_->scale.z = std::lerp(cursorCannon_->scale.z, 1.0f, player_->GetObjectShrinkTime());
	cursorCannon_->pos = cursorModelPos;
	cursorCannon_->rotate.y = angle_ * std::numbers::pi_v<float> / 180.0f;
	cursorLoopCannon_->scale.x = std::lerp(cursorLoopCannon_->scale.x, 1.0f, player_->GetObjectShrinkTime());
	cursorLoopCannon_->scale.y = std::lerp(cursorLoopCannon_->scale.y, 1.0f, player_->GetObjectShrinkTime());
	cursorLoopCannon_->scale.z = std::lerp(cursorLoopCannon_->scale.z, 1.0f, player_->GetObjectShrinkTime());
	cursorLoopCannon_->pos = cursorModelPos;
	cursorLoopCannon_->rotate.y = angle_ * std::numbers::pi_v<float> / 180.0f;
	cursorSail_->scale.x = std::lerp(cursorSail_->scale.x, 1.0f, player_->GetObjectShrinkTime());
	cursorSail_->scale.y = std::lerp(cursorSail_->scale.y, 1.0f, player_->GetObjectShrinkTime());
	cursorSail_->scale.z = std::lerp(cursorSail_->scale.z, 1.0f, player_->GetObjectShrinkTime());
	cursorSail_->pos = cursorModelPos;
	cursorSail_->rotate.y = angle_ * std::numbers::pi_v<float> / 180.0f;

	// ギア回転
	gearTexture_->rotate.z += 0.01f;

	maxSailCountTexture_->pos = maxCountTexturePos_;
	maxSailCountTexture_->scale = maxCountTextureScale_;
	maxSailCountTexture_->uvSize = { 0.1f,1.0f };
	maxSailCountTexture_->uvPibot = { (maxSailCount_ - player_->GetSailCount() - 1) * 0.1f,0.0f };

	maxCannonCountTexture_->pos = { maxCountTexturePos_.x ,maxCountTexturePos_.y + maxCountTextureDistance_ };
	maxCannonCountTexture_->scale = maxCountTextureScale_;
	maxCannonCountTexture_->uvSize = { 0.1f,1.0f };
	maxCannonCountTexture_->uvPibot = { (maxCannonCount_ - player_->GetCannonCount() - 1) * 0.1f,0.0f };


	maxLoopCannonCountTexture_->pos = { maxCountTexturePos_.x, maxCountTexturePos_.y - maxCountTextureDistance_ };
	maxLoopCannonCountTexture_->scale = maxCountTextureScale_;
	maxLoopCannonCountTexture_->uvSize = { 0.1f,1.0f };
	maxLoopCannonCountTexture_->uvPibot = { (maxLoopCannonCount_ - player_->GetLoopCannonCount() - 1) * 0.1f,0.0f };

	cursorBone_->Update();
	cursorCannon_->Update();
	cursorLoopCannon_->Update();
	cursorSail_->Update();


	static float kT = 0.2f;

	for (auto& sail : sailInterval_) {

		sail.left->scale = { std::lerp(sail.left->scale.x ,intervalScale_,kT),std::lerp(sail.left->scale.y,intervalScale_,kT) };
		sail.right->scale = { std::lerp(sail.right->scale.x ,intervalScale_,kT),std::lerp(sail.right->scale.y,intervalScale_,kT) };
	}

	for (auto& cannon : cannonInterval_) {

		cannon.left->scale = { std::lerp(cannon.left->scale.x ,intervalScale_,kT),std::lerp(cannon.left->scale.y,intervalScale_,kT) };
		cannon.right->scale = { std::lerp(cannon.right->scale.x ,intervalScale_,kT),std::lerp(cannon.right->scale.y,intervalScale_,kT) };
	}

	for (auto& loopCannon : loopCannonInterval_) {

		loopCannon.left->scale = { std::lerp(loopCannon.left->scale.x ,intervalScale_,kT),std::lerp(loopCannon.left->scale.y,intervalScale_,kT) };
		loopCannon.right->scale = { std::lerp(loopCannon.right->scale.x ,intervalScale_,kT),std::lerp(loopCannon.right->scale.y,intervalScale_,kT) };
	}
#ifdef USE_IMGUI
	Debug();
#endif // USE_IMGUI
}

void Customize::Draw(const Camera& camera) {
	int32_t cursor_x = int32_t(ChangeCursorToTileMap().x);
	int32_t cursor_y = int32_t(ChangeCursorToTileMap().y);
	if ((float(cursor_x) != -1.0f &&
		(float(cursor_y) != -1.0f)) &&
		player_->GetMapData(cursor_x, cursor_y).type == Player::kNone) {
		switch (currentType_) {
		case Customize::kNone:
			break;
		case Customize::kNotPut:
			break;
		case Customize::kSail:
			if (angle_ == 90.0f || angle_ == 270.0f) {
				if (cursor_y - 1.0f >= 0.0f && cursor_y + 1.0f < height_) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x, cursor_y - 1).type == Player::kNone &&
						player_->GetMapData(cursor_x, cursor_y + 1).type == Player::kNone) {
						cursorSail_->Draw(camera.GetViewProjection());
						cursorBone_->Draw(camera.GetViewProjection());
					}
				}
			}
			else {
				if (cursor_x - 1.0f >= 0.0f && cursor_x + 1.0f < width_) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x - 1, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x + 1, cursor_y).type == Player::kNone) {
						cursorSail_->Draw(camera.GetViewProjection());
						cursorBone_->Draw(camera.GetViewProjection());
					}
				}
			}
			break;
		case Customize::kCannon:
			if (angle_ == 0.0f) {
				if (cursor_y - 1.0f >= 0.0f) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x, cursor_y - 1).type == Player::kNone) {
						cursorCannon_->Draw(camera.GetViewProjection());
					}
				}
			}
			else if (angle_ == 90.0f) {
				if (cursor_x - 1.0f >= 0.0f) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x - 1, cursor_y).type == Player::kNone) {
						cursorCannon_->Draw(camera.GetViewProjection());
					}
				}
			}
			else if (angle_ == 180.0f) {
				if (cursor_y + 1.0f < height_) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x, cursor_y + 1).type == Player::kNone) {
						cursorCannon_->Draw(camera.GetViewProjection());
					}
				}
			}
			else {
				if (cursor_x + 1.0f < width_) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::Type::kNone &&
						player_->GetMapData(cursor_x + 1, cursor_y).type == Player::Type::kNone) {
						cursorCannon_->Draw(camera.GetViewProjection());
					}
				}
			}


			break;
		case Customize::kLoopCannon:
			if (angle_ == 0.0f) {
				if (cursor_y - 1.0f >= 0.0f) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x, cursor_y - 1).type == Player::kNone) {
						cursorLoopCannon_->Draw(camera.GetViewProjection());

					}
				}
			}
			else if (angle_ == 90.0f) {
				if (cursor_x - 1.0f >= 0.0f) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x - 1, cursor_y).type == Player::kNone) {
						cursorLoopCannon_->Draw(camera.GetViewProjection());

					}
				}
			}
			else if (angle_ == 180.0f) {
				if (cursor_y + 1.0f < height_) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x, cursor_y + 1).type == Player::kNone) {
						cursorLoopCannon_->Draw(camera.GetViewProjection());

					}
				}
			}
			else {
				if (cursor_x + 1.0f < width_) {
					if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
						player_->GetMapData(cursor_x + 1, cursor_y).type == Player::kNone) {
						cursorLoopCannon_->Draw(camera.GetViewProjection());

					}
				}
			}
			break;
		case Customize::kCursor:
			break;
		case Customize::kTypeCount:
			break;
		default:
			break;
		}
	}

}

void Customize::PostDraw(const Camera& camera) {
	gearTexture_->blend = BlendType::kUnenableDepthNormal;
	gearTexture_->Draw(camera.GetViewOthographics());
	customizeBaseTexture_->pos.z = 2.0f;
	customizeBaseTexture_->blend = BlendType::kUnenableDepthNormal;
	customizeBaseTexture_->Draw(camera.GetViewOthographics());

	for (int index = 0; auto & sail : sailInterval_) {
		if (player_->GetSailTextures(index)->Collision(ChangeCursorToTexturePos())) {
			sail.left->blend = BlendType::kUnenableDepthNormal;
			sail.left->Draw(camera.GetViewOthographics());
			sail.right->blend = BlendType::kUnenableDepthNormal;
			sail.right->Draw(camera.GetViewOthographics());
		}
		index++;
	}
	for (int index = 0; auto & cannon : cannonInterval_) {
		if (player_->GetCannonsTextures(index)->Collision(ChangeCursorToTexturePos())) {
			cannon.left->blend = BlendType::kUnenableDepthNormal;
			cannon.left->Draw(camera.GetViewOthographics());
			cannon.right->blend = BlendType::kUnenableDepthNormal;
			cannon.right->Draw(camera.GetViewOthographics());
		}
		index++;
	}
	for (int index = 0; auto & loopCannon : loopCannonInterval_) {
		if (player_->GetLoopCannonsTextures(index)->Collision(ChangeCursorToTexturePos())) {
			loopCannon.left->blend = BlendType::kUnenableDepthNormal;
			loopCannon.left->Draw(camera.GetViewOthographics());
			loopCannon.right->blend = BlendType::kUnenableDepthNormal;
			loopCannon.right->Draw(camera.GetViewOthographics());
		}
		index++;
	}
	if (player_->GetIsCustomize()) {
		switch (currentType_) {
		case Customize::kNone:
			break;
		case Customize::kSail:
			explanationSailTexture_->blend = BlendType::kUnenableDepthNormal;
			explanationSailTexture_->Draw(camera.GetViewOthographics());
			if ((ChangeCursorToTileMap().x != -1.0f ||
				ChangeCursorToTileMap().y != -1.0f)) {
				cursorSailTexture_->blend = BlendType::kUnenableDepthNormal;
				cursorSailTexture_->Draw(camera.GetViewOthographics());
				//cursorNumbers_->pos.z = 0.0f;
				//cursorNumbers_->Draw(camera.GetViewOthographics());
			}
			break;
		case Customize::kCannon:
			explanationCannonTexture_->blend = BlendType::kUnenableDepthNormal;
			explanationCannonTexture_->Draw(camera.GetViewOthographics());

			if ((ChangeCursorToTileMap().x != -1.0f ||
				ChangeCursorToTileMap().y != -1.0f)) {
				cursorCannonTexture_->blend = BlendType::kUnenableDepthNormal;
				cursorCannonTexture_->Draw(camera.GetViewOthographics());
				//cursorNumbers_->pos.z = 0.0f;
				//cursorNumbers_->Draw(camera.GetViewOthographics());
			}
			break;
		case Customize::kLoopCannon:
			explanationLoopCannonTexture_->blend = BlendType::kUnenableDepthNormal;
			explanationLoopCannonTexture_->Draw(camera.GetViewOthographics());

			if ((ChangeCursorToTileMap().x != -1.0f ||
				ChangeCursorToTileMap().y != -1.0f)) {
				cursorLoopCannonTexture_->blend = BlendType::kUnenableDepthNormal;
				cursorLoopCannonTexture_->Draw(camera.GetViewOthographics());
				//cursorNumbers_->pos.z = 0.0f;
				//cursorNumbers_->Draw(camera.GetViewOthographics());
			}
			break;
		case Customize::kCursor:
			explanationCursorTexture_->blend = BlendType::kUnenableDepthNormal;;
			explanationCursorTexture_->Draw(camera.GetViewOthographics());
			break;
		case Customize::kTypeCount:
			break;
		default:
			break;
		}
	}

	//cursorTexture_->pos.z = 0.0f;
	//cursorTexture_->Draw(camera.GetViewOthographics());

	startTexture_->blend = BlendType::kUnenableDepthNormal;
	startTexture_->Draw(camera.GetViewOthographics());
	toStageSelectTexture_->blend = BlendType::kUnenableDepthNormal;
	toStageSelectTexture_->Draw(camera.GetViewOthographics());
	maxSailCountTexture_->blend = BlendType::kUnenableDepthNormal;
	maxSailCountTexture_->Draw(camera.GetViewOthographics());
	maxCannonCountTexture_->blend = BlendType::kUnenableDepthNormal;
	maxCannonCountTexture_->Draw(camera.GetViewOthographics());
	maxLoopCannonCountTexture_->blend = BlendType::kUnenableDepthNormal;
	maxLoopCannonCountTexture_->Draw(camera.GetViewOthographics());
}

void Customize::OffCustomize() {
	// カスタマイズオフ
	player_->SetSailTexturesColor(Vector4::kIdentity);
	player_->SetCannonTexturesColor(Vector4::kIdentity);
	player_->SetLoopCannonTexturesColor(Vector4::kIdentity);

}

void Customize::Reset() {
	currentType_ = Type::kCursor;
	cursorBone_->scale = Vector3::kIdentity;
	cursorCannon_->scale = Vector3::kIdentity;
	cursorLoopCannon_->scale = Vector3::kIdentity;
	cursorSail_->scale = Vector3::kIdentity;
	angle_ = 0.0f;
	cursor_ = Input::GetInstance()->GetMouse()->GetPos();
	isCustomizeEnd_ = false;
}

void Customize::Debug() {
	ImGui::Begin("Customize");
	if (ImGui::TreeNode("Input")) {
		ImGui::DragInt("InputTime", &maxInput_);
		ImGui::DragInt("InputCount", &inputCount_);
		ImGui::DragFloat4("setColor", &setColor_.color.r, 0.05f);
		ImGui::Text("isInput:%d", isInput_);
		ImGui::DragFloat3("tileMapOffset", &tileMapOffset_.x, 1.0f);
		ImGui::TreePop();
	}
	ImGui::Text("CursorPos x:%f,y:%f", ChangeCursorToTileMap().x, ChangeCursorToTileMap().y);
	ImGui::Text("cursorCannonTexture_ x:%f,y:%f", cursorCannonTexture_->pos.x, cursorCannonTexture_->pos.y);
	ImGui::DragFloat("intervalScale", &intervalScale_);
	ImGui::DragFloat("intervalTextureDistance", &intervalTextureDistance_);
	ImGui::DragFloat("Angle", &angle_);
	ImGui::DragFloat("setObjectScale", &setObjectScale_, 0.1f, 1.0f);
	ImGui::Checkbox("isLoop", &isLoop_);
	ImGui::Text("Count:%d", interval_);
	cursorNumbers_->scale = { player_->GetNumberTextureSize() ,player_->GetNumberTextureSize() };
	ImGui::DragFloat2("explanationTexturePos_", &explanationTexturePos_.x);
	explanationCursorTexture_->pos = explanationTexturePos_;
	explanationCannonTexture_->pos = explanationTexturePos_;
	explanationLoopCannonTexture_->pos = explanationTexturePos_;
	explanationSailTexture_->pos = explanationTexturePos_;
	ImGui::DragFloat3("startTexture_", &startTexture_->pos.x);
	ImGui::DragFloat3("toStageSelectTexturescale", &toStageSelectTexture_->scale.x);
	ImGui::DragFloat3("toStageSelectTexturepos", &toStageSelectTexture_->pos.x);
	ImGui::DragFloat3("gearTexture_", &gearTexture_->pos.x);
	if (ImGui::Button("LoadCustomizeMapSCVLoad")) {
		auto csv = Lamb::LoadCsv("./Datas/CustomizeMapData/" + StageManager::GetCurrentStageString() + "/CustomizeMapData.csv");
		for (int y = 0; y < height_; y++) {
			for (int x = 0; x < width_; x++) {
				if (csv.at(y).at(x) == 1) {
					player_->SetMapDataType(x, (height_ - 1) - y, Player::Type::kNotPut);
				}
				else {
					player_->SetMapDataType(x, (height_ - 1) - y, Player::Type::kNone);
				}
			}
		}
	}
	ImGui::DragFloat2("maxCountTexturePos", &maxCountTexturePos_.x, 0.1f);
	ImGui::DragFloat2("maxCountTextureScale", &maxCountTextureScale_.x, 0.1f);
	ImGui::DragFloat("maxCountTextureDistance", &maxCountTextureDistance_, 0.1f);
	if (ImGui::TreeNode("MaxObject")) {
		ImGui::DragInt("MaxSailCount", &maxSailCount_, 1, 0);
		ImGui::DragInt("MaxCannonCount", &maxCannonCount_, 1, 0);
		ImGui::DragInt("MaxLoopCannonCount", &maxLoopCannonCount_, 1, 0);
		if (ImGui::TreeNode("SaveFile")) {
			std::list<std::string> stageList;
			for (uint32_t i = 0; i < StageManager::kMaxStageCount; i++) {
				stageList.emplace_back(StageManager::GetStageString(i));
			}

			// std::vector に変換する
			std::vector<const char*> stageArray;
			for (const auto& stage : stageList) {
				stageArray.push_back(stage.c_str());
			}
			saveString_ = stageArray[stageIndex_];
			// Combo を使用する
			if (ImGui::Combo("Stage", &stageIndex_, stageArray.data(), static_cast<int>(stageArray.size()))) {
				saveString_ = stageArray[stageIndex_];
			}
			if (ImGui::Button("Save") && !saveString_.empty()) {
				saveString_ = stageArray[stageIndex_];
				SaveJson(saveString_);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("LoadFile")) {
			std::list<std::string> stageList;
			for (uint32_t i = 0; i < StageManager::kMaxStageCount; i++) {
				stageList.emplace_back(StageManager::GetStageString(i));
			}

			// std::vector に変換する
			std::vector<const char*> stageArray;
			for (const auto& stage : stageList) {
				stageArray.push_back(stage.c_str());
			}

			loadString_ = stageArray[stageIndex_];
			// Combo を使用する
			if (ImGui::Combo("Stage", &stageIndex_, stageArray.data(), static_cast<int>(stageArray.size()))) {
				loadString_ = stageArray[stageIndex_];
			}
			if (ImGui::Button("Load") && !loadString_.empty()) {
				loadString_ = stageArray[stageIndex_];
				LoadJson(loadString_);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	ImGui::End();
	Input::GetInstance()->GetMouse()->Debug();
}

Vector2 Customize::ChangeCursorToTileMap() {
	Vector2 cursor{};
	auto mousePos = Input::GetInstance()->GetMouse()->GetPos();
	Vector2 tileMapOffset{};
	tileMapOffset.x = player_->GetTileMapOffset().x + 1280.0f * 0.5f;
	tileMapOffset.y = -player_->GetTileMapOffset().y + 720.0f * 0.5f;

	cursor.x = (mousePos.x - tileMapOffset.x) / player_->GetTileMapSize() + float(width_) * 0.5f;
	cursor.y = (mousePos.y - tileMapOffset.y) / player_->GetTileMapSize() * -1.0f + float(height_) * 0.5f;

	if (cursor.x >= float(width_) ||
		cursor.x < 0.0f ||
		cursor.y < 0.0f ||
		cursor.y >= float(height_)) {
		cursor = { -1.0f,-1.0f };
	}
	return Vector2(std::floor(cursor.x), std::floor(cursor.y));
}

Vector2 Customize::ChangeCursorToTexturePos() {
	return Vector2((cursor_.x - 1280.0f * 0.5f), cursor_.y * -1.0f + 720.0f * 0.5f);
}

void Customize::Mouse() {
	auto mouse = Input::GetInstance()->GetMouse();


	cursor_ = mouse->GetPos();

#pragma region カスタマイズ終了
	if (mouse->Pushed(Mouse::Button::Left) &&
		startTexture_->Collision(ChangeCursorToTexturePos())) {
		isCustomizeEnd_ = true;
	}
	// カスタマイズ画面でバック押したとき
	if (mouse->Pushed(Mouse::Button::Left) &&
		toStageSelectTexture_->Collision(ChangeCursorToTexturePos())) {
		SceneManager::GetInstance()->SceneChange(BaseScene::ID::StageSelect);
	}
#pragma endregion


	// カーソル変更
	if (mouse->GetWheelVelocity() < 0) {
		switch (currentType_) {
		case Customize::kNone:
			break;
		case Customize::kNotPut:
			break;
		case Customize::kSail:
		{
			if (StageManager::GetStage() >= StageManager::kCanLoopCannonStageCount &&
				player_->GetLoopCannonCount() < maxLoopCannonCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kLoopCannon;
			}
			else {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kCursor;
			}
		}
		break;
		case Customize::kCannon:
		{
			if (StageManager::GetStage() >= StageManager::kCanSailStageCount &&
				player_->GetSailCount() < maxSailCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kSail;
			}
			else if (StageManager::GetStage() >= StageManager::kCanLoopCannonStageCount &&
				player_->GetLoopCannonCount() < maxLoopCannonCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kLoopCannon;
			}
			else {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kCursor;
			}
		}
		break;
		case Customize::kLoopCannon:
		{
			pushLTRT_->Start(seVolume_, false);
			currentType_ = Type::kCursor;
		}
		break;
		case Customize::kCursor:
		{
			if (player_->GetCannonCount() < maxCannonCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kCannon;
			}
			else if (player_->GetSailCount() < maxSailCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kSail;
			}
			else if (player_->GetLoopCannonCount() < maxLoopCannonCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kLoopCannon;
			}
		}
		break;
		case Customize::kTypeCount:
			break;
		default:
			break;
		}
	}
	else if (mouse->GetWheelVelocity() > 0) {
		switch (currentType_) {
		case Customize::kNone:
			break;
		case Customize::kNotPut:
			break;
		case Customize::kSail:
		{
			if (player_->GetCannonCount() < maxCannonCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kCannon;
			}
			else {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kCursor;
			}
		}
		break;
		case Customize::kCannon:
		{
			pushLTRT_->Start(seVolume_, false);
			currentType_ = Type::kCursor;
		}
		break;
		case Customize::kLoopCannon:
		{
			if (StageManager::GetStage() >= StageManager::kCanSailStageCount &&
				player_->GetSailCount() < maxSailCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kSail;
			}
			else if (player_->GetCannonCount() < maxCannonCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kCannon;
			}
			else {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kCursor;
			}
		}
		break;
		case Customize::kCursor:
		{
			if (StageManager::GetStage() >= StageManager::kCanLoopCannonStageCount &&
				player_->GetLoopCannonCount() < maxLoopCannonCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kLoopCannon;
			}
			else if (StageManager::GetStage() >= StageManager::kCanSailStageCount &&
				player_->GetSailCount() < maxSailCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kSail;
			}
			else if (player_->GetCannonCount() < maxCannonCount_ - 1) {
				pushLTRT_->Start(seVolume_, false);
				currentType_ = Type::kCannon;
			}
		}
		break;
		case Customize::kTypeCount:
			break;
		default:
			break;
		}
	}
	/// 回転
	if (mouse->Pushed(Mouse::Button::Right)) {
		if (currentType_ != Type::kCursor) {
			pushLBRB_->Start(seVolume_, false);
		}
		// 大砲だけ回転逆
		if (currentType_ == Type::kCannon || currentType_ == Type::kLoopCannon) {
			angle_ += 90.0f;
		}
		else {
			if (angle_ == 0.0f) {
				angle_ = 270.0f;
			}
			else {
				angle_ -= 90.0f;
			}
		}
		angle_ = std::fmod(angle_, 360.0f);
	}
	// 消去

}

void Customize::SetObject() {
	auto gamepad = Input::GetInstance()->GetGamepad();
	auto mouse = Input::GetInstance()->GetMouse();
	int32_t cursor_x = int32_t(ChangeCursorToTileMap().x);
	int32_t cursor_y = int32_t(ChangeCursorToTileMap().y);

	if ((gamepad->Pushed(Gamepad::Button::A) || mouse->Pushed(Mouse::Button::Left)) &&
		(float(cursor_x) != -1.0f ||
			float(cursor_y) != -1.0f)) {
		switch (currentType_) {
		case Customize::kCursor:
		{
			int32_t index = player_->GetMapData(cursor_x, cursor_y).index;
			if (!IntervalUpdate(index)) {
				// 帆
				if (player_->GetMapData(cursor_x, cursor_y).type == Player::Type::kSail) {
					pushA_->Start(seVolume_, false);
					// 帆かつ選択されたインデックスを指定して削除
					if (index >= 0 && index <= player_->GetSailCount()) {
						currentType_ = Type::kSail;
						interval_ = player_->GetSailInterval(index);
						angle_ = player_->GetSailTextures(index)->rotate.z * 180.0f / std::numbers::pi_v<float>;
						cursor_.x = (player_->GetSailTextures(index)->pos.x - player_->GetTileMapOffset().x) / player_->GetTileMapSize() + int(float(width_) * 0.5f);
						cursor_.y = (player_->GetSailTextures(index)->pos.y - player_->GetTileMapOffset().y) / player_->GetTileMapSize() + int(float(height_) * 0.5f);

						player_->DeleteSails(index);
						player_->DeleteSailTextures(index);
						player_->DeleteBone(index);
						player_->DeleteSailsIntervals(index);
						player_->DeleteSailIntervalTextures(index);
						player_->DeleteSailNumbers(index);
						player_->SetSailCount(-1);

						// sailIntarval
						DeleteSailIntervalTexture(index);

						// 	帆かつ選択されたインデックスをして初期化
						for (int y = 0; y < height_; y++) {
							for (int x = 0; x < width_; x++) {
								if (player_->GetMapData(x, y).type == Player::kSail &&
									player_->GetMapData(x, y).index == index) {
									player_->SetMapDataType(x, y, Player::kNone);
									player_->SetMapDataIndex(x, y, -1);
								}
								else if (player_->GetMapData(x, y).type == Player::kSail &&
									player_->GetMapData(x, y).index >= index) {
									int mapDataIndex = player_->GetMapData(x, y).index;
									player_->SetMapDataIndex(x, y, mapDataIndex - 1);
								}
							}
						}
					}
				}
				// 大砲
				if (player_->GetMapData(cursor_x, cursor_y).type == Player::kCannon) {
					pushA_->Start(seVolume_, false);
					// 帆かつ選択されたインデックスを指定して削除
					if (index >= 0 && index <= player_->GetCannonCount()) {
						currentType_ = Type::kCannon;
						interval_ = player_->GetCannonInterval(index);
						angle_ = player_->GetCannonsTextures(index)->rotate.z * 180.0f / std::numbers::pi_v<float>;
						cursor_.x = (player_->GetCannonsTextures(index)->pos.x - player_->GetTileMapOffset().x) / player_->GetTileMapSize() + int(float(width_) * 0.5f);
						cursor_.y = (player_->GetCannonsTextures(index)->pos.y - player_->GetTileMapOffset().y) / player_->GetTileMapSize() + int(float(height_) * 0.5f);
						if (angle_ == 0.0f) {
							cursor_.y += 0.5f;
						}
						else if (angle_ == 90.0f) {
							cursor_.x += 0.5f;
						}
						else if (angle_ == 180.0f) {
							cursor_.y -= 0.5f;
						}
						else if (angle_ == 270.0f) {
							cursor_.x -= 0.5f;
						}

						player_->DeleteCannons(index);
						player_->DeleteCannonsTextures(index);
						player_->DeleteCannonsIntervals(index);
						player_->DeleteCannonIntervalTextures(index);
						player_->DeleteCannonsNumbers(index);
						player_->SetCannonCount(-1);

						// cannonInvar
						DeleteCannonIntervalTexture(index);

						// 	帆かつ選択されたインデックスをして初期化
						for (int y = 0; y < height_; y++) {
							for (int x = 0; x < width_; x++) {
								if (player_->GetMapData(x, y).type == Player::kCannon &&
									player_->GetMapData(x, y).index == index) {
									player_->SetMapDataType(x, y, Player::kNone);
									player_->SetMapDataIndex(x, y, -1);
								}
								else if (player_->GetMapData(x, y).type == Player::kCannon &&
									player_->GetMapData(x, y).index >= index) {
									int mapDataIndex = player_->GetMapData(x, y).index;
									player_->SetMapDataIndex(x, y, mapDataIndex - 1);
								}
							}
						}
					}
				}
				// ループ大砲
				if (player_->GetMapData(cursor_x, cursor_y).type == Player::kLoopCannon) {
					pushA_->Start(seVolume_, false);
					// 帆かつ選択されたインデックスを指定して削除
					if (index >= 0 && index <= player_->GetLoopCannonCount()) {
						currentType_ = Type::kLoopCannon;
						interval_ = player_->GetLoopCannonInterval(index);
						angle_ = player_->GetLoopCannonsTextures(index)->rotate.z * 180.0f / std::numbers::pi_v<float>;
						cursor_.x = (player_->GetLoopCannonsTextures(index)->pos.x - player_->GetTileMapOffset().x) / player_->GetTileMapSize() + int(float(width_) * 0.5f);
						cursor_.y = (player_->GetLoopCannonsTextures(index)->pos.y - player_->GetTileMapOffset().y) / player_->GetTileMapSize() + int(float(height_) * 0.5f);
						if (angle_ == 0.0f) {
							cursor_.y += 0.5f;
						}
						else if (angle_ == 90.0f) {
							cursor_.x += 0.5f;
						}
						else if (angle_ == 180.0f) {
							cursor_.y -= 0.5f;
						}
						else if (angle_ == 270.0f) {
							cursor_.x -= 0.5f;
						}
						player_->DeleteLoopCannons(index);
						player_->DeleteLoopCannonsTextures(index);
						player_->DeleteLoopCannonsIntervals(index);
						player_->DeleteLoopCannonIntervalTextures(index);
						player_->DeleteLoopCannonsNumbers(index);
						player_->SetLoopCannonCount(-1);

						// loopCannonInterval
						DeleteLoopCannonIntervalTexture(index);

						// 	帆かつ選択されたインデックスをして初期化
						for (int y = 0; y < height_; y++) {
							for (int x = 0; x < width_; x++) {
								if (player_->GetMapData(x, y).type == Player::kLoopCannon &&
									player_->GetMapData(x, y).index == index) {
									player_->SetMapDataType(x, y, Player::kNone);
									player_->SetMapDataIndex(x, y, -1);
								}
								else if (player_->GetMapData(x, y).type == Player::kLoopCannon &&
									player_->GetMapData(x, y).index >= index) {
									int mapDataIndex = player_->GetMapData(x, y).index;
									player_->SetMapDataIndex(x, y, mapDataIndex - 1);
								}
							}
						}
					}
				}
			}
		}
		break;
		case Customize::kSail:
		{
			//// 帆
			//if (player_->GetMapData(cursor_x, cursor_y).type == Player::Type::kSail) {
			//	int32_t index = player_->GetMapData(cursor_x, cursor_y).index;
			//	// 帆かつ選択されたインデックスを指定して削除
			//	if (index >= 0 && index <= player_->GetSailCount()) {
			//		currentType_ = Type::kSail;
			//		interval_ = player_->GetSailInterval(index);
			//		angle_ = player_->GetSailTextures(index)->rotate.z * 180.0f / std::numbers::pi_v<float>;
			//		cursor_.x = (player_->GetSailTextures(index)->pos.x - player_->GetTileMapOffset().x) / player_->GetTileMapSize() + int(float(width_) * 0.5f);
			//		cursor_.y = (player_->GetSailTextures(index)->pos.y - player_->GetTileMapOffset().y) / player_->GetTileMapSize() + int(float(height_) * 0.5f);
			//		player_->DeleteSails(index);
			//		player_->DeleteSailTextures(index);
			//		player_->DeleteBone(index);
			//		player_->DeleteSailsIntervals(index);
			//		player_->DeleteSailIntervalTextures(index);
			//		player_->DeleteSailNumbers(index);
			//		player_->SetSailCount(-1);

			//		// SailInterval
			//		DeleteSailIntervalTexture(index);

			//		// 	帆かつ選択されたインデックスをして初期化
			//		for (int y = 0; y < height_; y++) {
			//			for (int x = 0; x < width_; x++) {
			//				if (player_->GetMapData(x, y).type == Player::kSail &&
			//					player_->GetMapData(x, y).index == index) {
			//					player_->SetMapDataType(x, y, Player::kNone);
			//					player_->SetMapDataIndex(x, y, -1);
			//				}
			//				else if (player_->GetMapData(x, y).type == Player::kSail &&
			//					player_->GetMapData(x, y).index >= index) {
			//					int mapDataIndex = player_->GetMapData(x, y).index;
			//					player_->SetMapDataIndex(x, y, mapDataIndex - 1);
			//				}
			//			}
			//		}
			//	}
			//}
			//else 
			if (player_->GetSailCount() < maxSailCount_ - 1) {
				if (angle_ == 90.0f || angle_ == 270.0f) {
					if (cursor_y - 1.0f >= 0.0f && cursor_y + 1.0f < height_) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x, cursor_y - 1).type == Player::kNone &&
							player_->GetMapData(cursor_x, cursor_y + 1).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetSail();
							SetMapData(0, 1, Player::kSail);
						}
					}
				}
				else {
					if (cursor_x - 1.0f >= 0.0f && cursor_x + 1.0f < width_) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x - 1, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x + 1, cursor_y).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetSail();
							SetMapData(1, 0, Player::kSail);
						}
					}
				}
			}
			if (player_->GetSailCount() >= maxSailCount_ - 1) {
				currentType_ = Type::kCursor;

			}
		}
		break;
		case Customize::kCannon:
			if (player_->GetCannonCount() < maxCannonCount_ - 1) {
				if (angle_ == 0.0f) {
					if (cursor_y - 1.0f >= 0.0f) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x, cursor_y - 1).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetCannon(0, 0.5f);
							SetMapData(0, -1, Player::kCannon);
						}
					}
				}
				else if (angle_ == 90.0f) {
					if (cursor_x - 1.0f >= 0.0f) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x - 1, cursor_y).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetCannon(-0.5f, 0.0f);
							SetMapData(-1, 0, Player::kCannon);
						}
					}
				}
				else if (angle_ == 180.0f) {
					if (cursor_y + 1.0f < height_) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x, cursor_y + 1).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetCannon(0.0f, -0.5f);
							SetMapData(0, +1, Player::kCannon);
						}
					}
				}
				else {
					if (cursor_x + 1.0f < width_) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x + 1, cursor_y).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetCannon(0.5f, 0.0f);
							SetMapData(+1, 0, Player::kCannon);
						}
					}
				}
			}
			if (player_->GetCannonCount() >= maxCannonCount_ - 1) {
				currentType_ = Type::kCursor;
			}
			break;
		case Customize::kLoopCannon:

		{// ループ大砲
			if (player_->GetLoopCannonCount() < maxLoopCannonCount_ - 1) {
				if (angle_ == 0.0f) {
					if (cursor_y - 1.0f >= 0.0f) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x, cursor_y - 1).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetLoopCannon(0, 0.5f);
							SetMapData(0, -1, Player::kLoopCannon);
						}
					}
				}
				else if (angle_ == 90.0f) {
					if (cursor_x - 1.0f >= 0.0f) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x - 1, cursor_y).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetLoopCannon(-0.5f, 0.0f);
							SetMapData(-1, 0, Player::kLoopCannon);
						}
					}
				}
				else if (angle_ == 180.0f) {
					if (cursor_y + 1.0f < height_) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x, cursor_y + 1).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetLoopCannon(0.0f, -0.5f);
							SetMapData(0, +1, Player::kLoopCannon);
						}
					}
				}
				else {
					if (cursor_x + 1.0f < width_) {
						if (player_->GetMapData(cursor_x, cursor_y).type == Player::kNone &&
							player_->GetMapData(cursor_x + 1, cursor_y).type == Player::kNone) {
							pushA_->Start(seVolume_, false);
							SetLoopCannon(0.5f, 0.0f);
							SetMapData(+1, 0, Player::kLoopCannon);
						}
					}
				}
			}
			if (player_->GetLoopCannonCount() >= maxLoopCannonCount_ - 1) {
				currentType_ = Type::kCursor;
			}
		}
		break;
		case Customize::kTypeCount:
			break;
		default:
			break;
		}
	}
}

void Customize::SetSail() {
	player_->SetSailCount(1);
	player_->AddSailFlag();

	cursorSail_->scale = { setObjectScale_,setObjectScale_ ,setObjectScale_ };

	ModelInstance* sail = new ModelInstance();
	sail->Load("./Resources/Player/sail.obj");
	sail->SetParent(player_->GetBody());
	sail->rotate.y = angle_ * std::numbers::pi_v<float> / 180.0f;
	sail->scale.x = setObjectScale_;
	sail->scale.y = setObjectScale_;
	sail->scale.z = setObjectScale_;
	sail->pos.x = (ChangeCursorToTileMap().x - (int(float(width_) * 0.5f))) * player_->GetObjectDistance().x;
	sail->pos.z = (ChangeCursorToTileMap().y - (int(float(height_) * 0.5f))) * player_->GetObjectDistance().y;
	player_->AddSails(sail);

	ModelInstance* bone = new ModelInstance();
	bone->Load("./Resources/Player/bone.obj");
	bone->SetParent(player_->GetBody());
	bone->rotate.y = angle_ * std::numbers::pi_v<float> / 180.0f;
	bone->scale.x = setObjectScale_;
	bone->scale.y = setObjectScale_;
	bone->scale.z = setObjectScale_;
	bone->pos.x = (ChangeCursorToTileMap().x - (int(float(width_) * 0.5f))) * player_->GetObjectDistance().x;
	bone->pos.z = (ChangeCursorToTileMap().y - (int(float(height_) * 0.5f))) * player_->GetObjectDistance().y;
	player_->AddBone(bone);

	Tex2DInstance* sailTexture = new Tex2DInstance();
	sailTexture->Load("./Resources/InGame/customizeSail.png");
	sailTexture->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x  , (ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y ,0.0f };
	sailTexture->scale = { player_->GetTileMapSize() * 3.0f * decisionSize_ ,player_->GetTileMapSize() * decisionSize_ };
	sailTexture->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
	sailTexture->color = Vector4ToUint(setColor_);
	player_->AddSailTextures(sailTexture);
	player_->AddSailsIntervals(interval_, isLoop_);

	Tex2DInstance* sailIntervalTexture = new Tex2DInstance();
	sailIntervalTexture->Load("./Resources/white2x2.png");
	sailIntervalTexture->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x , (ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y ,0.0f };
	sailIntervalTexture->scale = { player_->GetTileMapSize() * 3.0f * decisionSize_ ,player_->GetTileMapSize() * decisionSize_ };
	sailIntervalTexture->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
	sailIntervalTexture->color = Vector4ToUint(setIntervalTextureColor_);
	sailIntervalTexture->uvPibot = { 0.0f,0.0f };
	player_->AddSailsIntervalTextures(sailIntervalTexture);

	Tex2DInstance* sailNumber = new Tex2DInstance();
	sailNumber->Load("./Resources/outLineNumberS.png");
	sailNumber->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y,0.0f };
	sailNumber->scale = { player_->GetNumberTextureSize() * decisionSize_ ,player_->GetNumberTextureSize() * decisionSize_ };
	sailNumber->uvSize = { 0.1f,1.0f };
	sailNumber->uvPibot = { interval_ * 0.1f,1.0f };
	sailNumber->color = Vector4ToUint(Vector4::kIdentity);
	player_->AddSailNumbers(sailNumber);

	std::unique_ptr<Tex2DInstance> left = std::make_unique<Tex2DInstance>();
	std::unique_ptr<Tex2DInstance> right = std::make_unique<Tex2DInstance>();

	left->Load("./Resources/InGame/minus.png");
	right->Load("./Resources/InGame/plus.png");

	left->scale = { intervalScale_, intervalScale_ };
	right->scale = { intervalScale_, intervalScale_ };

	left->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y, 0.0f };
	right->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y, 0.0f };
	if (angle_ == 0.0f || angle_ == 180.0f) {
		left->pos.x -= intervalTextureDistance_;
		right->pos.x += intervalTextureDistance_;

		left->rotate.z = angle_ * std::numbers::pi_v<float> / 180.0f;
		right->rotate.z = (angle_ - 180.0f) * std::numbers::pi_v<float> / 180.0f;
	}
	else if (angle_ == 90.0f || angle_ == 270.0f) {
		left->pos.y -= intervalTextureDistance_;
		right->pos.y += intervalTextureDistance_;

		left->rotate.z = (angle_ - 90.0f) * std::numbers::pi_v<float> / 180.0f;
		right->rotate.z = (angle_ + 90.0f) * std::numbers::pi_v<float> / 180.0f;
	}
	sailInterval_.emplace_back(Interval(std::move(left), std::move(right)));
}

void Customize::SetCannon(float x, float y) {
	player_->SetCannonCount(1);

	cursorCannon_->scale = { setObjectScale_,setObjectScale_ ,setObjectScale_ };

	ModelInstance* cannon = new ModelInstance();
	cannon->Load("./Resources/Player/cannon.obj");
	cannon->SetParent(player_->GetBody());
	cannon->rotate.y = angle_ * std::numbers::pi_v<float> / 180.0f;
	cannon->scale.x = setObjectScale_;
	cannon->scale.y = setObjectScale_;
	cannon->scale.z = setObjectScale_;
	cannon->pos.x = (ChangeCursorToTileMap().x - (int(float(width_) * 0.5f))) * player_->GetObjectDistance().x;
	cannon->pos.z = (ChangeCursorToTileMap().y - (int(float(height_) * 0.5f))) * player_->GetObjectDistance().y;
	player_->AddCannons(cannon);
	Tex2DInstance* cannonTexture = new Tex2DInstance();
	cannonTexture->Load("./Resources/InGame/customizeCannon.png");
	cannonTexture->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x , (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	cannonTexture->scale = { player_->GetTileMapSize() * decisionSize_  ,player_->GetTileMapSize() * 2.0f * decisionSize_ };
	cannonTexture->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
	cannonTexture->color = Vector4ToUint(setColor_);
	player_->AddCannonsTextures(cannonTexture);
	player_->AddCannonsIntervals(interval_, false);
	Tex2DInstance* cannonsIntervalTexture = new Tex2DInstance();
	cannonsIntervalTexture->Load("./Resources/white2x2.png");
	cannonsIntervalTexture->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x , (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	cannonsIntervalTexture->scale = { player_->GetTileMapSize() * decisionSize_  ,player_->GetTileMapSize() * 2.0f * decisionSize_ };
	cannonsIntervalTexture->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
	cannonsIntervalTexture->color = Vector4ToUint(setIntervalTextureColor_);
	cannonsIntervalTexture->uvPibot = { 0.0f,0.0f };
	player_->AddCannonIntervalTextures(cannonsIntervalTexture);
	Tex2DInstance* cannonNumber = new Tex2DInstance();
	cannonNumber->Load("./Resources/outLineNumberS.png");
	cannonNumber->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	cannonNumber->scale = { player_->GetNumberTextureSize() * decisionSize_   ,player_->GetNumberTextureSize() * decisionSize_ };
	cannonNumber->uvSize = { 0.1f,1.0f };
	cannonNumber->uvPibot = { interval_ * 0.1f,1.0f };
	cannonNumber->color = Vector4ToUint(Vector4::kIdentity);
	player_->AddCannonsNumbers(cannonNumber);

	std::unique_ptr<Tex2DInstance> left = std::make_unique<Tex2DInstance>();
	std::unique_ptr<Tex2DInstance> right = std::make_unique<Tex2DInstance>();

	left->Load("./Resources/InGame/minus.png");
	right->Load("./Resources/InGame/plus.png");

	left->scale = { intervalScale_, intervalScale_ };
	right->scale = { intervalScale_, intervalScale_ };

	left->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	right->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	if (angle_ == 0.0f || angle_ == 180.0f) {
		left->pos.y -= intervalTextureDistance_;
		right->pos.y += intervalTextureDistance_;
		if (angle_ == 0.0f) {
			left->rotate.z = angle_ * std::numbers::pi_v<float> / 180.0f;
			right->rotate.z = (angle_ + 180.0f) * std::numbers::pi_v<float> / 180.0f;
		}
		else {
			left->rotate.z = angle_ * std::numbers::pi_v<float> / 180.0f;
			right->rotate.z = (angle_ - 180.0f) * std::numbers::pi_v<float> / 180.0f;
		}
	}
	else if (angle_ == 90.0f || angle_ == 270.0f) {
		left->pos.x -= intervalTextureDistance_;
		right->pos.x += intervalTextureDistance_;
		if (angle_ == 90.0f) {
			left->rotate.z = (angle_ - 90.0f) * std::numbers::pi_v<float> / 180.0f;
			right->rotate.z = (angle_ + 90.0f) * std::numbers::pi_v<float> / 180.0f;
		}
		else {
			left->rotate.z = (angle_ + 90.0f) * std::numbers::pi_v<float> / 180.0f;
			right->rotate.z = (angle_ - 90.0f) * std::numbers::pi_v<float> / 180.0f;
		}
	}

	cannonInterval_.emplace_back(Interval(std::move(left), std::move(right)));
}

void Customize::SetLoopCannon(float x, float y) {

	cursorLoopCannon_->scale = { setObjectScale_,setObjectScale_ ,setObjectScale_ };

	player_->SetLoopCannonCount(1);
	ModelInstance* cannon = new ModelInstance();
	cannon->Load("./Resources/Player/loopCannon.obj");
	cannon->SetParent(player_->GetBody());
	cannon->rotate.y = angle_ * std::numbers::pi_v<float> / 180.0f;
	cannon->scale.x = setObjectScale_;
	cannon->scale.y = setObjectScale_;
	cannon->scale.z = setObjectScale_;
	cannon->pos.x = ((ChangeCursorToTileMap().x - (int(float(width_) * 0.5f)))) * player_->GetObjectDistance().x;
	cannon->pos.z = ((ChangeCursorToTileMap().y - (int(float(height_) * 0.5f)))) * player_->GetObjectDistance().y;
	player_->AddLoopCannons(cannon);
	Tex2DInstance* cannonTexture = new Tex2DInstance();
	cannonTexture->Load("./Resources/InGame/customizeLoopCannon.png");
	cannonTexture->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x , (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	cannonTexture->scale = { player_->GetTileMapSize() * decisionSize_ ,player_->GetTileMapSize() * 2.0f * decisionSize_ };
	cannonTexture->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
	cannonTexture->color = Vector4ToUint(setColor_);
	player_->AddLoopCannonsTextures(cannonTexture);
	player_->AddLoopCannonsIntervals(interval_, true);
	Tex2DInstance* cannonsIntervalTexture = new Tex2DInstance();
	cannonsIntervalTexture->Load("./Resources/white2x2.png");
	cannonsIntervalTexture->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x , (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	cannonsIntervalTexture->scale = { player_->GetTileMapSize() * decisionSize_ ,player_->GetTileMapSize() * 2.0f * decisionSize_ };
	cannonsIntervalTexture->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
	cannonsIntervalTexture->color = Vector4ToUint(setIntervalTextureColor_);
	cannonsIntervalTexture->uvPibot = { 0.0f,0.0f };
	player_->AddLoopCannonIntervalTextures(cannonsIntervalTexture);
	Tex2DInstance* cannonNumber = new Tex2DInstance();
	cannonNumber->Load("./Resources/outLineNumberS.png");
	cannonNumber->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	cannonNumber->scale = { player_->GetNumberTextureSize() * decisionSize_  ,player_->GetNumberTextureSize() * decisionSize_ };
	cannonNumber->uvSize = { 0.1f,1.0f };
	cannonNumber->uvPibot = { interval_ * 0.1f,1.0f };
	cannonNumber->color = Vector4ToUint(Vector4::kIdentity);
	player_->AddLoopCannonsNumbers(cannonNumber);

	std::unique_ptr<Tex2DInstance> left = std::make_unique<Tex2DInstance>();
	std::unique_ptr<Tex2DInstance> right = std::make_unique<Tex2DInstance>();

	left->Load("./Resources/InGame/minus.png");
	right->Load("./Resources/InGame/plus.png");

	left->scale = { intervalScale_, intervalScale_ };
	right->scale = { intervalScale_, intervalScale_ };

	left->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	right->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
	if (angle_ == 0.0f || angle_ == 180.0f) {
		left->pos.y -= intervalTextureDistance_;
		right->pos.y += intervalTextureDistance_;
		if (angle_ == 0.0f) {
			left->rotate.z = angle_ * std::numbers::pi_v<float> / 180.0f;
			right->rotate.z = (angle_ + 180.0f) * std::numbers::pi_v<float> / 180.0f;
		}
		else {
			left->rotate.z = angle_ * std::numbers::pi_v<float> / 180.0f;
			right->rotate.z = (angle_ - 180.0f) * std::numbers::pi_v<float> / 180.0f;
		}
	}
	else if (angle_ == 90.0f || angle_ == 270.0f) {
		left->pos.x -= intervalTextureDistance_;
		right->pos.x += intervalTextureDistance_;
		if (angle_ == 90.0f) {
			left->rotate.z = (angle_ - 90.0f) * std::numbers::pi_v<float> / 180.0f;
			right->rotate.z = (angle_ + 90.0f) * std::numbers::pi_v<float> / 180.0f;
		}
		else {
			left->rotate.z = (angle_ + 90.0f) * std::numbers::pi_v<float> / 180.0f;
			right->rotate.z = (angle_ - 90.0f) * std::numbers::pi_v<float> / 180.0f;
		}
	}
	loopCannonInterval_.emplace_back(Interval(std::move(left), std::move(right)));
}

void Customize::SetColor() {
	{
		// カウント
		if (currentType_ == Customize::Type::kSail) {
			cursorNumbers_->pos = ChangeCursorToTexturePos();
			//cursorNumbers_->pos = {
			//	(ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x,
			//	(ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y,
			//};
		}
		else if (currentType_ == Customize::Type::kCannon ||
			currentType_ == Customize::Type::kLoopCannon) {
			float x = 0, y = 0;
			if (angle_ == 0.0f) {
				y = 0.5f;
			}
			else if (angle_ == 90.0f) {
				x = -0.5f;
			}
			else if (angle_ == 180.0f) {
				y = -0.5f;
			}
			else {
				x = 0.5f;
			}
			cursorNumbers_->pos = ChangeCursorToTexturePos();
			//cursorNumbers_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + x) * player_->GetTileMapSize() + player_->GetTileMapOffset().x, (ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - y) * player_->GetTileMapSize() + player_->GetTileMapOffset().y };
		}
		cursorNumbers_->uvSize = { 0.1f ,1.0f };
		cursorNumbers_->uvPibot = { interval_ * 0.1f ,0.0f };
		switch (currentType_) {
		case Customize::kNone:
			break;
		case Customize::kSail:
			cursorSailTexture_->pos = ChangeCursorToTexturePos();
			//cursorSailTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y , 0.0f };
			cursorSailTexture_->scale = { player_->GetTileMapSize() * 3.0f, player_->GetTileMapSize() };
			cursorSailTexture_->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
			break;
		case Customize::kCannon:
			cursorCannonTexture_->pos = ChangeCursorToTexturePos();
			if (angle_ == 0.0f) {
				cursorCannonTexture_->pos.y -= player_->GetTileMapSize() * 0.5f;
			}
			else if (angle_ == 90.0f) {
				cursorCannonTexture_->pos.x -= player_->GetTileMapSize() * 0.5f;
			}
			else if (angle_ == 180) {
				cursorCannonTexture_->pos.y += player_->GetTileMapSize() * 0.5f;
			}
			else {
				cursorCannonTexture_->pos.x += player_->GetTileMapSize() * 0.5f;
			}
			//if (angle_ == 0.0f) {
			//	cursorCannonTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x ,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - 0.5f) * player_->GetTileMapSize() + player_->GetTileMapOffset().y , 0.0f };
			//}
			//else if (angle_ == 90.0f) {
			//	cursorCannonTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) - 0.5f) * player_->GetTileMapSize() + player_->GetTileMapOffset().x,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y , 0.0f };
			//}
			//else if (angle_ == 180) {
			//	cursorCannonTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f) + 0.5f) * player_->GetTileMapSize() + player_->GetTileMapOffset().y , 0.0f };
			//}
			//else {
			//	cursorCannonTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + 0.5f) * player_->GetTileMapSize() + player_->GetTileMapOffset().x,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y, 0.0f };
			//}
			cursorCannonTexture_->scale = { player_->GetTileMapSize() , player_->GetTileMapSize() * 2.0f };
			cursorCannonTexture_->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
			break;
		case Customize::kLoopCannon:
			cursorLoopCannonTexture_->pos = ChangeCursorToTexturePos();
			if (angle_ == 0.0f) {
				cursorLoopCannonTexture_->pos.y -= player_->GetTileMapSize() * 0.5f;
			}
			else if (angle_ == 90.0f) {
				cursorLoopCannonTexture_->pos.x -= player_->GetTileMapSize() * 0.5f;
			}
			else if (angle_ == 180) {
				cursorLoopCannonTexture_->pos.y += player_->GetTileMapSize() * 0.5f;
			}
			else {
				cursorLoopCannonTexture_->pos.x += player_->GetTileMapSize() * 0.5f;
			}
			//if (angle_ == 0.0f) {
			//	cursorLoopCannonTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x ,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f) - 0.5f) * player_->GetTileMapSize() + player_->GetTileMapOffset().y , 0.0f };
			//}
			//else if (angle_ == 90.0f) {
			//	cursorLoopCannonTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) - 0.5f) * player_->GetTileMapSize() + player_->GetTileMapOffset().x,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y , 0.0f };
			//}
			//else if (angle_ == 180) {
			//	cursorLoopCannonTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().x,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f) + 0.5f) * player_->GetTileMapSize() + player_->GetTileMapOffset().y , 0.0f };
			//}
			//else {
			//	cursorLoopCannonTexture_->pos = { (ChangeCursorToTileMap().x - int(float(width_) * 0.5f) + 0.5f) * player_->GetTileMapSize() + player_->GetTileMapOffset().x,(ChangeCursorToTileMap().y - int(float(height_) * 0.5f)) * player_->GetTileMapSize() + player_->GetTileMapOffset().y, 0.0f };
			//}
			cursorLoopCannonTexture_->scale = { player_->GetTileMapSize() , player_->GetTileMapSize() * 2.0f };
			cursorLoopCannonTexture_->rotate.z = -angle_ * std::numbers::pi_v<float> / 180.0f;
			break;
		case Customize::kCursor:
			break;
		case Customize::kTypeCount:
			break;
		default:
			break;
		}
	}
}

void Customize::LoadJson(const std::string& stageName) {
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
				if (objectName.find("Sail") != std::string::npos) {
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();
						if (itemNameObject == "Max") {
							maxSailCount_ = itItemObject->get<int32_t>();
						}
					}
				}
				else if (objectName.find("LoopCannon") != std::string::npos) {
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();
						if (itemNameObject == "Max") {
							maxLoopCannonCount_ = itItemObject->get<int32_t>();
						}

					}
				}
				else if (objectName.find("Cannon") != std::string::npos) {
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();
						if (itemNameObject == "Max") {
							maxCannonCount_ = itItemObject->get<int32_t>();
						}

					}
				}

			}
		}
	}
}

void Customize::SaveJson(const std::string& stageName) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	root[fileName_]["objectData"][("Sail")]["Max"] = maxSailCount_;
	root[fileName_]["objectData"][("Cannon")]["Max"] = maxCannonCount_;
	root[fileName_]["objectData"][("LoopCannon")]["Max"] = maxLoopCannonCount_;

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

void Customize::SetMapData(int32_t x, int32_t y, Player::Type type) {
	int32_t cursor_x = int32_t(ChangeCursorToTileMap().x);
	int32_t cursor_y = int32_t(ChangeCursorToTileMap().y);
	if (cursor_x != -1 ||
		cursor_y != -1) {
		if (type == Player::Type::kSail) {
			player_->SetMapDataType(cursor_x, cursor_y, Player::kSail);
			player_->SetMapDataType(cursor_x - x, cursor_y - y, Player::kSail);
			player_->SetMapDataType(cursor_x + x, cursor_y + y, Player::kSail);
			player_->SetMapDataIndex(cursor_x, cursor_y, player_->GetSailCount());
			player_->SetMapDataIndex(cursor_x - x, cursor_y - y, player_->GetSailCount());
			player_->SetMapDataIndex(cursor_x + x, cursor_y + y, player_->GetSailCount());
		}
		else if (type == Player::Type::kCannon) {
			player_->SetMapDataType(cursor_x, cursor_y, Player::kCannon);
			player_->SetMapDataType(cursor_x + x, cursor_y + y, Player::kCannon);
			player_->SetMapDataIndex(cursor_x, cursor_y, player_->GetCannonCount());
			player_->SetMapDataIndex(cursor_x + x, cursor_y + y, player_->GetCannonCount());
		}
		else if (type == Player::Type::kLoopCannon) {
			player_->SetMapDataType(cursor_x, cursor_y, Player::kLoopCannon);
			player_->SetMapDataType(cursor_x + x, cursor_y + y, Player::kLoopCannon);
			player_->SetMapDataIndex(cursor_x, cursor_y, player_->GetLoopCannonCount());
			player_->SetMapDataIndex(cursor_x + x, cursor_y + y, player_->GetLoopCannonCount());
		}
	}
}

bool Customize::IntervalUpdate(int32_t index) {
	// 秒数
	static uint32_t kMaxInterval = 5;
	static float kMaxSize = 1.2f;
	auto mouse = Input::GetInstance()->GetMouse();

	for (auto& sail : sailInterval_) {
		if (mouse->Pushed(Mouse::Button::Left) &&
			sail.left->Collision(ChangeCursorToTexturePos())) {
			moveTile_->Start(seVolume_, false);
			uint32_t interval = player_->GetSailInterval(index);
			interval--;
			if (interval <= 0) {
				interval = kMaxInterval;
			}
			// インターバルカウントを変更したときのテクスチャの更新
			player_->SetSailInterval(index, interval);
			player_->SetSailsIntervalNumbers(index);
			sail.left->scale = { intervalScale_ * kMaxSize ,intervalScale_ * kMaxSize };
			return true;
		}
		if (mouse->Pushed(Mouse::Button::Left) &&
			sail.right->Collision(ChangeCursorToTexturePos())) {
			moveTile_->Start(seVolume_, false);
			uint32_t interval = player_->GetSailInterval(index);
			interval++;
			if (interval > kMaxInterval) {
				interval = 1;
			}
			// インターバルカウントを変更したときのテクスチャの更新
			player_->SetSailInterval(index, interval);
			player_->SetSailsIntervalNumbers(index);
			sail.right->scale = { intervalScale_ * kMaxSize ,intervalScale_ * kMaxSize };
			return true;
		}
	}
	for (auto& cannon : cannonInterval_) {
		if (mouse->Pushed(Mouse::Button::Left) &&
			cannon.left->Collision(ChangeCursorToTexturePos())) {
			moveTile_->Start(seVolume_, false);
			uint32_t interval = player_->GetCannonInterval(index);
			interval--;
			if (interval <= 0) {
				interval = kMaxInterval;
			}
			// インターバルカウントを変更したときのテクスチャの更新
			player_->SetCannonInterval(index, interval);
			player_->SetCannonIntervalNumbers(index);
			cannon.left->scale = { intervalScale_ * kMaxSize ,intervalScale_ * kMaxSize };
			return true;
		}
		if (mouse->Pushed(Mouse::Button::Left) &&
			cannon.right->Collision(ChangeCursorToTexturePos())) {
			moveTile_->Start(seVolume_, false);
			uint32_t interval = player_->GetCannonInterval(index);
			interval++;
			if (interval > kMaxInterval) {
				interval = 1;
			}
			// インターバルカウントを変更したときのテクスチャの更新
			player_->SetCannonInterval(index, interval);
			player_->SetCannonIntervalNumbers(index);
			cannon.right->scale = { intervalScale_ * kMaxSize, intervalScale_ * kMaxSize };
			return true;
		}
	}
	for (auto& loopCannon : loopCannonInterval_) {
		if (mouse->Pushed(Mouse::Button::Left) &&
			loopCannon.left->Collision(ChangeCursorToTexturePos())) {
			moveTile_->Start(seVolume_, false);
			uint32_t interval = player_->GetLoopCannonInterval(index);
			interval--;
			if (interval <= 0) {
				interval = kMaxInterval;
			}
			// インターバルカウントを変更したときのテクスチャの更新
			player_->SetLoopCannonInterval(index, interval);
			player_->SetLoopCannonIntervalNumbers(index);
			loopCannon.left->scale = { intervalScale_ * kMaxSize ,intervalScale_ * kMaxSize };
			return true;
		}
		if (mouse->Pushed(Mouse::Button::Left) &&
			loopCannon.right->Collision(ChangeCursorToTexturePos())) {
			moveTile_->Start(seVolume_, false);
			uint32_t interval = player_->GetLoopCannonInterval(index);
			interval++;
			if (interval > kMaxInterval) {
				interval = 1;
			}
			// インターバルカウントを変更したときのテクスチャの更新
			player_->SetLoopCannonInterval(index, interval);
			player_->SetLoopCannonIntervalNumbers(index);
			loopCannon.right->scale = { intervalScale_ * kMaxSize ,intervalScale_ * kMaxSize };
			return true;
		}
	}

	return false;
}

void Customize::DeleteSailIntervalTexture(int32_t index) {
	sailInterval_.erase(sailInterval_.begin() + index);
}

void Customize::DeleteCannonIntervalTexture(int32_t index) {
	cannonInterval_.erase(cannonInterval_.begin() + index);
}

void Customize::DeleteLoopCannonIntervalTexture(int32_t index) {
	loopCannonInterval_.erase(loopCannonInterval_.begin() + index);
}

void Customize::DeleteObject() {
	auto gamepad = Input::GetInstance()->GetGamepad();
	auto mouse = Input::GetInstance()->GetMouse();
	if (gamepad->Pushed(Gamepad::Button::B) ||
		mouse->Pushed(Mouse::Button::Middle)) {
		int32_t cursor_x = int32_t(ChangeCursorToTileMap().x);
		int32_t cursor_y = int32_t(ChangeCursorToTileMap().y);
		int32_t index = player_->GetMapData(cursor_x, cursor_y).index;
		// 帆
		if (player_->GetMapData(cursor_x, cursor_y).type == Player::Type::kSail) {
			// 帆かつ選択されたインデックスを指定して削除
			if (index >= 0 && index <= player_->GetSailCount()) {
				pushB_->Start(seVolume_, false);

				player_->DeleteSails(index);
				player_->DeleteSailTextures(index);
				player_->DeleteBone(index);
				player_->DeleteSailsIntervals(index);
				player_->DeleteSailIntervalTextures(index);
				player_->DeleteSailNumbers(index);
				player_->SetSailCount(-1);

				DeleteSailIntervalTexture(index);

				// 	帆かつ選択されたインデックスをして初期化
				for (int y = 0; y < height_; y++) {
					for (int x = 0; x < width_; x++) {
						if (player_->GetMapData(x, y).type == Player::kSail &&
							player_->GetMapData(x, y).index == index) {
							player_->SetMapDataType(x, y, Player::kNone);
							player_->SetMapDataIndex(x, y, -1);
						}
						else if (player_->GetMapData(x, y).type == Player::kSail &&
							player_->GetMapData(x, y).index >= index) {
							int mapDataIndex = player_->GetMapData(x, y).index;
							player_->SetMapDataIndex(x, y, mapDataIndex - 1);
						}
					}
				}
			}
		}
		// 大砲
		if (player_->GetMapData(cursor_x, cursor_y).type == Player::kCannon) {
			// 帆かつ選択されたインデックスを指定して削除
			if (index >= 0 && index <= player_->GetCannonCount()) {
				pushB_->Start(seVolume_, false);

				player_->DeleteCannons(index);
				player_->DeleteCannonsTextures(index);
				player_->DeleteCannonsIntervals(index);
				player_->DeleteCannonIntervalTextures(index);
				player_->DeleteCannonsNumbers(index);
				player_->SetCannonCount(-1);

				DeleteCannonIntervalTexture(index);

				// 	帆かつ選択されたインデックスをして初期化
				for (int y = 0; y < height_; y++) {
					for (int x = 0; x < width_; x++) {
						if (player_->GetMapData(x, y).type == Player::kCannon &&
							player_->GetMapData(x, y).index == index) {
							player_->SetMapDataType(x, y, Player::kNone);
							player_->SetMapDataIndex(x, y, -1);
						}
						else if (player_->GetMapData(x, y).type == Player::kCannon &&
							player_->GetMapData(x, y).index >= index) {
							int mapDataIndex = player_->GetMapData(x, y).index;
							player_->SetMapDataIndex(x, y, mapDataIndex - 1);
						}
					}
				}
			}
		}
		// 大砲
		if (player_->GetMapData(cursor_x, cursor_y).type == Player::kLoopCannon) {
			// 帆かつ選択されたインデックスを指定して削除
			if (index >= 0 && index <= player_->GetLoopCannonCount()) {
				pushB_->Start(seVolume_, false);

				player_->DeleteLoopCannons(index);
				player_->DeleteLoopCannonsTextures(index);
				player_->DeleteLoopCannonsIntervals(index);
				player_->DeleteLoopCannonIntervalTextures(index);
				player_->DeleteLoopCannonsNumbers(index);
				player_->SetLoopCannonCount(-1);

				DeleteLoopCannonIntervalTexture(index);

				// 	帆かつ選択されたインデックスをして初期化
				for (int y = 0; y < height_; y++) {
					for (int x = 0; x < width_; x++) {
						if (player_->GetMapData(x, y).type == Player::kLoopCannon &&
							player_->GetMapData(x, y).index == index) {
							player_->SetMapDataType(x, y, Player::kNone);
							player_->SetMapDataIndex(x, y, -1);
						}
						else if (player_->GetMapData(x, y).type == Player::kLoopCannon &&
							player_->GetMapData(x, y).index >= index) {
							int mapDataIndex = player_->GetMapData(x, y).index;
							player_->SetMapDataIndex(x, y, mapDataIndex - 1);
						}
					}
				}
			}
		}
	}
}
