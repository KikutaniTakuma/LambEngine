#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Utils/Easing/Easing.h"
#include "Game/Cursor/Cursor.h"
#include "Game/Skydome/Skydome.h"


class SelectStageScene : public BaseScene {
public:
    SelectStageScene();
    SelectStageScene(const SelectStageScene&) = delete;
    SelectStageScene(SelectStageScene&&) = delete;
    ~SelectStageScene() = default;

    SelectStageScene& operator=(const SelectStageScene&) = delete;
    SelectStageScene& operator=(SelectStageScene&&) = delete;

public:
    void Initialize() override;

    void Finalize() override;

    void Update() override;

    void Draw() override;

private:
    Vector2 ChangeMouseToTexture();
    // どこの海を制する？
    std::unique_ptr<Texture2D> whichSea_;
    // ステージ名の枠
    std::unique_ptr<Texture2D> stageFrame_;
    // ステージ番号
    std::unique_ptr<Texture2D> stageIndexOneNumber_;
    std::unique_ptr<Texture2D> stageIndexTenNumber_;
    // ステージ名
    std::unique_ptr<Texture2D> stageName_;
    // 舟
    //std::unique_ptr<Texture2D> selectShip_;
    // スクリュー
    //std::unique_ptr<Texture2D> selectScrew_;
    // 吹き出し
    std::unique_ptr<Texture2D> speechBubble_;
    // 三角矢印
    std::unique_ptr<Texture2D> left_;
    std::unique_ptr<Texture2D> right_;
    // ステージイメージ画像
    std::unique_ptr<Texture2D> stageImage_;
    // クリアフラグ枠
    std::unique_ptr<Texture2D> clearFrame_;
    // クリアの文字と碇
    std::unique_ptr<Texture2D> clear_;
    // カスタマイズへ
    std::unique_ptr<Texture2D> toCustomize_;
    // カーソル
    std::unique_ptr<Cursor> cursor_;

    // イージング
    // どこの海を制する？
    Easing whichSeaEasing_;
    // 三角矢印
    Easing leftEasing_;
    Easing rightEasing_;

    // 仮で配置
    uint32_t stageIndex_;

    bool isInput_;
    int inputCount_;
    int maxInput_;

    class Audio* stageSelectBGM_;
    class Audio* changeStageSE_;
    class Audio* inCustomizeSE_;


    // 3Dカメラ
    std::unique_ptr<Camera> camera3D_;

    // 水面
    class Water* water_;
    // 舟のモデル
    std::unique_ptr<Model> ship_;
    std::unique_ptr<Model> screw_;

    float screwRotate_;
    float screwRotateSpeed_;

    // お空
    std::unique_ptr<Skydome> skydome_;
    // 雲
    class Cloud* cloud_;
};