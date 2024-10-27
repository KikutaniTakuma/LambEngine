#include "Player.h"

#include <cassert>
#include <filesystem>
#include <fstream>

#include <math.h>    
#include <numbers>

#include "imgui.h"
#include "json.hpp"

#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Game/CameraManager/CameraManager.h"
#include "Game/CollisionManager/Collision/Collision.h"
#include "Game/BulletManager/BulletManager.h"
#include "Game/StageManager/StageManager.h"
#include "Input/Input.h"
#include "Camera/Camera.h"
#include "Utils/EngineInfo.h"
#include "Utils/FileUtils.h"

#include "Utils/ScreenOut.h"

#include "Drawer/DrawerManager.h"

//std::numbers::pi_v<float>;

Player::Player() {
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();

	body_ = std::make_unique<ModelInstance>();
	screw_ = std::make_unique<ModelInstance>();
	body_->Load("./Resources/Player/body.obj");
	screw_->SetParent(body_.get());
	screw_->Load("./Resources/Player/screw.obj");
	screw_->pos.z = -3.0f;
	kTextureSize = 70.0f;
	numberSize_ = kTextureSize - 35.0f;
	tileMapOffset_ = { -305.0f,-105.0f };

	setObjectDistance_ = { 0.4f ,0.59f };
	preSetObjectDistance_ = setObjectDistance_;

	cannonShotSize_ = { 2.5f,2.0f };
	cannonShotShrinkTime_ = 0.1f;
	cannonTextureShotSize_ = { 1.5f,1.5f };
	cannonTextureShotShrinkTime_ = 0.1f;

	sailCount_ = -1;
	cannonCount_ = -1;
	loopCannonCount_ = -1;

	for (int y = 0; y < height_; y++) {
		for (int x = 0; x < width_; x++) {
			tileMap_.at(y).at(x).type = Type::kNone;
			tileMap_.at(y).at(x).index = -1;
			tileMapTexture_.at(y).at(x) = std::make_unique<Tex2DInstance>();
			tileMapLuggageTexture_.at(y).at(x) = std::make_unique<Tex2DInstance>();
			tileMapTexture_.at(y).at(x)->Load("./Resources/tileMap.png");
			tileMapLuggageTexture_.at(y).at(x)->Load("./Resources/luggage.png");
			tileMapTexture_.at(y).at(x)->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			tileMapTexture_.at(y).at(x)->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			tileMapTexture_.at(y).at(x)->scale = { kTextureSize ,kTextureSize };
			tileMapLuggageTexture_.at(y).at(x)->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			tileMapLuggageTexture_.at(y).at(x)->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			tileMapLuggageTexture_.at(y).at(x)->scale = { kTextureSize ,kTextureSize };
		}
	}

	scheme_ = std::make_unique<Tex2DInstance>();
	scheme_->Load("./Resources/uiImage.png");
	scheme_->scale = { 1280.0f,720.0f };

	AudioManager::GetInstance()->Load("./Resources/Sound/move.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/shot.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/sail.wav");
	AudioManager::GetInstance()->Load("./Resources/Sound/start.wav");

	moveAudioSound_ =   AudioManager::GetInstance()->Get("./Resources/Sound/move.wav");
	shotSound_ =		AudioManager::GetInstance()->Get("./Resources/Sound/shot.wav");
	sailSound_ =		AudioManager::GetInstance()->Get("./Resources/Sound/sail.wav");
	startSound_ =		AudioManager::GetInstance()->Get("./Resources/Sound/start.wav");

	maxPowerGauge_ = std::make_unique<Tex2DInstance>();
	currentPowerGauge_ = std::make_unique<Tex2DInstance>();

	maxPowerGauge_->color = Vector4ToUint({ 0.0f,0.0f,1.0f,1.0f });
	maxPowerGauge_->pos = { 430.0f,-321.0f,0.0f };
	maxPowerGauge_->scale = { 362.0f,17.5f };
	currentPowerGauge_->color = Vector4ToUint({ 1.0f,0.0f,0.0f,1.0f });
	currentPowerGauge_->pos = { 249.0f,-321.0f,-1.0f };
	currentPowerGauge_->scale = { 362.0f,17.5f };

	departure_ = std::make_unique<Tex2DInstance>();
	departure_->Load("./Resources/InGame/departure.png");
	departure_->scale = { 1280.0f,720.0f };

	Collider::SetColliderType(Collider::Type::kCollion);
	Collider::SetColliderAttribute(Collider::Attribute::kPlayer);
	Collider::InitializeCollision(body_->scale, body_->rotate, body_->pos);
	Collider::SetCollisionScale(Vector3(1.5f * 0.5f, 10.0f, 2.5f * 0.5f));

	emitterOffset_.z = -2.0f;

	name_ = "Player";
	globalVariables_ = std::make_unique<GlobalVariables>(name_);

	velocityTexturePos_ = { 422.0f,-255.0f };
	velocityTextureInterval_ = 26.0f;
	tensPlace_ = std::make_unique<Tex2DInstance>();
	tensPlace_->Load("./Resources/number.png");
	tensPlace_->scale = { 96.0f,96.0f };
	tensPlace_->uvSize = { 0.1f,1.0f };

	oneRank_ = std::make_unique<Tex2DInstance>();
	oneRank_->Load("./Resources/number.png");
	oneRank_->scale = { 96.0f,96.0f };
	oneRank_->uvSize = { 0.1f,1.0f };

	tensPlace_->pos = { velocityTexturePos_.x - velocityTextureInterval_ ,velocityTexturePos_.y };
	oneRank_->pos = { velocityTexturePos_.x + velocityTextureInterval_ ,velocityTexturePos_.y };

	kmTexture_ = std::make_unique<Tex2DInstance>();
	kmTexture_->Load("./Resources/InGame/km.png");
	kmTexture_->scale = { 128.0f,128.0f };
	kmTexture_->pos = { 548.0f,-260.0f };

	powerUI_ = std::make_unique<Tex2DInstance>();
	powerUI_->Load("./Resources/InGame/power.png");
	powerUI_->scale = { 460.0f,60.0f };
	powerUI_->pos = { 395.0f,-320.0f, 1.0f };

	goalText_ = std::make_unique<Tex2DInstance>();
	goalText_->Load("./Resources/InGame/goalText.png");
	goalText_->scale = { 520.0f,160.0f };
	goalText_->pos = { 0.0f,0.0f };

	goText_ = std::make_unique<Tex2DInstance>();
	goText_->Load("./Resources/InGame/go.png");
	goText_->scale = { 196.0f * 1.2f,128.0f * 1.2f };
	goText_->pos = { 0.0f,0.0f };


	stageNames_ = std::make_unique<Tex2DInstance>();
	stageNames_->Load("./Resources/InGame/stageName.png");
	stageNames_->scale = { 150.0f,30.0f };
	stageNames_->uvSize = { 1.0f / 12.0f, 1.0f };
	stageNames_->uvPibot = { float(StageManager::GetStage()) * (1.0f / 12.0f),0.0f };
	stageNames_->pos = { -500.0f,300.0f };

	isAddPower_ = false;

	magnification_ = 1.4f;

	deathTexture_ = std::make_unique<Tex2DInstance>();
	deathTexture_->Load("./Resources/InGame/byWhirlPool.png");
	deathTexture_->scale = { 480.0f,80.0f };
	deathTexture_->pos = { 0.0f,150.0f };
	deathTexture_->color = Vector4ToUint({ 1.0f,1.0f,1.0f,0.0f });

	isToCustomizeScene_ = false;
	chargeAnchorPoint_ = { 1280.0f * 0.5f,425.0f };
	chargeAnchorPointTexture_ = std::make_unique<Tex2DInstance>();
	chargeAnchorPointTexture_->Load("./Resources/ball.png");
	chargeAnchorPointTexture_->scale = { 10.0f,10.0f };
	chargeAnchorPointTexture_->pos = { chargeAnchorPoint_.x - 1280.0f * 0.5f,chargeAnchorPoint_.y - 720.0f * 0.5f };
	chargeAnchorPointTexture_->pos = { chargeAnchorPoint_.x - 1280.0f * 0.5f,-chargeAnchorPoint_.y + 720.0f * 0.5f };
}

Player::~Player() {
	moveAudioSound_->Stop();
}

void Player::Initialize() {
	SetGlobalVariable();

	moveAudioSound_->Stop();
	shotSound_->Stop();
	sailSound_->Stop();

	deathPosition_ = {};
	deathPlayerPosition_ = {};
	deathScale_ = {};
	body_->rotate = { 0.0f,0.0f,0.0f };
	body_->pos = { 0.0f,0.0f,0.0f };
	prePlayerPos_ = {};
	velocity_ = 0.0f;
	windVelocity_ = 0.0f;
	windVector_ = {};
	windDirection_ = {};
	cannonVelocity_ = 0.0f;
	cannonVector_ = {};
	angleVelocity_ = 0.0f;

	isAlive_ = true;
	isClear_ = false;
	isStart_ = false;
	isCharge_ = false;
	isGoal_ = false;
	isCustomize_ = true;
	isDeathAnimation_ = false;
	isSetDeathPos_ = false;
	currentAngle_ = 90.0f;

	power_ = 0.0f;

	chargeCount_ = 0.0f;
	chargeDuration_ = 0.0f;
	size_ = 0.0f;
	preStick_ = {};
	end_ = -Vector3::kXIdentity;
	start_ = Vector3::kXIdentity;

	easingCount_ = 4.0f;
	deathAnimationEasingCount_ = 3.0f;

	positionEasing_.Start(false, easingCount_,  Easeing::Type::kInSine);
	rotateEasing_.Start(false, easingCount_ * 0.5f,  Easeing::Type::kInSine);
	deathAnimationEasing_.Start(false, deathAnimationEasingCount_ * 0.5f,  Easeing::Type::kInSine);

	for (auto& sailInterval : sailsIntervals_) {
		sailInterval.count = 0;
		sailInterval.isUsed = false;
	}

	for (auto& sailFlag : sailFlag_) {
		sailFlag = 0;
	}
	float preTextureSize = kTextureSize;
	Vector2 preTileOffset = tileMapOffset_;
	kTextureSize = 70.0f;
	numberSize_ = kTextureSize - 35.0f;
	tileMapOffset_ = { -305.0f,-105.0f };
	ResetTexture(preTileOffset, preTextureSize);
	for (auto& cannonInterval : cannonIntervals_) {
		cannonInterval.count = 0;
		cannonInterval.isUsed = false;
	}

	for (auto& loopCannonInterval : loopCannonIntervals_) {
		loopCannonInterval.count = 0;
		loopCannonInterval.isUsed = false;
	}
	for (auto& sailIntervalTexture : sailsIntervalTextures_) {
		sailIntervalTexture->uvPibot = { 0.0f,0.0f };
	}
	for (auto& cannonIntervalTexture : cannonsIntervalTextures_) {
		cannonIntervalTexture->uvPibot = { 0.0f,0.0f };
	}
	for (auto& loopCannonIntervalTexture : loopCannonsIntervalTextures_) {
		loopCannonIntervalTexture->uvPibot = { 0.0f,0.0f };
	}

	for (auto& i : cannonParticle_) {
		i.reset(new Particle{});
		i->LoadSettingDirectory("cannon");
	}

	cannonParticleItr_ = cannonParticle_.begin();

	deathTexture_->color = Vector4ToUint({ 1.0f,1.0f,1.0f,0.0f });
	goEasing_.Start(false, 1.5f,  Easeing::Type::kInSine);

	isToCustomizeScene_ = false;
}

void Player::Update() {
	isAddPower_ = true;

	Mouse();

	if (isAlive_ && !isDeathAnimation_) {
		if (isCharge_) {
			Charge();
		}
		if (isGoal_) {
			GoalUpdate();
		}
		if (isAddPower_) {
			AddPower();
		}
		Move();

		UpdateGauge();

		Sound();

		UpdateCollision(body_->rotate, body_->pos);

		NumberUpdate();

		// go
		if (isStart_) {
			goText_->color = Vector4ToUint(goEasing_.Get({ 1.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,0.0f }));
			goEasing_.Update();
		}

		for (auto& i : cannonParticle_) {
			i->Update();
		}
	}
	/*if (!isAlive_) {
		Initialize();
	}*/
	if (isDeathAnimation_) {
		moveAudioSound_->Stop();
		DeathUpdate();
	}
	ParticleUpdate();
	ResourceUpdate();
#ifdef USE_DEBUG_CODE
	Debug();
	ApplyGlobalVariable();
	globalVariables_->Update();
#endif // USE_DEBUG_CODE
}

void Player::ResourceUpdate() {
	body_->Update();
	screw_->Update();
	for (auto& barrel : barrels_) {
#ifdef USE_DEBUG_CODE
		int x = static_cast<int>(barrel->pos.x / preSetObjectDistance_.x);
		int y = static_cast<int>(barrel->pos.z / preSetObjectDistance_.y);
		barrel->pos.x = float(x) * setObjectDistance_.x;
		barrel->pos.z = float(y) * setObjectDistance_.y;
#endif // USE_DEBUG_CODE

		barrel->Update();
	}
	for (auto& bone : bones_) {
		bone->scale.x = std::lerp(bone->scale.x, 1.0f, cannonShotShrinkTime_);
		bone->scale.y = std::lerp(bone->scale.y, 1.0f, cannonShotShrinkTime_);
		bone->scale.z = std::lerp(bone->scale.z, 1.0f, cannonShotShrinkTime_);
		bone->Update();
	}
	for (auto& sail : sails_) {
		sail->scale.x = std::lerp(sail->scale.x, 1.0f, cannonShotShrinkTime_);
		sail->scale.y = std::lerp(sail->scale.y, 1.0f, cannonShotShrinkTime_);
		sail->scale.z = std::lerp(sail->scale.z, 1.0f, cannonShotShrinkTime_);
		sail->Update();
	}
	for (auto& cannon : cannons_) {
		cannon->scale.x = std::lerp(cannon->scale.x, 1.0f, cannonShotShrinkTime_);
		cannon->scale.y = std::lerp(cannon->scale.y, 1.0f, cannonShotShrinkTime_);
		cannon->scale.z = std::lerp(cannon->scale.z, 1.0f, cannonShotShrinkTime_);
		cannon->Update();
	}
	for (auto& loopCannon : loopCannons_) {
		loopCannon->scale.x = std::lerp(loopCannon->scale.x, 1.0f, cannonShotShrinkTime_);
		loopCannon->scale.y = std::lerp(loopCannon->scale.y, 1.0f, cannonShotShrinkTime_);
		loopCannon->scale.z = std::lerp(loopCannon->scale.z, 1.0f, cannonShotShrinkTime_);
		loopCannon->Update();
	}

	for (auto& sailTexture : sailsTextures_) {
		sailTexture->scale.x = std::lerp(sailTexture->scale.x, kTextureSize * 3.0f, cannonTextureShotShrinkTime_);
		sailTexture->scale.y = std::lerp(sailTexture->scale.y, kTextureSize, cannonTextureShotShrinkTime_);
	}
	for (auto& cannonTexture : cannonsTextures_) {
		cannonTexture->scale.x = std::lerp(cannonTexture->scale.x, kTextureSize, cannonTextureShotShrinkTime_);
		cannonTexture->scale.y = std::lerp(cannonTexture->scale.y, kTextureSize * 2.0f, cannonTextureShotShrinkTime_);
	}
	for (auto& loopCannonTexture : loopCannonsTextures_) {
		loopCannonTexture->scale.x = std::lerp(loopCannonTexture->scale.x, kTextureSize, cannonTextureShotShrinkTime_);
		loopCannonTexture->scale.y = std::lerp(loopCannonTexture->scale.y, kTextureSize * 2.0f, cannonTextureShotShrinkTime_);
	}
	for (auto& sailIntervalTexture : sailsIntervalTextures_) {
		sailIntervalTexture->scale.x = std::lerp(sailIntervalTexture->scale.x, kTextureSize * 3.0f, cannonTextureShotShrinkTime_);
		sailIntervalTexture->scale.y = std::lerp(sailIntervalTexture->scale.y, kTextureSize, cannonTextureShotShrinkTime_);
	}
	for (auto& cannonIntervalTexture : cannonsIntervalTextures_) {
		cannonIntervalTexture->scale.x = std::lerp(cannonIntervalTexture->scale.x, kTextureSize, cannonTextureShotShrinkTime_);
		cannonIntervalTexture->scale.y = std::lerp(cannonIntervalTexture->scale.y, kTextureSize * 2.0f, cannonTextureShotShrinkTime_);
	}
	for (auto& loopCannonIntervalTexture : loopCannonsIntervalTextures_) {
		loopCannonIntervalTexture->scale.x = std::lerp(loopCannonIntervalTexture->scale.x, kTextureSize, cannonTextureShotShrinkTime_);
		loopCannonIntervalTexture->scale.y = std::lerp(loopCannonIntervalTexture->scale.y, kTextureSize * 2.0f, cannonTextureShotShrinkTime_);
	}
	for (auto& sailNumberTexture : sailNumbers_) {
		sailNumberTexture->scale.x = std::lerp(sailNumberTexture->scale.x, numberSize_, cannonTextureShotShrinkTime_);
		sailNumberTexture->scale.y = std::lerp(sailNumberTexture->scale.y, numberSize_, cannonTextureShotShrinkTime_);
	}
	for (auto& cannonNumberTexture : cannonNumbers_) {
		cannonNumberTexture->scale.x = std::lerp(cannonNumberTexture->scale.x, numberSize_, cannonTextureShotShrinkTime_);
		cannonNumberTexture->scale.y = std::lerp(cannonNumberTexture->scale.y, numberSize_, cannonTextureShotShrinkTime_);
	}
	for (auto& loopCannonNumberTexture : loopCannonNumbers_) {
		loopCannonNumberTexture->scale.x = std::lerp(loopCannonNumberTexture->scale.x, numberSize_, cannonTextureShotShrinkTime_);
		loopCannonNumberTexture->scale.y = std::lerp(loopCannonNumberTexture->scale.y, numberSize_, cannonTextureShotShrinkTime_);
	}
}
void Player::Draw(const Camera& camera) {
	body_->Draw(camera.GetViewProjection());
	screw_->Draw(camera.GetViewProjection());
	for (auto& barrel : barrels_) {
		barrel->Draw(camera.GetViewProjection());
	}
	for (auto& bone : bones_) {
		bone->Draw(camera.GetViewProjection());
	}
	for (size_t i = 0; auto & sail : sails_) {
		if (isCustomize_) {
			sail->Draw(camera.GetViewProjection());
		}
		else if (sailFlag_.at(i)) {
			sail->Draw(camera.GetViewProjection());
		}
		i++;
	}
	for (auto& cannon : cannons_) {
		cannon->Draw(camera.GetViewProjection());
	}
	for (auto& loopCannon : loopCannons_) {
		loopCannon->Draw(camera.GetViewProjection());
	}
#ifdef USE_DEBUG_CODE
	DebugDraw(camera.GetViewProjection());
#endif // USE_DEBUG_CODE

	/*for (auto& i : cannonParticle_) {
		i->Draw(camera.rotate, camera.GetViewProjection());
	}*/
}

void Player::DrawUI(const Camera& camera) {

	// カスタマイズ画面で映したくないもの
	if (!isCustomize_ && !isGoal_) {
		powerUI_->blend = BlendType::kUnenableDepthNormal;
		powerUI_->Draw(camera.GetViewOthographics());
		tex2D_->Draw(
			Mat4x4::MakeTranslate(Vector3::kXIdentity * 0.5f) * Mat4x4::MakeAffin(currentPowerGauge_->scale, currentPowerGauge_->rotate, currentPowerGauge_->pos),
			Mat4x4::kIdentity,
			camera.GetViewOthographics(),
			0,
			0xff0000ff,
			BlendType::kUnenableDepthNormal
		);
		//maxPowerGauge_->Draw(camera.GetViewOthographics());
	}
	if (isCharge_ && !isStart_ && !isGoal_ && !isCustomize_) {
		departure_->blend = BlendType::kUnenableDepthNormal;
		departure_->Draw(camera.GetViewOthographics());
		chargeAnchorPointTexture_->blend = BlendType::kUnenableDepthNormal;
		chargeAnchorPointTexture_->Draw(camera.GetViewOthographics());

	}
	else if (!isStart_ && !isGoal_ && !isCustomize_) {
		goalText_->blend = BlendType::kUnenableDepthNormal;
		goalText_->Draw(camera.GetViewOthographics());
	}
	else if (isStart_ && !isGoal_ && !isCustomize_) {
		scheme_->blend = BlendType::kUnenableDepthNormal;
		scheme_->Draw(camera.GetViewOthographics());
		stageNames_->blend = BlendType::kUnenableDepthNormal;
		stageNames_->Draw(camera.GetViewOthographics());
		if (tensPlace_->uvPibot.x != 0.0f) {
			tensPlace_->blend = BlendType::kUnenableDepthNormal;
			tensPlace_->Draw(camera.GetViewOthographics());
		}
		oneRank_->blend = BlendType::kUnenableDepthNormal;
		oneRank_->Draw(camera.GetViewOthographics());
		kmTexture_->blend = BlendType::kUnenableDepthNormal;
		kmTexture_->Draw(camera.GetViewOthographics());
		goText_->blend = BlendType::kUnenableDepthNormal;
		goText_->Draw(camera.GetViewOthographics());
		deathTexture_->blend = BlendType::kUnenableDepthNormal;
		deathTexture_->Draw(camera.GetViewOthographics());
	}
	// チャージ画面で映したくないもの
	if (isCustomize_ || isStart_) {
		for (int y = 0; y < height_; y++) {
			for (int x = 0; x < width_; x++) {
				if (tileMap_.at(y).at(x).type == Type::kNotPut) {
					tileMapLuggageTexture_.at(y).at(x)->pos.z = 1.0f;
					tileMapLuggageTexture_.at(y).at(x)->blend = BlendType::kUnenableDepthNormal;
					tileMapLuggageTexture_.at(y).at(x)->Draw(camera.GetViewOthographics());
				}
				else {
					tileMapTexture_.at(y).at(x)->pos.z = 1.0f;
					tileMapTexture_.at(y).at(x)->blend = BlendType::kUnenableDepthNormal;
					tileMapTexture_.at(y).at(x)->Draw(camera.GetViewOthographics());
				}
			}
		}
		for (auto& sailTexture : sailsTextures_) {
			sailTexture->blend = BlendType::kUnenableDepthNormal;
			sailTexture->Draw(camera.GetViewOthographics());
		}
		for (auto& cannonTexture : cannonsTextures_) {
			cannonTexture->blend = BlendType::kUnenableDepthNormal;
			cannonTexture->Draw(camera.GetViewOthographics());
		}
		for (auto& loopCannonTexture : loopCannonsTextures_) {
			loopCannonTexture->blend = BlendType::kUnenableDepthNormal;
			loopCannonTexture->Draw(camera.GetViewOthographics());
		}
		if (isCustomize_) {
			for (auto& sailIntervalTexture : sailsIntervalTextures_) {
				sailIntervalTexture->blend = BlendType::kUnenableDepthNormal;
				sailIntervalTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& cannonIntervalTexture : cannonsIntervalTextures_) {
				cannonIntervalTexture->blend = BlendType::kUnenableDepthNormal;
				cannonIntervalTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& loopCannonIntervalTexture : loopCannonsIntervalTextures_) {
				loopCannonIntervalTexture->blend = BlendType::kUnenableDepthNormal;
				loopCannonIntervalTexture->Draw(camera.GetViewOthographics());
			}

			for (auto& sailNumberTexture : sailNumbers_) {
				sailNumberTexture->blend = BlendType::kUnenableDepthNormal;
				sailNumberTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& cannonNumberTexture : cannonNumbers_) {
				cannonNumberTexture->blend = BlendType::kUnenableDepthNormal;
				cannonNumberTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& loopCannonNumberTexture : loopCannonNumbers_) {
				loopCannonNumberTexture->blend = BlendType::kUnenableDepthNormal;
				loopCannonNumberTexture->Draw(camera.GetViewOthographics());
			}
		}
		else {

			for (auto& sailNumberTexture : sailNumbers_) {
				sailNumberTexture->blend = BlendType::kUnenableDepthNormal;
				sailNumberTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& cannonNumberTexture : cannonNumbers_) {
				cannonNumberTexture->blend = BlendType::kUnenableDepthNormal;
				cannonNumberTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& loopCannonNumberTexture : loopCannonNumbers_) {
				loopCannonNumberTexture->blend = BlendType::kUnenableDepthNormal;
				loopCannonNumberTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& sailIntervalTexture : sailsIntervalTextures_) {
				sailIntervalTexture->blend = BlendType::kUnenableDepthNormal;
				sailIntervalTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& cannonIntervalTexture : cannonsIntervalTextures_) {
				cannonIntervalTexture->blend = BlendType::kUnenableDepthNormal;
				cannonIntervalTexture->Draw(camera.GetViewOthographics());
			}
			for (auto& loopCannonIntervalTexture : loopCannonsIntervalTextures_) {
				loopCannonIntervalTexture->blend = BlendType::kUnenableDepthNormal;
				loopCannonIntervalTexture->Draw(camera.GetViewOthographics());
			}

		}
	}
}

void Player::DebugTileMap() {
	ImGui::Text("isAlive_:%d", isAlive_);
	ImGui::Text("isClear_:%d", isClear_);
	ImGui::Text("isGoal_:%d", isGoal_);
	ImGui::Text("isStart_:%d", isStart_);
	ImGui::Text("isCharge_:%d", isCharge_);
	ImGui::Text("isCustomize_:%d", isCustomize_);
	ImGui::Text("isAddPower_:%d", isAddPower_);
	if (ImGui::TreeNode("Object")) {
		preSetObjectDistance_ = setObjectDistance_;
		ImGui::DragFloat2("SetObjectDistance", &setObjectDistance_.x, 0.01f, 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Offset")) {
		ImGui::DragFloat("kTextureSize", &kTextureSize, 1.0f, 0.0f);
		numberSize_ = kTextureSize - 35.0f;
		ImGui::DragFloat2("tileMapOffset", &tileMapOffset_.x, 1.0f);
		for (int y = 0; y < height_; y++) {
			for (int x = 0; x < width_; x++) {
				tileMapTexture_.at(y).at(x)->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
				tileMapTexture_.at(y).at(x)->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
				tileMapTexture_.at(y).at(x)->scale = { kTextureSize ,kTextureSize };
				tileMapLuggageTexture_.at(y).at(x)->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
				tileMapLuggageTexture_.at(y).at(x)->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
				tileMapLuggageTexture_.at(y).at(x)->scale = { kTextureSize ,kTextureSize };
			}
		}
		for (auto& sailTexture : sailsTextures_) {
			float x = int(float(width_) * 0.5f) * sailTexture->pos.x - kTextureSize + tileMapOffset_.x;
			float y = int(float(height_) * 0.5f) * sailTexture->pos.y - kTextureSize + tileMapOffset_.y;
			sailTexture->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			sailTexture->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			sailTexture->scale = { kTextureSize * 3.0f,kTextureSize };
		}
		for (auto& cannonTexture : cannonsTextures_) {
			float x = int(float(width_) * 0.5f) * cannonTexture->pos.x - kTextureSize + tileMapOffset_.x;
			float y = int(float(height_) * 0.5f) * cannonTexture->pos.y - kTextureSize + tileMapOffset_.y;
			cannonTexture->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			cannonTexture->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			cannonTexture->scale = { kTextureSize, kTextureSize * 2.0f };
		}
		for (auto& loopCannonTexture : loopCannonsTextures_) {
			float x = int(float(width_) * 0.5f) * loopCannonTexture->pos.x - kTextureSize + tileMapOffset_.x;
			float y = int(float(height_) * 0.5f) * loopCannonTexture->pos.y - kTextureSize + tileMapOffset_.y;
			loopCannonTexture->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			loopCannonTexture->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			loopCannonTexture->scale = { kTextureSize, kTextureSize * 2.0f };
		}
		for (auto& sailNumber : sailNumbers_) {
			float x = int(float(width_) * 0.5f) * sailNumber->pos.x - kTextureSize + tileMapOffset_.x;
			float y = int(float(height_) * 0.5f) * sailNumber->pos.y - kTextureSize + tileMapOffset_.y;
			sailNumber->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			sailNumber->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			sailNumber->scale = { numberSize_,numberSize_ };
		}
		for (auto& cannonNumber : cannonNumbers_) {
			float x = int(float(width_) * 0.5f) * cannonNumber->pos.x - kTextureSize + tileMapOffset_.x;
			float y = int(float(height_) * 0.5f) * cannonNumber->pos.y - kTextureSize + tileMapOffset_.y;
			cannonNumber->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			cannonNumber->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			cannonNumber->scale = { numberSize_,numberSize_ };
		}
		for (auto& loopCannonNumber : loopCannonNumbers_) {
			float x = int(float(width_) * 0.5f) * loopCannonNumber->pos.x - kTextureSize + tileMapOffset_.x;
			float y = int(float(height_) * 0.5f) * loopCannonNumber->pos.y - kTextureSize + tileMapOffset_.y;
			loopCannonNumber->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			loopCannonNumber->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			loopCannonNumber->scale = { numberSize_,numberSize_ };
		}
		ImGui::TreePop();
	}
}


void Player::SetSailTexturesColor(const Vector4& color) {
	for (auto& sailTexture : sailsTextures_) {
		sailTexture->color = Vector4ToUint(color);
	}
}

void Player::SetCannonTexturesColor(const Vector4& color) {
	for (auto& cannonTexture : cannonsTextures_) {
		cannonTexture->color = Vector4ToUint(color);
	}
}

void Player::SetLoopCannonTexturesColor(const Vector4& color) {
	for (auto& loopCannonTexture : loopCannonsTextures_) {
		loopCannonTexture->color = Vector4ToUint(color);
	}
}

void Player::SetVelocity(float velocity) {
	velocity_ += velocity;
	velocity_ = std::clamp(velocity_, 0.0f, floatParameter_.at(kMaxSpeed));
	NumberUpdate();
}

void Player::SetCannonVelocity(float velocity) {
	cannonVelocity_ += velocity;
	cannonVelocity_ = std::clamp(cannonVelocity_, 0.0f, floatParameter_.at(kMaxCannonVelocity));
	NumberUpdate();
}

void Player::SetSailVelocity(float velocity) {
	windVelocity_ += velocity;
	windVelocity_ = std::clamp(windVelocity_, 0.0f, floatParameter_.at(kMaxWindVelocity));
	NumberUpdate();
}

void Player::AddPower() {
	if (isAddPower_ && isCustomize_) {
		float t = 1.0f - (power_ / floatParameter_.at(kMaxPower));
		currentPowerGauge_->scale.x = 362.0f * (1.0f - std::clamp(t, 0.0f, 1.0f));
		chargeCount_ += floatParameter_.at(kCustomizeAddPower) * Lamb::DeltaTime();
		power_ = chargeCount_ * std::numbers::pi_v<float>;
		power_ = std::clamp(power_, 0.0f, floatParameter_.at(kMaxPower));
	}
}

void Player::ResetTexture(Vector2 preTileMapOffset, float preTextureSize) {
	for (auto& bone : bones_) {
		bone->scale = Vector3::kIdentity;
		bone->Update();
	}
	for (auto& sail : sails_) {
		sail->scale = Vector3::kIdentity;
		sail->Update();
	}
	for (auto& cannon : cannons_) {
		cannon->scale = Vector3::kIdentity;
		cannon->Update();
	}
	for (auto& loopCannon : loopCannons_) {
		loopCannon->scale = Vector3::kIdentity;
		loopCannon->Update();
	}
	for (int y = 0; y < height_; y++) {
		for (int x = 0; x < width_; x++) {
			tileMapTexture_.at(y).at(x)->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			tileMapTexture_.at(y).at(x)->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			tileMapTexture_.at(y).at(x)->scale = { kTextureSize ,kTextureSize };
			tileMapLuggageTexture_.at(y).at(x)->pos.x = (x - int(float(width_) * 0.5f)) * kTextureSize + tileMapOffset_.x;
			tileMapLuggageTexture_.at(y).at(x)->pos.y = (y - int(float(height_) * 0.5f)) * kTextureSize + tileMapOffset_.y;
			tileMapLuggageTexture_.at(y).at(x)->scale = { kTextureSize ,kTextureSize };
		}
	}
	for (auto& sailTexture : sailsTextures_) {
		Vector2 pos = { (sailTexture->pos.x - preTileMapOffset.x) / preTextureSize,(sailTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		sailTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		sailTexture->scale.x = kTextureSize * 3.0f;
		sailTexture->scale.y = kTextureSize;
	}
	for (auto& cannonTexture : cannonsTextures_) {
		Vector2 pos = { (cannonTexture->pos.x - preTileMapOffset.x) / preTextureSize,(cannonTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		cannonTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		cannonTexture->scale.x = kTextureSize;
		cannonTexture->scale.y = kTextureSize * 2.0f;
	}
	for (auto& loopCannonTexture : loopCannonsTextures_) {
		Vector2 pos = { (loopCannonTexture->pos.x - preTileMapOffset.x) / preTextureSize,(loopCannonTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		loopCannonTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		loopCannonTexture->scale.x = kTextureSize;
		loopCannonTexture->scale.y = kTextureSize * 2.0f;
	}
	for (auto& sailIntervalTexture : sailsIntervalTextures_) {
		Vector2 pos = { (sailIntervalTexture->pos.x - preTileMapOffset.x) / preTextureSize,(sailIntervalTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		sailIntervalTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		sailIntervalTexture->scale.x = kTextureSize * 3.0f;
		sailIntervalTexture->scale.y = kTextureSize;
	}
	for (auto& cannonIntervalTexture : cannonsIntervalTextures_) {
		Vector2 pos = { (cannonIntervalTexture->pos.x - preTileMapOffset.x) / preTextureSize ,(cannonIntervalTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		cannonIntervalTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		cannonIntervalTexture->scale.x = kTextureSize;
		cannonIntervalTexture->scale.y = kTextureSize * 2.0f;
	}
	for (auto& loopCannonIntervalTexture : loopCannonsIntervalTextures_) {
		Vector2 pos = { (loopCannonIntervalTexture->pos.x - preTileMapOffset.x) / preTextureSize ,(loopCannonIntervalTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		loopCannonIntervalTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		loopCannonIntervalTexture->scale.x = kTextureSize;
		loopCannonIntervalTexture->scale.y = kTextureSize * 2.0f;
	}
	for (auto& sailNumberTexture : sailNumbers_) {
		Vector2 pos = { (sailNumberTexture->pos.x - preTileMapOffset.x) / preTextureSize ,(sailNumberTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		sailNumberTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		sailNumberTexture->scale.x = numberSize_;
		sailNumberTexture->scale.y = numberSize_;
	}
	for (auto& cannonNumberTexture : cannonNumbers_) {
		Vector2 pos = { (cannonNumberTexture->pos.x - preTileMapOffset.x) / preTextureSize ,(cannonNumberTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		cannonNumberTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		cannonNumberTexture->scale.x = numberSize_;
		cannonNumberTexture->scale.y = numberSize_;
	}
	for (auto& loopCannonNumberTexture : loopCannonNumbers_) {
		Vector2 pos = { (loopCannonNumberTexture->pos.x - preTileMapOffset.x) / preTextureSize ,(loopCannonNumberTexture->pos.y - preTileMapOffset.y) / preTextureSize };
		loopCannonNumberTexture->pos = { pos.x * kTextureSize + tileMapOffset_.x,pos.y * kTextureSize + tileMapOffset_.y };
		loopCannonNumberTexture->scale.x = numberSize_;
		loopCannonNumberTexture->scale.y = numberSize_;
	}
}

void Player::SetSailsIntervalNumbers(int32_t index) {
	sailNumbers_.at(index)->uvPibot.x = sailsIntervals_.at(index).interval * 0.1f;
}

void Player::SetCannonIntervalNumbers(int32_t index) {
	cannonNumbers_.at(index)->uvPibot.x = cannonIntervals_.at(index).interval * 0.1f;
}

void Player::SetLoopCannonIntervalNumbers(int32_t index) {
	loopCannonNumbers_.at(index)->uvPibot.x = loopCannonIntervals_.at(index).interval * 0.1f;
}

void Player::StopInGameAudio() {
	moveAudioSound_->Stop();
}

void Player::Move() {
	//auto gamepad = Input::GetInstance()->GetGamepad();
	// スタートフラグが
	if (isStart_ && !isGoal_) {
		CannonUpdate();
		WindUpdate();
		// 進む処理
		{
			// スクリューの回転
			{
				screw_->rotate.x += velocity_ * 0.05f;
				screw_->rotate.x = std::fmod(screw_->rotate.x, 360.0f * std::numbers::pi_v<float> / 180.0f);
			}
			// パワーの進処理
			{
				// パワーがあったら
				if (power_ > 0.0f) {
					float currentMaxSpeed = (floatParameter_.at(kMaxSpeed) - weight_);
					if (velocity_ < currentMaxSpeed) {
						velocity_ += floatParameter_.at(kAcceleration) * Lamb::DeltaTime();
					}
					power_ -= floatParameter_.at(kSubtractPower) * Lamb::DeltaTime();
					power_ = std::clamp(power_, 0.0f, floatParameter_.at(kMaxPower));
				}
				// パワーがなくなったら減速
				if (power_ <= 0.0f) {
					float currentMaxDeceleration = (floatParameter_.at(kDeceleration) + weight_);
					if (velocity_ > 0.0f) {
						velocity_ -= currentMaxDeceleration * Lamb::DeltaTime();
					}
					else {
						velocity_ = 0.0f;
					}
				}

				// 速度をクランプ（最大速度を超えたとき）
				velocity_ = std::clamp(velocity_, 0.0f, floatParameter_.at(kMaxSpeed));
			}
		}
		// 進む方向
		{
			Vector3 vector{};
			vector = { std::cos(currentAngle_ * std::numbers::pi_v<float> / 180.0f),0.0f,std::sin(currentAngle_ * std::numbers::pi_v<float> / 180.0f) };
			// 自機の回転	
			UpdateRotate(vector);
			prePlayerPos_ = body_->pos;
			body_->pos += ((vector_ * velocity_) + (windVector_ * windVelocity_) + (cannonVector_ * cannonVelocity_)) * Lamb::DeltaTime();
		}

	}
	if (!isGoal_) {
		if (isStart_ &&
			Input::GetInstance()->GetKey()->Pushed(DIK_C)) {
			if (CameraManager::GetInstance()->GetType() != CameraManager::Type::kFollow) {
				CameraManager::GetInstance()->SetType(CameraManager::Type::kFollow);
			}
			else {
				CameraManager::GetInstance()->SetType(CameraManager::Type::kFixed);
			}
		}
	}
}

void Player::UpdateRotate(const Vector3& vector) {
	float angle = 0.0f;
	Vector3 from = { 0.0f,0.0f,1.0f };
	Vector3 to = vector;
	float dot = from.Dot(to);
	Vector2 Vector2From = { from.x ,from.z };
	Vector2 Vector2To = { to.x ,to.z };
	if (dot >= 1.0f) {
		angle = 0.0f;
	}
	else if (dot <= -1.0f) {
		angle = 180.0f * std::numbers::pi_v<float> / 180.0f;
	}
	else if (Vector2From.Cross(Vector2To) > 0) {
		angle = -std::acosf(dot);
	}
	else {
		angle = std::acosf(dot);
	}
	angle = LerpShortAngle(body_->rotate.y, angle, floatParameter_.at(FloatParameter::kTurnSpeed));
	body_->rotate.y = angle;
	vector_ = { std::sin(angle),0.0f,std::cos(angle) };
	vector_ = vector_.Normalize();
}

void Player::UpdateGauge() {
	float t = 1.0f - (power_ / floatParameter_.at(kMaxPower));
	currentPowerGauge_->scale.x = 362.0f * (1.0f - std::clamp(t, 0.0f, 1.0f));
}

void Player::Charge() {
	//auto gamepad = Input::GetInstance()->GetGamepad();
	//Vector3 stick = Vector2{ gamepad->GetStick(Gamepad::Stick::LEFT_X,0.3f),gamepad->GetStick(Gamepad::Stick::LEFT_Y,0.3f) };
	Vector3 mouse = Input::GetInstance()->GetMouse()->GetPos();
	mouse -= chargeAnchorPoint_;

	Input::GetInstance()->GetMouse()->Debug();

	// スティックをある程度傾けていたら
	if (0.0f < std::abs(mouse.x) || 0.0f < std::abs(mouse.y)) {
		// 線のベクトルとスティックのベクトルで内積を取りコサインを出す
		float startAndStickCos = start_.Normalize().Dot(mouse.Normalize());
		// 前スティックとの外積を取って前のスティックが左側にあるか(回転させる向きを固定させる)
		Vector3 stickPreStickCorss = mouse.Cross(preStick_).Normalize();
		// スティックと線のクロス積でスティックが線より回転していたらを取る
		Vector3 stickStartCorss = mouse.Cross(start_.Normalize()).Normalize();

		// それぞれの条件
		// 1.動く範囲に入ってるか
		// 2.スティックの動きが左回転か
		// 3.スティックが線より回転してるか
		if (Lamb::Between(startAndStickCos, chargeDuration_, 1.0f)
			&& stickPreStickCorss == Vector3::kZIdentity
			&& stickStartCorss == Vector3::kZIdentity
			) {
			// Quaternion版のDirToDirで回転
			start_ *= Quaternion::DirectionToDirection(start_.Normalize(), mouse.Normalize());
			end_ = -start_;
			// チャージカウントを加算(回転量:単位(rad))
			chargeCount_ += float(std::acos(startAndStickCos));
		}
	}
	preStick_ = mouse;

	power_ = chargeCount_ * std::numbers::pi_v<float> *1.5f;
	power_ = std::clamp(power_, 0.0f, floatParameter_.at(kMaxPower));
	if (power_ >= floatParameter_.at(kMaxPower)) {
		startSound_->Start(seVolume_, false);
		isCharge_ = false;
		float preTextureSize = kTextureSize;
		Vector2 preTileOffset = tileMapOffset_;
		kTextureSize = 45.0f;
		numberSize_ = kTextureSize - 35.0f;
		tileMapOffset_ = { -480.0f,-170.0f };
		ResetTexture(preTileOffset, preTextureSize);
	}
	if (power_ < floatParameter_.at(kMaxPower)) {
		screw_->rotate.x = -chargeCount_;
	}
}

void Player::Sound() {
	// 音
	{
		if (velocity_ > 0.0f) {
			if (!moveAudioSound_->IsStart()) {
				moveAudioSound_->Start(seVolume_, true);
			}
		}
		else {
			velocity_ = 0.0f;
			moveAudioSound_->Stop();
		}
	}
}

void Player::CannonUpdate() {
#pragma region キクタニが追加
	// 大砲が発射されたかを初期化
	isCannonShot_ = false;
#pragma endregion
	// 前後の加速度
	float cannonAcceleration{};
	// ベクトルの合成用
	Vector3 addCannonVector{};
	// 最終的に進む方向
	Vector3 cannonVector{};
	float sumPower = floatParameter_.at(kCannonSubtractPower) * (cannons_.size() + loopCannons_.size());
	struct PosIndex {
		float pos;
		size_t index;
		bool isUsed;
	};
	// 大砲の処理
	{
		std::list<PosIndex> appendPosIndex{};
		for (size_t i = 0; auto & cannon : cannons_) {
			if (!cannonIntervals_.at(i).isUsed &&
#pragma region キクタニが追加
				// 単位を合わせるために1000倍する
				cannonIntervals_.at(i).count >= cannonIntervals_.at(i).interval * 1000u &&
#pragma endregion
				power_ >= sumPower) {

				// エミッタのポジション
				Vector3 emitterPos = cannon->pos + body_->pos;

				// 大砲のパーティクル発生
				(*cannonParticleItr_)->ParticleStart(emitterPos);

				cannonParticleItr_++;
				if (cannonParticleItr_ == cannonParticle_.end()) {
					cannonParticleItr_ = cannonParticle_.begin();
				}


				// 左右の加速度
				float angleAcceleration{};
				// 大砲が後ろと前
				cannonVector = vector_.Normalize() * Mat4x4::MakeRotateY(cannon->rotate.y);
				static double epsilon = 1e-5;
				// 誤差が範囲内であれば 0.0f にする
				if (std::fabs(cannonVector.x) < epsilon) {
					cannonVector.x = 0.0f;
				}
				if (std::fabs(cannonVector.y) < epsilon) {
					cannonVector.y = 0.0f;
				}
				if (std::fabs(cannonVector.z) < epsilon) {
					cannonVector.z = 0.0f;
				}
				cannonVector = cannonVector.Normalize();
				if (cannon->rotate.y == 0.0f * std::numbers::pi_v<float> / 180.0f ||
					cannon->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f ||
					cannon->pos.z / setObjectDistance_.y == 0.0f) {
					if (cannon->rotate.y == 0.0f * std::numbers::pi_v<float> / 180.0f ||
						cannon->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f) {
						velocity_ += cannonVector.Length() * floatParameter_.at(kCannonPower);
					}
					addCannonVector += cannonVector;
				}
				else {
					if (cannon->rotate.y == 90.0f * std::numbers::pi_v<float> / 180.0f) {
						if (std::fabs(std::floor(cannon->pos.z / setObjectDistance_.y)) >= 2.0f) {
							if (cannon->pos.z > 0.0f) {
								angleAcceleration = +floatParameter_.at(kAddAngle) * +magnification_;
							}
							else {
								angleAcceleration = +floatParameter_.at(kAddAngle) * -magnification_;
							}
						}
						else {
							if (cannon->pos.z > 0.0f) {
								angleAcceleration = +floatParameter_.at(kAddAngle);
							}
							else {
								angleAcceleration = -floatParameter_.at(kAddAngle);
							}
						}
						PosIndex tmp{};
						tmp.pos = cannon->pos.z;
						tmp.index = i;
						tmp.isUsed = false;
						appendPosIndex.push_back(tmp);
					}
					else {
						if (std::fabs(std::floor(cannon->pos.z / setObjectDistance_.y)) >= 2.0f) {
							if (cannon->pos.z > 0.0f) {
								angleAcceleration = -floatParameter_.at(kAddAngle) * magnification_;
							}
							else {
								angleAcceleration = -floatParameter_.at(kAddAngle) * -magnification_;
							}
						}
						else {
							if (cannon->pos.z > 0.0f) {
								angleAcceleration = -floatParameter_.at(kAddAngle);

							}
							else {
								angleAcceleration = +floatParameter_.at(kAddAngle);

							}
						}
						PosIndex tmp{};
						tmp.pos = cannon->pos.z;
						tmp.index = i;
						tmp.isUsed = false;
						appendPosIndex.push_back(tmp);
					}
				}
				angleVelocity_ += angleAcceleration;
				power_ -= floatParameter_.at(kCannonSubtractPower);
				cannonIntervals_.at(i).isUsed = true;
				// モデル、テクスチャのスケールを大きく
				cannons_.at(i)->scale.x = cannonShotSize_.x;
				cannons_.at(i)->scale.z = cannonShotSize_.y;

				Vector2 cannonShotOverSize{};
				cannonShotOverSize.x = kTextureSize * cannonTextureShotSize_.x;
				cannonShotOverSize.y = kTextureSize * 2.0f * cannonTextureShotSize_.y;
				Vector2 cannonShotNumberOverSize{ numberSize_ ,numberSize_ };

				cannonsIntervalTextures_.at(i)->scale = cannonShotOverSize;
				cannonsTextures_.at(i)->scale = cannonShotOverSize;
				cannonNumbers_.at(i)->scale = cannonShotNumberOverSize;
				Vector3 cannonPos = (cannon->GetWorldMatrix()).GetTranslate();
				BulletManager::GetInstance()->Create(cannonPos, -cannonVector);
				shotSound_->Start(seVolume_, false);
				//cannonIntervals_.at(i).count = 0;
#pragma region キクタニが追加
				// 大砲が発射されたのでtrueを入れる
				isCannonShot_ = true;
#pragma endregion
			}
			else {
#pragma region キクタニが追加
				// uint32_t型をmillisecondで扱う
				cannonIntervals_.at(i).count += static_cast<uint32_t>(Lamb::DeltaTime() * 1000.0f);
				// インターバル(millisecond)を超えないように調整
				cannonIntervals_.at(i).count = std::min(cannonIntervals_.at(i).count, cannonIntervals_.at(i).interval * 1000u);
#pragma endregion
			}
			float t = std::clamp(float(cannonIntervals_.at(i).count) / float(cannonIntervals_.at(i).interval * 1000u), 0.0f, 1.0f);

			if (cannon->rotate.y == 0.0f * std::numbers::pi_v<float> / 180.0f) {
				cannonsIntervalTextures_.at(i)->uvPibot.x = -t;
			}
			else if (cannon->rotate.y == 90.0f * std::numbers::pi_v<float> / 180.0f) {
				// あってる
				cannonsIntervalTextures_.at(i)->uvPibot.y = t;
			}
			else if (cannon->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f) {
				cannonsIntervalTextures_.at(i)->uvPibot.x = t;
			}
			else {
				cannonsIntervalTextures_.at(i)->uvPibot.y = -t;
			}
			i++;
		}
		// 角度が一緒だった時に横に移動する処理
		if (!appendPosIndex.empty()) {
			auto it1 = appendPosIndex.begin();
			while (it1 != std::prev(appendPosIndex.end())) {
				auto it2 = std::next(it1);
				while (it2 != appendPosIndex.end()) {
					if (std::fabs(it1->pos) == std::fabs(it2->pos) &&
						!it1->isUsed && !it2->isUsed) {
						cannonVector = vector_.Normalize() * Mat4x4::MakeRotateY(cannons_.at(it1->index)->rotate.y);
						addCannonVector += cannonVector;
						it1->isUsed = true;
						it2->isUsed = true;

					}
					else {
						++it2;
					}
				}
				if (it1 != appendPosIndex.end()) {
					++it1;
				}
			}
		}
	}
	// ループ大砲
	{
		std::list<PosIndex> appendPosIndex{};
		for (size_t i = 0; auto & cannon : loopCannons_) {
			if (!loopCannonIntervals_.at(i).isUsed &&
				loopCannonIntervals_.at(i).count >= loopCannonIntervals_.at(i).interval * 1000u &&
				power_ >= sumPower) {
				// 左右の加速度
				float angleAcceleration{};
				// 大砲が後ろと前
				cannonVector = vector_.Normalize() * Mat4x4::MakeRotateY(cannon->rotate.y);
				static double epsilon = 1e-5;
				// 誤差が範囲内であれば 0.0f にする
				if (std::fabs(cannonVector.x) < epsilon) {
					cannonVector.x = 0.0f;
				}
				if (std::fabs(cannonVector.y) < epsilon) {
					cannonVector.y = 0.0f;
				}
				if (std::fabs(cannonVector.z) < epsilon) {
					cannonVector.z = 0.0f;
				}
				cannonVector = cannonVector.Normalize();
				if (cannon->rotate.y == 0.0f * std::numbers::pi_v<float> / 180.0f ||
					cannon->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f ||
					cannon->pos.z / setObjectDistance_.y == 0.0f) {
					if (cannon->rotate.y == 0.0f * std::numbers::pi_v<float> / 180.0f ||
						cannon->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f) {
						velocity_ += cannonVector.Length() * floatParameter_.at(kCannonPower);
					}
					addCannonVector += cannonVector;
				}
				else {
					if (cannon->rotate.y == 90.0f * std::numbers::pi_v<float> / 180.0f) {
						if (std::fabs(std::floor(cannon->pos.z / setObjectDistance_.y)) >= 2.0f) {
							if (cannon->pos.z > 0.0f) {
								angleAcceleration = +floatParameter_.at(kAddAngle) * +magnification_;
							}
							else {
								angleAcceleration = +floatParameter_.at(kAddAngle) * -magnification_;
							}
						}
						else {
							if (cannon->pos.z > 0.0f) {
								angleAcceleration = +floatParameter_.at(kAddAngle);
							}
							else {
								angleAcceleration = -floatParameter_.at(kAddAngle);
							}
						}
						PosIndex tmp{};
						tmp.pos = cannon->pos.z;
						tmp.index = i;
						tmp.isUsed = false;
						appendPosIndex.push_back(tmp);
					}
					else {
						if (std::fabs(std::floor(cannon->pos.z / setObjectDistance_.y)) >= 2.0f) {
							if (cannon->pos.z > 0.0f) {
								angleAcceleration = -floatParameter_.at(kAddAngle) * magnification_;
							}
							else {
								angleAcceleration = -floatParameter_.at(kAddAngle) * -magnification_;
							}
						}
						else {
							if (cannon->pos.z > 0.0f) {
								angleAcceleration = -floatParameter_.at(kAddAngle);

							}
							else {
								angleAcceleration = +floatParameter_.at(kAddAngle);

							}
						}
						PosIndex tmp{};
						tmp.pos = cannon->pos.z;
						tmp.index = i;
						tmp.isUsed = false;
						appendPosIndex.push_back(tmp);
					}
				}
				angleVelocity_ += angleAcceleration;
				power_ -= floatParameter_.at(kCannonSubtractPower);
				loopCannonIntervals_.at(i).isUsed = true;
				if (loopCannonIntervals_.at(i).isLoop) {
					loopCannonIntervals_.at(i).count = 0;
					loopCannonIntervals_.at(i).isUsed = false;
				}
				Vector3 cannonPos = (cannon->GetWorldMatrix()).GetTranslate();
				BulletManager::GetInstance()->Create(cannonPos, -cannonVector);
				shotSound_->Start(seVolume_, false);
				loopCannons_.at(i)->scale.x = cannonShotSize_.x;
				loopCannons_.at(i)->scale.z = cannonShotSize_.y;

				Vector2 cannonShotOverSize{};
				cannonShotOverSize.x = kTextureSize * cannonTextureShotSize_.x;
				cannonShotOverSize.y = kTextureSize * 2.0f * cannonTextureShotSize_.y;
				Vector2 cannonShotNumberOverSize{ numberSize_ ,numberSize_ };

				loopCannonsIntervalTextures_.at(i)->scale = cannonShotOverSize;
				loopCannonsTextures_.at(i)->scale = cannonShotOverSize;
				loopCannonNumbers_.at(i)->scale = cannonShotNumberOverSize;
#pragma region キクタニが追加
				// 大砲が発射されたのでtrueを入れる
				isCannonShot_ = true;
			}
			else {
				loopCannonIntervals_.at(i).count += static_cast<uint32_t>(Lamb::DeltaTime() * 1000.0f);
				loopCannonIntervals_.at(i).count = std::min(loopCannonIntervals_.at(i).count, loopCannonIntervals_.at(i).interval * 1000u);
			}
			float t = std::clamp(float(loopCannonIntervals_.at(i).count) / float(loopCannonIntervals_.at(i).interval * 1000u), 0.0f, 1.0f);
#pragma endregion
			if (cannon->rotate.y == 0.0f * std::numbers::pi_v<float> / 180.0f) {
				loopCannonsIntervalTextures_.at(i)->uvPibot.x = -t;
			}
			else if (cannon->rotate.y == 90.0f * std::numbers::pi_v<float> / 180.0f) {
				// あってる
				loopCannonsIntervalTextures_.at(i)->uvPibot.y = t;
			}
			else if (cannon->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f) {
				loopCannonsIntervalTextures_.at(i)->uvPibot.x = t;
			}
			else {
				loopCannonsIntervalTextures_.at(i)->uvPibot.y = -t;
			}
			i++;
		}
		if (!appendPosIndex.empty()) {
			for (auto it1 = appendPosIndex.begin(); it1 != std::prev(appendPosIndex.end()); ++it1) {
				for (auto it2 = std::next(it1); it2 != appendPosIndex.end(); ++it2) {
					if (std::fabs(it1->pos) == std::fabs(it2->pos)) {
						cannonVector = vector_.Normalize() * Mat4x4::MakeRotateY(loopCannons_.at(it1->index)->rotate.y);
						// 誤差が範囲内であれば 0.0f にする
						static double epsilon = 1e-5;
						if (std::fabs(cannonVector.x) < epsilon) {
							cannonVector.x = 0.0f;
						}
						if (std::fabs(cannonVector.y) < epsilon) {
							cannonVector.y = 0.0f;
						}
						if (std::fabs(cannonVector.z) < epsilon) {
							cannonVector.z = 0.0f;
						}
						addCannonVector += cannonVector * 2.0f;
						it1->isUsed = true;
						it2->isUsed = true;
					}
				}
			}
		}

	}
	if (addCannonVector.Length() != 0.0f) {
		// 前後の大砲の加速
		cannonAcceleration = addCannonVector.Length() * floatParameter_.at(kCannonPower);
		cannonVector_ = addCannonVector.Normalize();
		cannonVelocity_ += cannonAcceleration;
	}
	cannonVelocity_ -= floatParameter_.at(kSubtractCannonVelocity) */* (cannons_.size() + loopCannons_.size()) **/ Lamb::DeltaTime();
	cannonVelocity_ = std::clamp(cannonVelocity_, 0.0f, floatParameter_.at(kMaxCannonVelocity) * (cannons_.size() + loopCannons_.size()));
	currentAngle_ -= angleVelocity_ * Lamb::DeltaTime();
	currentAngle_ = std::fmod(currentAngle_, 360.0f);
	if (std::fabs(angleVelocity_) <= 1.0f) {
		angleVelocity_ = 0.0f;
	}
	else if (angleVelocity_ < 0.0f) {
		angleVelocity_ = std::fabs(angleVelocity_) - floatParameter_.at(kSubtractAngle) * Lamb::DeltaTime();
		angleVelocity_ *= -1.0f;
	}
	else {
		angleVelocity_ = angleVelocity_ - floatParameter_.at(kSubtractAngle) * Lamb::DeltaTime();
	}
}

void Player::WindUpdate() {
	// 風の進む処理
	{
		// フラグ処理
		{
			for (size_t i = 0; auto & sail : sailsIntervals_) {
				sail.count += static_cast<uint32_t>(Lamb::DeltaTime() * 1000.0f);
				sail.count = std::min(sail.count, sail.interval * 1000u);

				if (!sail.isUsed &&
					sail.count >= sail.interval * 1000u) {
					sailFlag_.at(i) ^= 1;
					sail.isUsed = true;
					sailSound_->Start(seVolume_, false);
					Vector2 sailTextureOverSize{};
					sailTextureOverSize = { kTextureSize * 3.0f * cannonTextureShotSize_.x ,kTextureSize * cannonTextureShotSize_.y };
					Vector2 sailTextureNumberOverSize{};
					sailTextureNumberOverSize = { numberSize_ * cannonTextureShotSize_.x ,numberSize_ * cannonTextureShotSize_.y };
					sails_.at(i)->scale.x = cannonShotSize_.x;
					sails_.at(i)->scale.z = cannonShotSize_.y;
					bones_.at(i)->scale.x = cannonShotSize_.x;
					bones_.at(i)->scale.z = cannonShotSize_.y;
					sailsIntervalTextures_.at(i)->scale = sailTextureOverSize;
					sailNumbers_.at(i)->scale = sailTextureNumberOverSize;
					sailsTextures_.at(i)->scale = sailTextureOverSize;

					if (sail.isLoop) {
						sail.isUsed = false;
						sail.count = 0;
					}
				}
				float t = std::clamp(float(sailsIntervals_.at(i).count) / float(sailsIntervals_.at(i).interval * 1000u), 0.0f, 1.0f);

				if (sails_.at(i)->rotate.y == 0.0f * std::numbers::pi_v<float> / 180.0f) {
					sailsIntervalTextures_.at(i)->uvPibot.x = -t;
				}
				else if (sails_.at(i)->rotate.y == 90.0f * std::numbers::pi_v<float> / 180.0f) {
					// あってる
					sailsIntervalTextures_.at(i)->uvPibot.y = t;
				}
				else if (sails_.at(i)->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f) {
					sailsIntervalTextures_.at(i)->uvPibot.x = t;
				}
				else {
					sailsIntervalTextures_.at(i)->uvPibot.y = -t;
				}
				i++;
			}
		}
		// 加速度
		float windAcceleration{};
		// ベクトルの合成用
		Vector3 addWindVector{};
		if (windDirection_.Length() > 0.0f) {
			for (size_t i = 0; auto & sail : sails_) {
				if (sailFlag_.at(i)) {
					Vector3 sailVector = (vector_.Normalize() * Mat4x4::MakeRotateY(sail->rotate.y)).Normalize();
					static double epsilon = 1e-5;
					// 誤差が範囲内であれば 0.0f にする
					if (std::fabs(sailVector.x) < epsilon) {
						sailVector.x = 0.0f;
					}
					if (std::fabs(sailVector.y) < epsilon) {
						sailVector.y = 0.0f;
					}
					if (std::fabs(sailVector.z) < epsilon) {
						sailVector.z = 0.0f;
					}
					sailVector = sailVector.Normalize();
					if (windDirection_.Normalize().Dot(sailVector) >= 0.5f ||
						windDirection_.Normalize().Dot(sailVector) <= -0.5f) {
						Vector3 windVector{};
						if (windDirection_.x == 1.0f) {
							windVector = vector_.Normalize() * Mat4x4::MakeRotateY(90.0f * std::numbers::pi_v<float> / 180.0f);
						}
						else if (windDirection_.x == -1.0f) {
							windVector = vector_.Normalize() * Mat4x4::MakeRotateY(270.0f * std::numbers::pi_v<float> / 180.0f);

						}
						else if (windDirection_.z == 1.0f) {
							windVector = vector_.Normalize() * Mat4x4::MakeRotateY(0.0f * std::numbers::pi_v<float> / 180.0f);

						}
						else if (windDirection_.z == -1.0f) {
							windVector = vector_.Normalize() * Mat4x4::MakeRotateY(180.0f * std::numbers::pi_v<float> / 180.0f);

						}
						if (std::fabs(windVector.x) < epsilon) {
							windVector.x = 0.0f;
						}
						if (std::fabs(windVector.y) < epsilon) {
							windVector.y = 0.0f;
						}
						if (std::fabs(windVector.z) < epsilon) {
							windVector.z = 0.0f;
						}
						windVector = windVector.Normalize();
						addWindVector += windVector;
					}
				}
				i++;
			}
			windAcceleration = addWindVector.Length() * floatParameter_.at(kWindPower);
			windVector_ = addWindVector.Normalize();
			windVelocity_ += windAcceleration * Lamb::DeltaTime();
		}
		windVelocity_ -= floatParameter_.at(kSubtractWindVelocity) * Lamb::DeltaTime();
		windVelocity_ = std::clamp(windVelocity_, 0.0f, floatParameter_.at(kMaxWindVelocity) * sails_.size());
		windDirection_ = Vector3::kZero;
	}
}

void Player::GoalUpdate() {
	Vector3 offset = (Vector3(0.0f, 0.0f, 1.0f) * Mat4x4::MakeRotateY(goal_.rotate.y)).Normalize() * goal_.scale.z * 2.0f;
	body_->pos = positionEasing_.Get(goalPlayerPos_, goal_.pos + offset);
	body_->rotate.y = rotateEasing_.Get(goalPlayerRotate_, LerpShortAngle(goalPlayerRotate_, goal_.rotate.y));
	screw_->rotate.x -= 1.0f;
	positionEasing_.Update();
	rotateEasing_.Update();
	if (positionEasing_.ActiveExit()) {
		isClear_ = true;
	}
}

void Player::NumberUpdate() {
	float sum = ((velocity_)+(windVelocity_)+(cannonVelocity_));
	if (std::fabs(body_->pos.Length() - prePlayerPos_.Length()) <= 0.05f) {
		tensPlace_->uvPibot = { 0.0f * 0.1f, 1.0f };
		oneRank_->uvPibot = { 0.0f * 0.1f, 1.0f };
	}
	else if (sum >= 10) {
		float ten = floor(float(int(sum / 10.0f)));
		float one = floor(std::fmod(sum, 10.0f));

		tensPlace_->uvPibot = { ten * 0.1f, 1.0f };
		oneRank_->uvPibot = { one * 0.1f, 1.0f };
	}
	else {
		float one = floor(std::fmod(sum, 10.0f));
		tensPlace_->uvPibot = { 0.0f * 0.1f, 1.0f };
		oneRank_->uvPibot = { one * 0.1f, 1.0f };
	}

}

void Player::ParticleUpdate() {
}

void Player::DeathUpdate() {
	if (!deathAnimationEasing_.GetIsActive()) {
		if (Input::GetInstance()->GetGamepad()->Pushed(Gamepad::Button::START)) {
			isAlive_ = false;
		}

	}
	Vector3 vector = (deathPlayerPosition_ - deathPosition_).Normalize();
	// 最悪
	float angle = std::atan2(vector.z, vector.x);

	//float cos = deathPlayerPosition_.Normalize().Dot(deathPosition_.Normalize());

	//float sin = deathPlayerPosition_.Normalize().Cross(deathPosition_.Normalize()).Length();

	static float time = 0.0f;
	static const float max = 2.0f * std::numbers::pi_v<float>;
	static const float angularSpeed = max / 120.0f;
	// Increment time in the opposite direction
	time -= angularSpeed;

	// Wrap around time if it becomes negative
	if (time <= 0.0f) {
		time += max;
	}

	//angle;
	body_->pos.x = deathPosition_.x + deathScale_.x * std::cos(time + angle);
	body_->pos.z = deathPosition_.z + deathScale_.z * std::sin(time + angle);
	body_->rotate.y += 0.01f;
	deathScale_.x = std::lerp(deathScale_.x, 0.0f, 0.001f);
	deathScale_.z = std::lerp(deathScale_.z, 0.0f, 0.001f);
	deathTexture_->color = Vector4ToUint(deathAnimationEasing_.Get({ 1.0f,1.0f,1.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }));
	deathAnimationEasing_.Update();
}

void Player::Mouse() {
	auto mouse = Input::GetInstance()->GetMouse();
	if (mouse->Pushed(Mouse::Button::Right) && !isGoal_ && isStart_) {
		isToCustomizeScene_ = true;
	}
}

void Player::CustomizeLoad() {
	const std::filesystem::path kDirectoryPath = "./Datas/Customize/Customize";
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
	nlohmann::json::iterator itGroup = root.find("Customize");
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
					ModelInstance* sail = new ModelInstance();
					sail->Load("./Resources/Player/sail.obj");
					sail->SetParent(body_.get());
					sails_.emplace_back(std::move(sail));
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();
						//要素数3の配列であれば
						if (itItemObject->is_array() && itItemObject->size() == 3) {
							//名前がpositionだった場合、positionを登録
							if (itemNameObject == "position") {
								//float型のjson配列登録
								sails_.back()->pos = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がrotationだった場合、rotationを登録
							else if (itemNameObject == "rotate") {
								//float型のjson配列登録
								sails_.back()->rotate = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がscaleだった場合、scaleを登録
							else if (itemNameObject == "scale") {
								//float型のjson配列登録
								sails_.back()->scale = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
						}
						else if (itemNameObject == "interval") {
							Interval tmp{};
							tmp.interval = itItemObject.value() * 60;
							tmp.count = 0;
							sailsIntervals_.emplace_back(tmp);
						}
					}
					sailCount_++;
					sailFlag_.emplace_back(0);

					int x = int(sails_.back()->pos.x + int(float(width_) * 0.5f));
					int y = int(sails_.back()->pos.z + int(float(height_) * 0.5f));
					tileMap_.at(y).at(x).type = Type::kSail;
					tileMap_.at(y).at(x).index = sailCount_;
					if (sails_.back()->rotate.y == 0 * std::numbers::pi_v<float> / 180.0f ||
						sails_.back()->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f) {
						tileMap_.at(y).at(x + 1).type = Type::kSail;
						tileMap_.at(y).at(x - 1).type = Type::kSail;
						tileMap_.at(y).at(x + 1).index = sailCount_;
						tileMap_.at(y).at(x - 1).index = sailCount_;
					}
					else if (sails_.back()->rotate.y == 90.0f * std::numbers::pi_v<float> / 180.0f ||
						sails_.back()->rotate.y == 270.0f * std::numbers::pi_v<float> / 180.0f) {
						tileMap_.at(y + 1).at(x).type = Type::kSail;
						tileMap_.at(y - 1).at(x).type = Type::kSail;
						tileMap_.at(y + 1).at(x).index = sailCount_;
						tileMap_.at(y - 1).at(x).index = sailCount_;
					}
					Tex2DInstance* sailTexture = new Tex2DInstance();
					sailTexture->Load("./Resources/InGame/customizeSail.png");
					sailTexture->pos = { (sails_.back()->pos.x) * kTextureSize + tileMapOffset_.x, (sails_.back()->pos.z) * kTextureSize + tileMapOffset_.y };
					sailTexture->scale = { kTextureSize * 3.0f,kTextureSize };
					sailTexture->rotate.z = sails_.back()->rotate.y;
					sailsTextures_.emplace_back(std::move(sailTexture));
					Tex2DInstance* sailNumber = new Tex2DInstance();
					sailNumber->Load("./Resources/outLineNumber.png");
					sailNumber->pos = { (sails_.back()->pos.x) * kTextureSize + tileMapOffset_.x, (sails_.back()->pos.z) * kTextureSize + tileMapOffset_.y };
					sailNumber->scale = { numberSize_,numberSize_ };
					sailNumber->uvSize = { 0.1f,1.0f };
					sailNumber->uvPibot = { sailsIntervals_.back().interval * 0.1f,1.0f };
					sailNumbers_.emplace_back(sailNumber);
					sailCount_++;
				}
				else if (objectName.find("Cannon") != std::string::npos) {
					// Assuming you have a Cannon class
					ModelInstance* cannon = new ModelInstance();
					cannon->Load("./Resources/Player/cannon.obj");
					cannon->SetParent(body_.get());
					cannons_.emplace_back(std::move(cannon));
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();

						//要素数3の配列であれば
						if (itItemObject->is_array() && itItemObject->size() == 3) {

							//名前がpositionだった場合、positionを登録
							if (itemNameObject == "position") {
								//float型のjson配列登録
								cannons_.back()->pos = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がrotationだった場合、rotationを登録
							else if (itemNameObject == "rotate") {
								//float型のjson配列登録
								cannons_.back()->rotate = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がscaleだった場合、scaleを登録
							else if (itemNameObject == "scale") {
								//float型のjson配列登録
								cannons_.back()->scale = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
						}
						else if (itemNameObject == "interval") {
							uint32_t interval = itItemObject.value();
							cannonIntervals_.emplace_back(interval);
						}
					}
					int x = int(cannons_.back()->pos.x + int(float(width_) * 0.5f));
					int y = int(cannons_.back()->pos.z + int(float(height_) * 0.5f));
					tileMap_.at(y).at(x).type = Type::kCannon;
					tileMap_.at(y).at(x).index = cannonCount_;
					Tex2DInstance* cannonTexture = new Tex2DInstance();
					cannonTexture->Load("./Resources/InGame/customizeCannon.png");
					cannonTexture->scale = { kTextureSize,kTextureSize * 2.0f };
					cannonTexture->rotate.z = cannons_.back()->rotate.y;
					if (cannons_.back()->rotate.y == 0 * std::numbers::pi_v<float> / 180.0f) {
						cannonTexture->pos = { (cannons_.back()->pos.x) * kTextureSize + tileMapOffset_.x, (cannons_.back()->pos.z - 0.5f) * kTextureSize + tileMapOffset_.y };
						tileMap_.at(y - 1).at(x).type = Type::kCannon;
						tileMap_.at(y - 1).at(x).index = cannonCount_;
						Tex2DInstance* cannonNumber = new Tex2DInstance();
						cannonNumber->Load("./Resources/outLineNumber.png");
						cannonNumber->pos = { (cannons_.back()->pos.x) * kTextureSize + tileMapOffset_.x, (cannons_.back()->pos.z - 0.5f) * kTextureSize + tileMapOffset_.y };
						cannonNumber->scale = { numberSize_,numberSize_ };
						cannonNumber->uvSize = { 0.1f,1.0f };
						cannonNumber->uvPibot = { cannonIntervals_.back().interval * 0.1f,1.0f };
						cannonNumbers_.emplace_back(cannonNumber);
					}
					else if (cannons_.back()->rotate.y == 90.0f * std::numbers::pi_v<float> / 180.0f) {
						cannonTexture->pos = { (cannons_.back()->pos.x - 0.5f) * kTextureSize + tileMapOffset_.x, (cannons_.back()->pos.z) * kTextureSize + tileMapOffset_.y };
						tileMap_.at(y).at(x - 1).type = Type::kCannon;
						tileMap_.at(y).at(x - 1).index = cannonCount_;
						Tex2DInstance* cannonNumber = new Tex2DInstance();
						cannonNumber->Load("./Resources/outLineNumber.png");
						cannonNumber->pos = { (cannons_.back()->pos.x - 0.5f) * kTextureSize + tileMapOffset_.x, (cannons_.back()->pos.z) * kTextureSize + tileMapOffset_.y };
						cannonNumber->scale = { numberSize_,numberSize_ };
						cannonNumber->uvSize = { 0.1f,1.0f };
						cannonNumber->uvPibot = { cannonIntervals_.back().interval * 0.1f,1.0f };
						cannonNumbers_.emplace_back(cannonNumber);
					}

					else if (cannons_.back()->rotate.y == 180.0f * std::numbers::pi_v<float> / 180.0f) {
						cannonTexture->pos = { (cannons_.back()->pos.x) * kTextureSize + tileMapOffset_.x, (cannons_.back()->pos.z + 0.5f) * kTextureSize + tileMapOffset_.y };
						tileMap_.at(y + 1).at(x).type = Type::kCannon;
						tileMap_.at(y + 1).at(x).index = cannonCount_;
						Tex2DInstance* cannonNumber = new Tex2DInstance();
						cannonNumber->Load("./Resources/outLineNumber.png");
						cannonNumber->pos = { (cannons_.back()->pos.x) * kTextureSize + tileMapOffset_.x, (cannons_.back()->pos.z + 0.5f) * kTextureSize + tileMapOffset_.y };
						cannonNumber->scale = { numberSize_,numberSize_ };
						cannonNumber->uvSize = { 0.1f,1.0f };
						cannonNumber->uvPibot = { cannonIntervals_.back().interval * 0.1f,1.0f };
						cannonNumbers_.emplace_back(cannonNumber);
					}
					else {
						cannonTexture->pos = { (cannons_.back()->pos.x + 0.5f) * kTextureSize + tileMapOffset_.x, (cannons_.back()->pos.z) * kTextureSize + tileMapOffset_.y };
						tileMap_.at(y).at(x + 1).type = Type::kCannon;
						tileMap_.at(y).at(x + 1).index = cannonCount_;
						Tex2DInstance* cannonNumber = new Tex2DInstance();
						cannonNumber->Load("./Resources/outLineNumber.png");
						cannonNumber->pos = { (cannons_.back()->pos.x + 0.5f) * kTextureSize + tileMapOffset_.x, (cannons_.back()->pos.z) * kTextureSize + tileMapOffset_.y };
						cannonNumber->scale = { numberSize_,numberSize_ };
						cannonNumber->uvSize = { 0.1f,1.0f };
						cannonNumber->uvPibot = { cannonIntervals_.back().interval * 0.1f,1.0f };
						cannonNumbers_.emplace_back(cannonNumber);
					}

					cannonsTextures_.emplace_back(std::move(cannonTexture));
					cannonCount_++;
				}
				else if (objectName.find("Bone") != std::string::npos) {
					ModelInstance* bone = new ModelInstance();
					bone->Load("./Resources/Player/bone.obj");
					bone->SetParent(body_.get());
					bones_.emplace_back(std::move(bone));
					for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {
						//アイテム名を取得
						const std::string& itemNameObject = itItemObject.key();

						//要素数3の配列であれば
						if (itItemObject->is_array() && itItemObject->size() == 3) {

							//名前がpositionだった場合、positionを登録
							if (itemNameObject == "position") {
								//float型のjson配列登録
								bones_.back()->pos = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がrotationだった場合、rotationを登録
							else if (itemNameObject == "rotate") {
								//float型のjson配列登録
								bones_.back()->rotate = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}
							//名前がscaleだった場合、scaleを登録
							else if (itemNameObject == "scale") {
								//float型のjson配列登録
								bones_.back()->scale = (Vector3({ itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) }));
							}

						}
					}
				}

			}
		}
	}
}

void Player::Debug() {
	ImGui::Begin("Player");

	ImGui::DragFloat2("deathTexture", &deathTexture_->pos.x, 0.1f);
	ImGui::DragFloat("Magnification", &magnification_, 0.1f);
	ImGui::DragFloat3("Pos", &body_->pos.x, 0.1f);
	ImGui::DragFloat3("rotate", &body_->rotate.x, 0.1f);
	ImGui::DragFloat("Weight", &weight_, 1.0f, 0.0f);
	ImGui::DragFloat("Power", &power_, 1.0f, 0.0f);
	ImGui::DragFloat("Velocity", &velocity_, 1.0f, 0.0f);
	ImGui::DragFloat("WindVelocity", &windVelocity_, 1.0f, 0.0f);
	ImGui::DragFloat("CurrentAngle", &currentAngle_, 1.0f, 0.0f);
	ImGui::Checkbox("isAlive", &isAlive_);
	ImGui::DragFloat("CannonVelocity", &cannonVelocity_, 0.1f);
	ImGui::DragFloat("AngleVelocity", &angleVelocity_, 0.1f);
	ImGui::DragFloat2("TileMapOffset", &tileMapOffset_.x, 0.1f);
	ImGui::DragFloat2("velocityTexturePos", &velocityTexturePos_.x, 0.1f);
	ImGui::DragFloat("velocityTextureInterval", &velocityTextureInterval_, 0.1f);
	ImGui::DragFloat2("kmTexture_", &kmTexture_->pos.x, 0.1f);
	ImGui::DragFloat2("cannonShotSize_", &cannonShotSize_.x, 0.1f, 0.0f);
	ImGui::DragFloat("cannonShotShrinkTime", &cannonShotShrinkTime_, 0.1f, 0.0f);
	tensPlace_->pos = { velocityTexturePos_.x - velocityTextureInterval_ ,velocityTexturePos_.y };
	oneRank_->pos = { velocityTexturePos_.x + velocityTextureInterval_ ,velocityTexturePos_.y };
	ImGui::DragFloat2("chargeAnchorPoint", &chargeAnchorPoint_.x, 0.1f);
	ImGui::End();
	DebugTileMap();
}

void Player::SetGlobalVariable() {
	globalVariables_->CreateGroup(name_);

	for (int i = 0; i < FloatParameter::kFloatParameterCount; i++) {
		globalVariables_->AddItem(name_, floatParameterItemNames_.at(i), floatParameter_.at(i));
	}

	globalVariables_->LoadFile();

	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable() {
	for (int i = 0; i < FloatParameter::kFloatParameterCount; i++) {
		floatParameter_.at(i) = globalVariables_->GetFloatValue(name_, floatParameterItemNames_.at(i));
	}
}

void Player::OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) {
	color_.at(myIndex) = Vector4ToUint(Vector4::kYIdentity);
	Vector3 pushVector{};
	Vector3 prePosVector = prePlayerPos_ - body_->pos;
	prePosVector = prePosVector.Normalize();
	if (collider->GetColliderType(pairIndex) == Collider::Type::kCollion &&
		collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kOther &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex), pushVector)) {
		body_->pos += pushVector;
		UpdateCollision(body_->rotate, body_->pos);
		color_.at(myIndex) = Vector4ToUint(Vector4::kXIdentity);
	}
}

float Player::LerpShortAngle(float a, float b, float t) {
	// 角度差分を求める
	float diff = b - a;
	// 角度[-2PI,+2PI]に補正
	diff = std::fmod(diff, 360.0f * std::numbers::pi_v<float> / 180.0f);
	// 角度[-PI,+PI]に補正
	if (diff > 180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff -= 360.0f * std::numbers::pi_v<float> / 180.0f;
	}
	else if (diff < -180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff += 360.0f * std::numbers::pi_v<float> / 180.0f;
	}
	return std::lerp(a, a + diff, t);
}

float Player::LerpShortAngle(float a, float b) {
	// 角度差分を求める
	float diff = b - a;
	// 角度[-2PI,+2PI]に補正
	diff = std::fmod(diff, 360.0f * std::numbers::pi_v<float> / 180.0f);
	// 角度[-PI,+PI]に補正
	if (diff > 180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff -= 360.0f * std::numbers::pi_v<float> / 180.0f;
	}
	else if (diff < -180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff += 360.0f * std::numbers::pi_v<float> / 180.0f;
	}
	return a + diff;
}
