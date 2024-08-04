#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Utils/Easeing.h"
#include "Game/Cursor/Cursor.h"

#include "Drawers/Other/WaterTex2D/WaterTex2D.h"


class SelectStageScene : public BaseScene {
public:
    SelectStageScene();
    SelectStageScene(const SelectStageScene&) = delete;
    SelectStageScene(SelectStageScene&&) = delete;
    ~SelectStageScene() = default;

    SelectStageScene& operator=(const SelectStageScene&) = delete;
    SelectStageScene& operator=(SelectStageScene&&) = delete;

public:
    void Load() override;

    void Initialize() override;

    void Finalize() override;

    void Update() override;

    void Draw() override;

private:
    Vector2 ChangeMouseToTexture();
    // どこの海を制する？
    std::unique_ptr<Tex2DInstance> whichSea_;
    // ステージ名の枠
    std::unique_ptr<Tex2DInstance> stageFrame_;
    // ステージ番号
    std::unique_ptr<Tex2DInstance> stageIndexOneNumber_;
    std::unique_ptr<Tex2DInstance> stageIndexTenNumber_;
    // ステージ名
    std::unique_ptr<Tex2DInstance> stageName_;
    // 舟
    //std::unique_ptr<Tex2DInstance> selectShip_;
    // スクリュー
    //std::unique_ptr<Tex2DInstance> selectScrew_;
    // 吹き出し
    std::unique_ptr<Tex2DInstance> speechBubble_;
    // 三角矢印
    std::unique_ptr<Tex2DInstance> left_;
    std::unique_ptr<Tex2DInstance> right_;
    // ステージイメージ画像
    std::unique_ptr<Tex2DInstance> stageImage_;
    // クリアフラグ枠
    std::unique_ptr<Tex2DInstance> clearFrame_;
    // クリアの文字と碇
    std::unique_ptr<Tex2DInstance> clear_;
    // カスタマイズへ
    std::unique_ptr<Tex2DInstance> toCustomize_;
    // カーソル
    std::unique_ptr<Cursor> cursor_;

    // イージング
    // どこの海を制する？
    Easeing whichSeaEasing_;
    // 三角矢印
    Easeing leftEasing_;
    Easeing rightEasing_;

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
    std::unique_ptr<ModelInstance> ship_;
    std::unique_ptr<ModelInstance> screw_;

    float screwRotate_;
    float screwRotateSpeed_;

    WaterTex2D::WaveData waveData_;

};