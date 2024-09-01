#include "SelectStageScene.h"

#include "../Game/Src/Game/StageManager/StageManager.h"

#include "imgui.h"

#include "AudioManager/AudioManager.h"

#include "Game/Water/Water.h"
#include <numbers>

#include "Utils/EngineInfo.h"

SelectStageScene::SelectStageScene() : BaseScene(BaseScene::ID::StageSelect) {}

void SelectStageScene::Load()
{
}

void SelectStageScene::Initialize() {

    whichSea_ = std::make_unique<Tex2DInstance>();
    whichSea_->Load("./Resources/OutGame/whichSea.png");
    whichSea_->scale = { 720.0f, 160.0f };
    whichSea_->pos = { 0.0f, 280.0f, 0.0f };

    stageFrame_= std::make_unique<Tex2DInstance>();
    stageFrame_->Load("./Resources/OutGame/stageFrame.png");
    stageFrame_->scale = { 350.0f, 130.0f };
    stageFrame_->pos = { -465.0f, 100.0f, 0.0f };

    stageIndexOneNumber_ = std::make_unique<Tex2DInstance>();
    stageIndexOneNumber_->Load("./Resources/number.png");
    stageIndexOneNumber_->scale = { 26.0f,26.0f };
    stageIndexOneNumber_->uvSize = { 0.1f,1.0f };
    stageIndexOneNumber_->pos = { -545.0f, 142.0f, 0.0f };

    stageIndexTenNumber_ = std::make_unique<Tex2DInstance>();
    stageIndexTenNumber_->Load("./Resources/number.png");
    stageIndexTenNumber_->scale = stageIndexOneNumber_->scale;
    stageIndexTenNumber_->uvSize = { 0.1f,1.0f };
    stageIndexTenNumber_->pos = { -545.0f, 142.0f, 0.0f };

    stageName_ = std::make_unique<Tex2DInstance>();
    stageName_->Load("./Resources/OutGame/stageNameBig.png");
    stageName_->scale = { 250.0f, 50.0f };
    stageName_->uvSize = { 1.0f / 12.0f, 1.0f };
    stageName_->pos = { -490.0f, 85.0f, 0.0f };
    stageName_->color = Vector4ToUint({0.0f, 0.0f, 0.0f, 1.0f });

   /* selectShip_ = std::make_unique<Tex2DInstance>();
    selectShip_->Load("./Resources/OutGame/selectShip.png");
    selectShip_->scale = { 300.0f, 220.0f };
    selectShip_->pos = { -320.0f, -135.0f, 0.0f };*/

   /* selectScrew_ = std::make_unique<Tex2DInstance>();
    selectScrew_->Load("./Resources/OutGame/selectScrew.png");
    selectScrew_->scale = { 20.0f, 100.0f };
    selectScrew_->pos = { -550.0f, -235.0f, 0.0f };*/

    speechBubble_ = std::make_unique<Tex2DInstance>();
    speechBubble_->Load("./Resources/OutGame/speechBubble.png");
    speechBubble_->scale = { 600.0f, 420.0f };
    speechBubble_->pos = { 230.0f, 0.0f, 0.0f };

    left_ = std::make_unique<Tex2DInstance>();
    left_->Load("./Resources/OutGame/triangle.png");
    left_->scale = { 55.0f, 55.0f };
    left_->pos = { -20.0f, 0.0f, 0.0f };
    leftEasing_.Start(false, 0.1f, Easeing::Type::kOutSine);
    right_ = std::make_unique<Tex2DInstance>();
    right_->Load("./Resources/OutGame/triangle.png");
    right_->scale = { 55.0f, 55.0f };
    right_->pos = { 570.0f, 0.0f, 0.0f };
    right_->rotate.z = 3.14f;
    rightEasing_.Start(false, 0.1f, Easeing::Type::kOutSine);

    stageImage_ = std::make_unique<Tex2DInstance>();
    stageImage_->Load("./Resources/OutGame/stageImage/stageImage.png");
    stageImage_->scale = { 340.0f, 250.0f };
    stageImage_->uvSize = { 1.0f / 12.0f, 1.0f };
    stageImage_->pos = { 280.0f, 1.0f, 0.0f };

    clearFrame_ = std::make_unique<Tex2DInstance>();
    clearFrame_->Load("./Resources/OutGame/clearFrame.png");
    clearFrame_->scale = { 110.0f, 110.0f };
    clearFrame_->pos = { 465.0f, 135.0f, 0.0f };

    clear_ = std::make_unique<Tex2DInstance>();
    clear_->Load("./Resources/OutGame/clear.png");
    clear_->scale = { 120.0f, 150.0f };
    clear_->pos = { 467.0f, 163.0f, 0.0f };

    toCustomize_ = std::make_unique<Tex2DInstance>();
    toCustomize_->Load("./Resources/OutGame/toCustomize.png");
    toCustomize_->scale = { 360.0f, 127.0f };
    toCustomize_->pos = { 460.0f, -307.5f, 0.0f };

    stageIndex_ = StageManager::GetStage();

    maxInput_ = 15;
    isInput_ = true;
    inputCount_ = 0;

    audioManager_->Load("./Resources/Sound/stageSelect.wav");
    stageSelectBGM_ = audioManager_->Get("./Resources/Sound/stageSelect.wav");
    stageSelectBGM_->Start(0.1f, true);

    audioManager_->Load("./Resources/Sound/changeSE.wav");
    audioManager_->Load("./Resources/Sound/toCustomizeSE.wav");

    changeStageSE_ = audioManager_->Get("./Resources/Sound/changeSE.wav");
    inCustomizeSE_ = audioManager_->Get("./Resources/Sound/toCustomizeSE.wav");



    camera3D_ = std::make_unique<Camera>();

    camera3D_->pos.y = 3.6f;
    camera3D_->rotate.y = -0.17f;


    // 水のインスタンス追加
    water_ = Water::GetInstance();

    ship_.reset(new ModelInstance());
    ship_->Load("./Resources/Player/titlePlayer.obj");
    ship_->pos.x = -9.03f;
    ship_->pos.z = 23.37f;
    ship_->rotate.y = 1.401f;
    screw_.reset(new ModelInstance());
    screw_->Load("./Resources/Player/screw.obj");
    screw_->SetParent(ship_.get());
    screw_->pos.z = -3.0f;
    screwRotate_ = 0.0f;
    screwRotateSpeed_ = std::numbers::pi_v<float>;

    cursor_ = std::make_unique<Cursor>();
    cursor_->Initialize();

    currentCamera_->pos.z = -10.0f;

    renderingManager_->SetTime(7.0f);
}

void SelectStageScene::Finalize() {
    stageSelectBGM_->Stop();
}

void SelectStageScene::Update() {
    cursor_->Update();

    auto mouse = Input::GetInstance()->GetMouse();
    currentCamera_->Update();
    
#pragma region 2024/03/19 キクタニ追記
    camera3D_->Debug("3DCamera");
    camera3D_->Update();

    ship_->Update();

    screwRotate_ += screwRotateSpeed_ * Lamb::DeltaTime();

    screw_->rotate.x = screwRotate_;

    screw_->Update();

    waveData_.ripplesPoint = ship_->pos;
    waveData_.time += Lamb::DeltaTime();
    waveData_.waveStrength = 0.2f;
    waveData_.ripples = 10.0f;
    waveData_.waveSpeed = 2.0f;
    waveData_.timeAttenuation = 0.0f;

    water_->Update(currentCamera_->GetPos());
    water_->SetWaveData(waveData_);
#pragma endregion



    // スクリュー回転
    //selectScrew_->rotate.z += 0.065f;

    // どこ海イージング
    if (!whichSeaEasing_.GetIsActive()) {
        whichSeaEasing_.Start(false, 0.5f, Easeing::Type::kOutSine);
    }

    // イージングUpdate
    whichSeaEasing_.Update();
    whichSea_->scale = whichSeaEasing_.Get({ 720.0f * 1.05f, 160.0f * 1.05f }, { 720.0f, 160.0f });
    leftEasing_.Update();
    left_->scale = leftEasing_.Get({ 82.5f, 82.5f }, { 55.0f, 55.0f });
    rightEasing_.Update();
    right_->scale = rightEasing_.Get({ 82.5f, 82.5f }, { 55.0f, 55.0f });

    // ステージカウント
    {
        stageName_->uvPibot = { float(stageIndex_) * (1.0f / 12.0f), 1.0f};
        stageImage_->uvPibot = { float(stageIndex_) * (1.0f / 12.0f), 1.0f};

        if (stageIndex_ + 1 >= 10) {
            float ten = floor(float(int(float(stageIndex_ + 1) / 10.0f)));
            float one = floor(std::fmod(float(stageIndex_ + 1), 10.0f));

            stageIndexTenNumber_->uvPibot = { ten * 0.1f, 1.0f };
            stageIndexOneNumber_->uvPibot = { one * 0.1f, 1.0f };

            stageIndexOneNumber_->pos = { -530.0f, 142.0f, 0.0f };

        }
        else {
            float one = floor(std::fmod(float(stageIndex_ + 1), 10.0f));

            stageIndexTenNumber_->uvPibot = { 0.0f * 0.1f, 1.0f };
            stageIndexOneNumber_->uvPibot = { one * 0.1f, 1.0f };

            stageIndexOneNumber_->pos = { -545.0f, 142.0f, 0.0f };
        }
    }
    // ステージセレクト
    {
        auto gamepad = Input::GetInstance()->GetGamepad();
        if (isInput_ &&
            (std::fabs(gamepad->GetStick(Gamepad::Stick::LEFT) != Vector2::kZero))) {
            changeStageSE_->Start(0.05f, false);
            isInput_ = false;
            inputCount_ = 0;
            // Xの方がでかい
            if (std::fabs(gamepad->GetStick(Gamepad::Stick::LEFT).x) > std::fabs(gamepad->GetStick(Gamepad::Stick::LEFT).y)) {
                if (gamepad->GetStick(Gamepad::Stick::LEFT).x > 0.0f) {
                    rightEasing_.Start(false, 0.2f, Easeing::Type::kOutSine);
                    if (stageIndex_ >= StageManager::kMaxStageCount - 1) {
                        stageIndex_ = 0;
                    }
                    else {
                        stageIndex_++;
                    }
                }
                else {
                    leftEasing_.Start(false, 0.2f, Easeing::Type::kOutSine);
                    if (stageIndex_ <= 0) {
                        stageIndex_ = StageManager::kMaxStageCount - 1;
                    }
                    else {
                        stageIndex_--;
                    }
                }
            }
        }
        if (mouse->Pushed(Mouse::Button::Left) &&
            right_->Collision(ChangeMouseToTexture())) {
            rightEasing_.Start(false, 0.2f, Easeing::Type::kOutSine);
            changeStageSE_->Start(0.05f, false);
            if (stageIndex_ >= StageManager::kMaxStageCount - 1) {
                stageIndex_ = 0;
            }
            else {
                stageIndex_++;
            }
        }
        if (mouse->Pushed(Mouse::Button::Left) &&
            left_->Collision(ChangeMouseToTexture())) {
            changeStageSE_->Start(0.05f, false);
            leftEasing_.Start(false, 0.2f, Easeing::Type::kOutSine);
            if (stageIndex_ <= 0) {
                stageIndex_ = StageManager::kMaxStageCount - 1;
            }
            else {
                stageIndex_--;
            }
        }
        // カウントリセット
        {
            if (!isInput_) {
                inputCount_++;
                if (inputCount_ >= maxInput_) {
                    isInput_ = true;
                    inputCount_ = 0;
                }
            }
            if (gamepad->GetStick(Gamepad::Stick::LEFT) == Vector2::kZero) {
                isInput_ = true;
                inputCount_ = 0;
            }
        }
    }
    auto pos = ChangeMouseToTexture();
    pos;
    if (mouse->Pushed(Mouse::Button::Left) &&
        toCustomize_->Collision(ChangeMouseToTexture())) {
        SceneManager::GetInstance()->SceneChange(BaseScene::ID::Game);
        StageManager::SetStage(stageIndex_);
        inCustomizeSE_->Start(0.5f, false);
    }

#ifdef _DEBUG
    ImGui::Begin("StageCount");
    ImGui::Text("Stage:%d", stageIndex_);
    ImGui::End();

    Input::GetInstance()->GetMouse()->Debug();
    ImGui::Begin("UIPosition");
    ImGui::Text("MousePos x:%f,y:%f",ChangeMouseToTexture().x, ChangeMouseToTexture().y);
    ImGui::DragFloat3("stageImage", &stageImage_->pos.x, 1.0f);
    ImGui::End();
#endif // _DEBUG

    renderingManager_->SetCameraPos(currentCamera_->GetPos());
    renderingManager_->SetCameraMatrix(currentCamera_->GetViewProjection());
    renderingManager_->SetProjectionInverseMatrix(currentCamera_->GetProjection().Inverse());
}

void SelectStageScene::Draw() {
#pragma region 2024/03/19 キクタニ追記
    water_->Draw(camera3D_->GetViewProjection());
    ship_->Draw(camera3D_->GetViewProjection());
    screw_->Draw(camera3D_->GetViewProjection());
#pragma endregion

    whichSea_->blend = BlendType::kUnenableDepthNormal;
    whichSea_->Draw(currentCamera_->GetViewOthographics());
    stageFrame_->blend = BlendType::kUnenableDepthNormal;
    stageFrame_->Draw(currentCamera_->GetViewOthographics());
    stageName_->blend = BlendType::kUnenableDepthNormal;
    stageName_->Draw(currentCamera_->GetViewOthographics());
    speechBubble_->blend = BlendType::kUnenableDepthNormal;
    speechBubble_->Draw(currentCamera_->GetViewOthographics());
    left_->blend = BlendType::kUnenableDepthNormal;
    left_->Draw(currentCamera_->GetViewOthographics());
    right_->blend = BlendType::kUnenableDepthNormal;
    right_->Draw(currentCamera_->GetViewOthographics());
    stageImage_->blend = BlendType::kUnenableDepthNormal;
    stageImage_->Draw(currentCamera_->GetViewOthographics());
    clearFrame_->blend = BlendType::kUnenableDepthNormal;
    clearFrame_->Draw(currentCamera_->GetViewOthographics());
    if (StageManager::GetIsGoal(stageIndex_)) {
        clear_->blend = BlendType::kUnenableDepthNormal;
        clear_->Draw(currentCamera_->GetViewOthographics());
    }
    toCustomize_->blend = BlendType::kUnenableDepthNormal;
    toCustomize_->Draw(currentCamera_->GetViewOthographics());

    stageIndexOneNumber_->blend = BlendType::kUnenableDepthNormal;
    stageIndexOneNumber_->Draw(currentCamera_->GetViewOthographics());
    if (stageIndexTenNumber_->uvPibot.x != 0.0f) {
        stageIndexTenNumber_->blend = BlendType::kUnenableDepthNormal;
        stageIndexTenNumber_->Draw(currentCamera_->GetViewOthographics());
    }

    cursor_->Draw(*currentCamera_);
}

Vector2 SelectStageScene::ChangeMouseToTexture() {
    auto mouse = Input::GetInstance()->GetMouse();
    Vector3 texPos = Vector3(mouse->GetPos(), 0.0f) * (Mat4x4::MakeTranslate(Vector3::kZIdentity * 10.0f) * Mat4x4::MakeOrthographic(1280.0f, 720.0f, 0.1f, 1000.0f) * Mat4x4::MakeViewPort(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f)).Inverse();
    return { texPos.x, texPos.y };
}
