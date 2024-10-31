#pragma once

#include <cstdint> 
#include <memory>
#include <vector>

#include "AudioManager/AudioManager.h"
#include "Drawer/Model/Model.h"
#include "Drawer/Texture2D/Texture2D.h"
#include "Drawer/Particle/Particle.h"
#include "GameExternals/CollisionManager/Collider/Collider.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Math/Mat4x4.h"
#include "Math/Vector3.h"
#include "Utils/Easeing.h"

class Camera;
class Player :public Collider {
public:
	enum Type {
		kNone,
		kNotPut,
		kSail,
		kCannon,
		kLoopCannon,

		kTypeCount,
	};
private:
	struct MapData {
		Type type;
		int32_t index;
	};
	enum FloatParameter {
		kTurnSpeed,
		kAddAngle,
		kSubtractAngle,
		kAcceleration,
		kDeceleration,
		kWindPower,
		kCannonPower,
		kMaxSpeed,
		kMaxPower,
		kSubtractPower,
		kCannonSubtractPower,
		kMaxCannonVelocity,
		kSubtractCannonVelocity,
		kMaxWindVelocity,
		kSubtractWindVelocity,
		kCustomizeAddPower,
		kFloatParameterCount,
	};
	struct Interval {
		uint32_t count;
		uint32_t interval;
		bool isLoop;
		bool isUsed;
	};
	struct Goal {
		Vector3 scale;
		Vector3 pos;
		Vector3 rotate;
	};
public:
	Player();
	~Player();
	void Initialize();
	void Update();
	void ResourceUpdate();
	void Draw(const Camera& camera);
	void DrawUI(const Camera& camera);

	void DebugTileMap();

	const Vector3& GetDeathPos()const { return deathPlayerPosition_; }
	const Vector3& GetPosition() const { return body_->pos; }
	const Vector3& GetRotate() const { return body_->rotate; }
	const Vector3& GetScale() const { return body_->scale; }
	void SetSailTexturesColor(const Vector4& color);
	void SetCannonTexturesColor(const Vector4& color);
	void SetLoopCannonTexturesColor(const Vector4& color);
	void SetCamera(const Camera* camera) { camera_ = camera; }
	void SetWindDirection(const Vector3& vector) { windDirection_ = vector; }
	void SetVelocity(float velocity);
	void SetSailVelocity(float velocity);
	void SetCannonVelocity(float velocity);
	float GetPower() { return power_; }
	float GetVelocity() { return velocity_; }
	float GetCannonVelocity() { return cannonVelocity_; }
	float GetWindVelocity() { return windVelocity_; }
	float GetSumVelocity() { return velocity_ + cannonVelocity_ + windVelocity_; }
	bool GetIsAlive() { return isAlive_; }
	void SetIsAlive(bool flag) { isAlive_ = flag; }
	bool GetIsClear() { return isClear_; }
	void SetIsClear(bool flag) { isClear_ = flag; }
	bool GetIsCharge() { return isCharge_; }
	void SetIsCharge(bool flag) { isCharge_ = flag; }
	bool GetIsStart() { return isStart_; }
	void SetIsStart(bool flag) { isStart_ = flag; }
	bool GetIsGoal() { return isGoal_; }
	void SetIsGoal(bool flag) { isGoal_ = flag; }
	bool GetIsCustomize() { return isCustomize_; }
	bool GetIsToCustomizeScene() { return isToCustomizeScene_; }
	void SetIsCustomize(bool flag) { isCustomize_ = flag; }
	void SetDeathAnimation(bool flag) { isDeathAnimation_ = flag; }
	bool GetDeathAnimation() { return isDeathAnimation_; }
	bool GetIsSetDeathPos() { return isSetDeathPos_; }
	void SetIsSetDeathPos(bool flag) {  isSetDeathPos_=flag; }

	void SetDeathPosition(const Vector3& playerPos, const Vector3& pos, const Vector3& scale) {
		deathPlayerPosition_ = playerPos;
		deathPosition_ = pos;
		deathScale_ = scale;
	}

	void AddPower();

	float GetTileMapSize() { return kTextureSize; }
	Vector2 GetTileMapOffset() { return tileMapOffset_; }
	float GetNumberTextureSize() { return numberSize_; }

	void SetGoalPlayerPos(const Vector3& pos) { goalPlayerPos_ = pos; }
	void SetGoalPlayerRotate(float rotate) { goalPlayerRotate_ = rotate; }

	void SetGoal(const Vector3& scale, const Vector3& rotate, const Vector3& pos) {
		goal_.scale = scale;
		goal_.rotate = rotate;
		goal_.pos = pos;
	}
	// PlayerInfo
	ModelInstance* GetBody() { return body_.get(); }

	Tex2DInstance* GetSailTextures(int32_t index) { return sailsTextures_.at(index).get(); }
	Tex2DInstance* GetCannonsTextures(int32_t index) { return cannonsTextures_.at(index).get(); }
	Tex2DInstance* GetLoopCannonsTextures(int32_t index) { return loopCannonsTextures_.at(index).get(); }

	void AddBarrel(ModelInstance* model) { barrels_.emplace_back(std::move(model)); }
	void AddSails(ModelInstance* model) { sails_.emplace_back(std::move(model)); }
	void AddCannons(ModelInstance* model) { cannons_.emplace_back(std::move(model)); }
	void AddLoopCannons(ModelInstance* model) { loopCannons_.emplace_back(std::move(model)); }
	void AddBone(ModelInstance* model) { bones_.emplace_back(std::move(model)); }
	void AddSailTextures(Tex2DInstance* texture) { sailsTextures_.emplace_back(std::move(texture)); }
	void AddCannonsTextures(Tex2DInstance* texture) { cannonsTextures_.emplace_back(std::move(texture)); }
	void AddLoopCannonsTextures(Tex2DInstance* texture) { loopCannonsTextures_.emplace_back(std::move(texture)); }
	void AddSailsIntervals(uint32_t interval, bool flag) {
		Interval tmp{};
		tmp.interval = interval;
		tmp.count = 0;
		tmp.isLoop = flag;
		tmp.isUsed = false;
		sailsIntervals_.emplace_back(tmp);
	}
	void AddCannonsIntervals(uint32_t interval, bool flag) {
		Interval tmp{};
#pragma region キクタニが追加
		// 単位を秒に合わせるために60倍にしてたのを削除
		tmp.interval = interval;
#pragma endregion
		tmp.count = 0;
		tmp.isLoop = flag;
		tmp.isUsed = false;
		cannonIntervals_.emplace_back(tmp);
	}
	void AddLoopCannonsIntervals(uint32_t interval, bool flag) {
		Interval tmp{};
		tmp.interval = interval;
		tmp.count = 0;
		tmp.isLoop = flag;
		tmp.isUsed = false;
		loopCannonIntervals_.emplace_back(tmp);
	}

	void AddSailsIntervalTextures(Tex2DInstance* texture) { sailsIntervalTextures_.emplace_back(std::move(texture)); }
	void AddSailNumbers(Tex2DInstance* texture) { sailNumbers_.emplace_back(std::move(texture)); }

	void AddCannonIntervalTextures(Tex2DInstance* texture) { cannonsIntervalTextures_.emplace_back(std::move(texture)); }
	void AddCannonsNumbers(Tex2DInstance* texture) { cannonNumbers_.emplace_back(std::move(texture)); }

	void AddLoopCannonIntervalTextures(Tex2DInstance* texture) { loopCannonsIntervalTextures_.emplace_back(std::move(texture)); }
	void AddLoopCannonsNumbers(Tex2DInstance* texture) { loopCannonNumbers_.emplace_back(std::move(texture)); }
	
	void AddSailFlag() { sailFlag_.emplace_back(0); }

	void DeleteSails(int32_t index) { sails_.erase(sails_.begin() + index); }
	void DeleteCannons(int32_t index) { cannons_.erase(cannons_.begin() + index); }
	void DeleteLoopCannons(int32_t index) { loopCannons_.erase(loopCannons_.begin() + index); }
	void DeleteBone(int32_t index) { bones_.erase(bones_.begin() + index); }
	void DeleteSailTextures(int32_t index) { sailsTextures_.erase(sailsTextures_.begin() + index); }
	void DeleteCannonsTextures(int32_t index) { cannonsTextures_.erase(cannonsTextures_.begin() + index); }
	void DeleteLoopCannonsTextures(int32_t index) { loopCannonsTextures_.erase(loopCannonsTextures_.begin() + index); }
	void DeleteSailsIntervals(int32_t index) { sailsIntervals_.erase(sailsIntervals_.begin() + index); }
	void DeleteCannonsIntervals(int32_t index) { cannonIntervals_.erase(cannonIntervals_.begin() + index); }
	void DeleteLoopCannonsIntervals(int32_t index) { loopCannonIntervals_.erase(loopCannonIntervals_.begin() + index); }
	void DeleteSailIntervalTextures(int32_t index) { sailsIntervalTextures_.erase(sailsIntervalTextures_.begin() + index); }
	void DeleteCannonIntervalTextures(int32_t index) { cannonsIntervalTextures_.erase(cannonsIntervalTextures_.begin() + index); }
	void DeleteLoopCannonIntervalTextures(int32_t index) { loopCannonsIntervalTextures_.erase(loopCannonsIntervalTextures_.begin() + index); }
	void DeleteSailNumbers(int32_t index) { sailNumbers_.erase(sailNumbers_.begin() + index); }
	void DeleteCannonsNumbers(int32_t index) { cannonNumbers_.erase(cannonNumbers_.begin() + index); }
	void DeleteLoopCannonsNumbers(int32_t index) { loopCannonNumbers_.erase(loopCannonNumbers_.begin() + index); }
	void DeleteSailFlag(int32_t index) { sailFlag_.erase(sailFlag_.begin() + index); }

	void SetSailsIntervalNumbers(int32_t index);
	void SetCannonIntervalNumbers(int32_t index);
	void SetLoopCannonIntervalNumbers(int32_t index);

	const MapData GetMapData(int32_t x, int32_t y) const { return tileMap_.at(y).at(x); }
	void SetMapDataIndex(int32_t x, int32_t y, int32_t index) { tileMap_.at(y).at(x).index = index; ; }
	void SetMapDataType(int32_t x, int32_t y, Type type) { tileMap_.at(y).at(x).type = type; }

	uint32_t GetSailInterval(int32_t index) { return sailsIntervals_.at(index).interval; }
	uint32_t GetCannonInterval(int32_t index) { return cannonIntervals_.at(index).interval; }
	uint32_t GetLoopCannonInterval(int32_t index) { return loopCannonIntervals_.at(index).interval; }
	
	void SetSailInterval(int32_t index,uint32_t interval) { sailsIntervals_.at(index).interval= interval; }
	void SetCannonInterval(int32_t index, uint32_t interval) { cannonIntervals_.at(index).interval= interval; }
	void SetLoopCannonInterval(int32_t index, uint32_t interval) { loopCannonIntervals_.at(index).interval= interval; }
	
	int32_t GetSailCount() { return sailCount_; }
	void SetSailCount(const int32_t index) { sailCount_ += index; }
	int32_t GetCannonCount() { return cannonCount_; }
	void SetCannonCount(const int32_t index) { cannonCount_ += index; }
	int32_t GetLoopCannonCount() { return loopCannonCount_; }
	void SetLoopCannonCount(const int32_t index) { loopCannonCount_ += index; }
	Vector2 GetObjectDistance() { return setObjectDistance_; }
	void SetObjectDistance(Vector2 distance) { setObjectDistance_ = distance; }
	float GetObjectShrinkTime() { return cannonShotShrinkTime_; }

#pragma region キクタニが追加
	inline bool GetIsCannonShot() const {
		return isCannonShot_;
	}

	void StopInGameAudio();
#pragma endregion


private:
	static const int width_ = 3;
	static const int height_ = 5;


	float kTextureSize;
	void Move();
	void UpdateRotate(const Vector3& vector);
	void UpdateGauge();
	void Charge();
	void Sound();
	void CannonUpdate();
	void WindUpdate();
	void GoalUpdate();
	void NumberUpdate();
	void ParticleUpdate();
	void DeathUpdate();
	void Mouse();
	void ResetTexture(Vector2 preTileMapOffset, float preTextureSize);

	void CustomizeLoad();

	void Debug();

	void OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) override;

	float LerpShortAngle(float a, float b, float t);
	float LerpShortAngle(float a, float b);

	const Camera* camera_;
	std::unique_ptr<ModelInstance> body_;
	std::unique_ptr<ModelInstance> screw_;
	std::vector< std::unique_ptr<ModelInstance> >barrels_;
	std::vector<std::unique_ptr<ModelInstance>> bones_;
	std::vector<std::unique_ptr<ModelInstance>> sails_;
	std::vector<std::unique_ptr<Tex2DInstance>> sailsIntervalTextures_;
	std::vector<Interval> sailsIntervals_;
	std::vector<std::unique_ptr<ModelInstance>> cannons_;
	std::vector<std::unique_ptr<Tex2DInstance>> cannonsIntervalTextures_;
	std::vector<Interval> cannonIntervals_;
	std::vector<std::unique_ptr<ModelInstance>> loopCannons_;
	std::vector<std::unique_ptr<Tex2DInstance>> loopCannonsIntervalTextures_;
	std::vector<Interval> loopCannonIntervals_;
	// オブジェクトごとの間隔
	Vector2 setObjectDistance_;
	Vector2 preSetObjectDistance_;

	Vector2 cannonShotSize_;
	float cannonShotShrinkTime_;
	Vector2 cannonTextureShotSize_;
	float cannonTextureShotShrinkTime_;

	float magnification_;

	std::vector<std::unique_ptr<Tex2DInstance>> sailNumbers_;
	std::vector<std::unique_ptr<Tex2DInstance>> cannonNumbers_;
	std::vector<std::unique_ptr<Tex2DInstance>> loopCannonNumbers_;

	float numberSize_;

	Texture2D* tex2D_ = nullptr;

	std::unique_ptr<Tex2DInstance> maxPowerGauge_;
	std::unique_ptr<Tex2DInstance> scheme_;
	std::unique_ptr<Tex2DInstance> currentPowerGauge_;
	std::unique_ptr<Tex2DInstance> departure_;
	std::unique_ptr<Tex2DInstance> chargeUI_;
	std::unique_ptr<Tex2DInstance> powerUI_;
	std::unique_ptr<Tex2DInstance> goalText_;
	std::unique_ptr<Tex2DInstance> goText_;
	std::unique_ptr<Tex2DInstance> stageNames_;

	// go
	Easeing goEasing_;

	Vector3 vector_;

	Vector3 prePlayerPos_;

	// 現在の重量
	float weight_;
	// 残りのエネルギー
	float power_;
	// 速度
	float velocity_;

	// 進行方向(Degree)
	float currentAngle_;
	float angleVelocity_;
	// 風のベクトル
	Vector3 windVector_;
	Vector3 windDirection_;
	float windVelocity_;

	// 生きてるか
	bool isAlive_;

	// クリアしたか
	bool isClear_;

	// ゴールしたら
	bool isGoal_;

	float seVolume_ = 0.1f;
	// 進んでる音
	Audio* moveAudioSound_;
	// 弾発射音
	Audio* shotSound_;
	// ほはる音
	Audio* sailSound_;
	// チャージ音
	Audio* startSound_;

	// スタートフラグ
	bool isStart_;

	// チャージ中
	bool isCharge_;
	// チャージする中心点
	Vector2 chargeAnchorPoint_;
	std::unique_ptr<Tex2DInstance> chargeAnchorPointTexture_;

	// 大砲
	Vector3 cannonVector_;
	float cannonVelocity_;


	// チャージ
	float chargeCount_;
	float chargeDuration_;
	float size_;
	Vector3 start_;
	Vector3 end_;
	Vector3 preStick_;

	// タイルマップ
	Vector2 tileMapOffset_;
	std::array<std::array<std::unique_ptr<Tex2DInstance>, width_>, height_> tileMapTexture_;
	std::array<std::array<std::unique_ptr<Tex2DInstance>, width_>, height_> tileMapLuggageTexture_;
	std::vector<std::unique_ptr<Tex2DInstance>> cannonsTextures_;
	std::vector<std::unique_ptr<Tex2DInstance>> loopCannonsTextures_;
	std::vector<std::unique_ptr<Tex2DInstance>> sailsTextures_;
	std::array<std::array<MapData, width_>, height_> tileMap_;
	std::vector<int> sailFlag_;
	int32_t sailCount_;
	int32_t cannonCount_;
	int32_t loopCannonCount_;

	// ゴール
	Goal goal_;
	Vector3 goalPlayerPos_;
	float goalPlayerRotate_;
	float easingCount_;
	Easeing positionEasing_;
	Easeing rotateEasing_;

	// 速度描画
	std::unique_ptr<Tex2DInstance> tensPlace_;
	std::unique_ptr<Tex2DInstance> oneRank_;
	std::unique_ptr<Tex2DInstance> kmTexture_;

	Vector2 velocityTexturePos_;
	float velocityTextureInterval_;

	// カスタマイズ
	bool isCustomize_;
	// カスタマイズシーンに移動する
	bool isToCustomizeScene_;
	// 徐々にゲージを溜めるかフラグ
	bool isAddPower_;
	// 死んだポジション
	bool isDeathAnimation_;
	bool isSetDeathPos_;
	Vector3 deathPosition_;
	Vector3 deathPlayerPosition_;
	Vector3 deathScale_;
	Easeing deathAnimationEasing_;
	float deathAnimationEasingCount_;
	std::unique_ptr<Tex2DInstance> deathTexture_;
#pragma region GlobalVariables

	void SetGlobalVariable();
	void ApplyGlobalVariable();

	std::unique_ptr<GlobalVariables> globalVariables_;
	std::string name_;
	std::array<float, FloatParameter::kFloatParameterCount> floatParameter_;

	std::array<std::string, FloatParameter::kFloatParameterCount> floatParameterItemNames_ = {
		"TurnSpeed",
		"AddAngle",
		"SubtractAngle",
		"Acceleration",
		"Deceleration",
		"WindPower",
		"CannonPower",
		"MaxSpeed",
		"MaxPower",
		"SubtractPower",
		"CannonSubtractPower",
		"MaxCannonVelocity",
		"SubtractCannonVelocity",
		"MaxWindVelocity",
		"SubtractWindVelocity",
		"CustomizeAddPower",
	};
#pragma endregion

#pragma region キクタニが追加しました
	// エミッタのオフセット
	Vector3 emitterOffset_;

	// 2023/02/01追記
	// 大砲が発射されたか
	bool isCannonShot_;

	// 大砲のパーティクル
	std::array<std::unique_ptr<Particle>, 14llu> cannonParticle_;
	decltype(cannonParticle_)::iterator cannonParticleItr_;
#pragma endregion
};