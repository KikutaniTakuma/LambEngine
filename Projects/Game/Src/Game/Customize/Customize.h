#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include "AudioManager/AudioManager.h"
#include "Drawer/Model/Model.h"
#include "Drawer/Texture2D/Texture2D.h"
#include "Game/Player/Player.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Math/Mat4x4.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

class Camera;
class Customize {
private:
	enum Type {
		kNone,
		kNotPut,
		kSail,
		kCannon,
		kLoopCannon,
		kCursor,

		kTypeCount,
	};

	struct MapData {
		Type type;
		int32_t index;
	};

	struct Interval {
		std::unique_ptr<Tex2DInstance> left;
		std::unique_ptr<Tex2DInstance> right;
	};
public:
	Customize();

	void Initialize();

	void Update();

	void Draw(const Camera& camera);
	void PostDraw(const Camera& camera);

	void SetPlayer(Player* player) { player_ = player; }

	void SetCamera(Camera* camera) { camera_ = camera; }

	void OnCustomize();
	void OffCustomize();
	void Reset();

	bool IsCustomizeEnd() { return isCustomizeEnd_; }
private:
	static const int width_ = 3;
	static const int height_ = 5;
	void Debug();
	Vector2 ChangeCursorToTileMap();
	Vector2 ChangeCursorToTexturePos();
	void Mouse();
	void SetObject();
	void SetSail();
	void SetCannon(float x, float y);
	void SetLoopCannon(float x, float y);
	void DeleteObject();
	void SetColor();
	void LoadJson(const std::string& stageName);
	void SaveJson(const std::string& stageName);
	void SetMapData(int32_t x, int32_t y, Player::Type type);
	bool IntervalUpdate(int32_t index);

	void DeleteSailIntervalTexture(int32_t index);
	void DeleteCannonIntervalTexture(int32_t index);
	void DeleteLoopCannonIntervalTexture(int32_t index);

	// プレイヤーから借りてくる
	Player* player_;
	Camera* camera_;

	// カスタマイズ自身が持ってる
	std::unique_ptr<Tex2DInstance> customizeBackGroundTexture_;
	std::unique_ptr<Tex2DInstance> customizeBaseTexture_;

	std::unique_ptr<ModelInstance> cursorSail_;
	std::unique_ptr<ModelInstance> cursorBone_;
	std::unique_ptr<ModelInstance> cursorCannon_;
	std::unique_ptr<ModelInstance> cursorLoopCannon_;

	std::unique_ptr<Tex2DInstance> cursorNumbers_;
	std::unique_ptr<Tex2DInstance> cursorSailTexture_;
	std::unique_ptr<Tex2DInstance> cursorCannonTexture_;
	std::unique_ptr<Tex2DInstance> cursorLoopCannonTexture_;

	std::vector<Interval>  sailInterval_;
	std::vector<Interval>  cannonInterval_;
	std::vector<Interval>  loopCannonInterval_;
	float intervalScale_ = 25.0f;
	float intervalTextureDistance_ = 35.0f;


	uint32_t interval_;

	std::unique_ptr<Tex2DInstance> cursorTexture_;

	Vector2 tileMapOffset_;
	Vector2 cursor_;
	float angle_;

	Type currentType_;

	std::string fileName_;

	bool isInput_;
	int inputCount_;
	int maxInput_;

	bool isTriggerInput_;
	int triggerInputCount_;
	int triggerMaxInput_;

	float decisionSize_;

	Vector4 setColor_;
	Vector4 setIntervalTextureColor_;

	Vector2 explanationTexturePos_;
	Vector2 explanationTextureSize_;
	std::unique_ptr<Tex2DInstance> explanationCursorTexture_;
	std::unique_ptr<Tex2DInstance> explanationCannonTexture_;
	std::unique_ptr<Tex2DInstance> explanationLoopCannonTexture_;
	std::unique_ptr<Tex2DInstance> explanationSailTexture_;

	std::unique_ptr<Tex2DInstance> startTexture_;
	std::unique_ptr<Tex2DInstance> gearTexture_;
	std::unique_ptr<Tex2DInstance> toStageSelectTexture_;

	std::unique_ptr<Tex2DInstance> maxSailCountTexture_;
	std::unique_ptr<Tex2DInstance> maxCannonCountTexture_;
	std::unique_ptr<Tex2DInstance> maxLoopCannonCountTexture_;

	Vector2 maxCountTexturePos_;
	Vector2 maxCountTextureScale_;
	float maxCountTextureDistance_;

	// ループするか
	bool isLoop_;
	// ステージごとのMaxオブジェクト数
	int32_t maxSailCount_;
	int32_t maxCannonCount_;
	int32_t maxLoopCannonCount_;

	// 
	float setObjectScale_;

	// SE
	float seVolume_ = 0.2f;
	Audio* pushA_;
	Audio* pushB_;
	Audio* pushY_;
	Audio* pushLBRB_;
	Audio* pushLTRT_;
	Audio* pushStart_;
	Audio* pushSelectStage_;
	Audio* moveTile_;

	int stageIndex_;
	std::string loadString_;
	std::string saveString_;

	// カスタマイズ終了
	bool isCustomizeEnd_;
};