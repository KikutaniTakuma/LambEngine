//#pragma once
//#include "Drawers/Texture2D/Texture2D.h"
//#include "Utils/Camera/Camera.h"
//#include "Drawers/PeraRender/PeraRender.h"
//#include "Drawers/Line/Line.h"
//#include "Utils/Flg/Flg.h"
//
//#include <memory>
//#include <unordered_map>
//#include <deque>
//#include <array>
//class Minimap final {
//private:
//	struct ObjectData {
//		Vector3 scale;
//		Vector3 rotate;
//		Vector3 pos;
//	};
//
//public:
//	enum Type {
//		kBlock,
//		kGoal,
//		kPlayer,
//		kWind,
//		kWhirpool,
//		kDeathPoint,
//		kBeach,
//
//		kNum
//	};
//
//public:
//	Minimap() = default;
//	Minimap(const Minimap&) = delete;
//	Minimap(Minimap&&) = delete;
//	~Minimap() = default;
//
//	Minimap& operator=(const Minimap&) = delete;
//	Minimap& operator=(Minimap&&) = delete;
//
//public:
//	void Initialize(const Vector3& playerPos, const Vector3& goalPos);
//
//	void Finalize();
//
//	// 更新
//	void Update();
//
//	// 描画
//	void Draw(const Vector3& playerPos);
//
//	void Debug();
//
//public:
//	// プレイヤーのポジションをセット
//	void SetPlayerState(const Vector3& scale, const Vector3& rotate, const Vector3& pos);
//
//	// プレイヤーが死んだ位置を保存
//	void SetPlayerDeathPos(const Vector3& deathPos);
//
//	// 内部的に定期的に追加していく
//	void AddPlayerPos(const Vector3& pos);
//
//	// 移動した同線を新しくする(保存は直近3回まで)
//	void NewMoveHistory();
//
//	// カスタマイズモードとインゲーム中のラインの描画を変えるために必要
//	void SwitchCustomizeAndInGame();
//
//	// プレイヤーの初期位置とゴールの位置で初期のカメラとズームを調整
//	void AdjustCameraPos(const Vector3& playerPos, const Vector3& goalPos);
//
//private:
//	// ロードしたデータを元にテクスチャのインスタンスを作る
//	void CreateTex2D();
//
//	// ブロックの情報取得
//	void LoadBlockData();
//
//	// ゴールの情報を取得
//	void LoadGoalData();
//
//	// 渦潮の情報を取得
//	void LoadWhirlpool();
//
//	// 風の情報を取得
//	void LoadWind();
//
//
//	float GetPreToCurrentLength(const Vector3& playerPos);
//
//	void StopAddPlayerPos();
//
//private:
//	// カメラの移動
//	void CameraMove();
//
//public:
//	Vector3 miniMapPos;
//	Vector3 miniMapScale;
//
//private:
//	std::unique_ptr<PeraRender> pera_;
//	Vector2 peraScale_;
//	Vector3 scaleBasis_;
//
//	Vector3 playerFirstPos_;
//	Vector3 firstCameraPos_;
//
//	Vector3 miniMapScaleCusomize_;
//	Vector3 miniMapScaleInGame_;
//	Vector3 miniMapPosCusomize_;
//	Vector3 miniMapPosInGame_;
//
//
//	float objectScale_;
//	float playerScale_;
//
//	// 移動最大
//	Vector2 cameraMaxPos;
//	// 移動最小
//	Vector2 cameraMinPos;
//
//	float maxZoom_;
//	float minZoom_;
//	float zoomSpeed_;
//
//
//	// ローカルカメラ
//	std::unique_ptr<Camera> localCamera_;
//	std::unique_ptr<Camera> staticCamera_;
//
//	float cameraMoveSpeed_;
//
//	// ブロック用
//	std::vector<std::unique_ptr<Texture2D>> blockTextures_;
//	// 渦潮用
//	std::vector<std::unique_ptr<Texture2D>> whirpoolTextures_;
//	// 風用
//	std::vector<std::unique_ptr<Texture2D>> windTextures_;
//	// プレイヤー用
//	std::unique_ptr<Texture2D> playerTex_;
//	// ゴール用
//	std::unique_ptr<Texture2D> goalTex_;
//	std::unique_ptr<Texture2D> rightBeachTex_;
//	std::unique_ptr<Texture2D> leftBeachTex_;
//
//
//	// ミニマップの背景リソース
//	std::unique_ptr<Texture2D> minimapBackGround_;
//
//	// ミニマップ表示用の色
//	std::array<Vector4, Type::kNum> colors_;
//	// ミニマップのテクスチャ
//	std::array<std::string, Type::kNum> texturePathes_;
//
//	// ブロックのデータ
//	std::deque<ObjectData> blockData_;
//	// 渦潮のデータ
//	std::deque<ObjectData> whirpoolData_;
//	// 風のデータ
//	std::deque<ObjectData> windData_;
//	// プレイヤーのデータ
//	ObjectData playerData_;
//	// ゴールのデータ
//	ObjectData goalData_;
//
//	// 死んだ位置表示用のテクスチャ
//	std::array<std::unique_ptr<Texture2D>, 3llu> deathPosTextures_;
//	// 死んだ位置を保存するコンテナ
//	std::array<Vector3, 3llu> deathPositions_;
//	std::array<bool, 3llu> isDeathPositionsDraw_;
//
//	// プレイヤーが動いた軌跡を保存するコンテナ(過去3回まで)
//	std::array<std::vector<Vector3>, 3llu> playerMoveHistory_;
//	// 表示する軌跡
//	int32_t currentPlayerMoveHistoryIndex_;
//	// 保存する軌跡の要素数
//	int32_t savePlayerMoveHistoryIndex_;
//
//	// 軌跡表示用
//	Line::LineDrawPack line_;
//	// ポジションを追加する基準の進んだ長さ
//	float addPosLengthBasis_;
//
//
//	// 風のテクスチャを1秒間で移動させる速度
//	float windAnimationSpeed_;
//
//	// 渦潮のテクスチャを1秒間で回転させる速度
//	float whirpoolRotateSpeed_;
//
//	// カスタマイズか
//	Lamb::Flg isCustomize_;
//
//	// グリッド用テクスチャ
//	std::unique_ptr<Texture2D> gridTexture_;
//
//	bool isStop_;
//};