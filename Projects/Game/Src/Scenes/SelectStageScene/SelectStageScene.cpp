#include "SelectStageScene.h"

#include "../Game/Src/Game/StageManager/StageManager.h"

#include "imgui.h"

#include "AudioManager/AudioManager.h"

#include "Game/Water/Water.h"
#include <numbers>

#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"

SelectStageScene::SelectStageScene() : BaseScene(BaseScene::ID::StageSelect) {}

void SelectStageScene::Initialize() {

    whichSea_ = std::make_unique<Texture2D>();
    whichSea_->LoadTexture("./Resources/OutGame/whichSea.png");
    whichSea_->scale = { 720.0f, 160.0f };
    whichSea_->pos = { 0.0f, 280.0f, 0.0f };

    stageFrame_= std::make_unique<Texture2D>();
    stageFrame_->LoadTexture("./Resources/OutGame/stageFrame.png");
    stageFrame_->scale = { 350.0f, 130.0f };
    stageFrame_->pos = { -465.0f, 100.0f, 0.0f };

    stageIndexOneNumber_ = std::make_unique<Texture2D>();
    stageIndexOneNumber_->LoadTexture("./Resources/number.png");
    stageIndexOneNumber_->scale = { 26.0f,26.0f };
    stageIndexOneNumber_->uvSize = { 0.1f,1.0f };
    stageIndexOneNumber_->pos = { -545.0f, 142.0f, 0.0f };

    stageIndexTenNumber_ = std::make_unique<Texture2D>();
    stageIndexTenNumber_->LoadTexture("./Resources/number.png");
    stageIndexTenNumber_->scale = stageIndexOneNumber_->scale;
    stageIndexTenNumber_->uvSize = { 0.1f,1.0f };
    stageIndexTenNumber_->pos = { -545.0f, 142.0f, 0.0f };

    stageName_ = std::make_unique<Texture2D>();
    stageName_->LoadTexture("./Resources/OutGame/stageNameBig.png");
    stageName_->scale = { 250.0f, 50.0f };
    stageName_->uvSize = { 1.0f / 12.0f, 1.0f };
    stageName_->pos = { -490.0f, 85.0f, 0.0f };
    stageName_->color = Vector4ToUint({0.0f, 0.0f, 0.0f, 1.0f });

   /* selectShip_ = std::make_unique<Texture2D>();
    selectShip_->LoadTexture("./Resources/OutGame/selectShip.png");
    selectShip_->scale = { 300.0f, 220.0f };
    selectShip_->pos = { -320.0f, -135.0f, 0.0f };*/

   /* selectScrew_ = std::make_unique<Texture2D>();
    selectScrew_->LoadTexture("./Resources/OutGame/selectScrew.png");
    selectScrew_->scale = { 20.0f, 100.0f };
    selectScrew_->pos = { -550.0f, -235.0f, 0.0f };*/

    speechBubble_ = std::make_unique<Texture2D>();
    speechBubble_->LoadTexture("./Resources/OutGame/speechBubble.png");
    speechBubble_->scale = { 600.0f, 420.0f };
    speechBubble_->pos = { 230.0f, 0.0f, 0.0f };

    left_ = std::make_unique<Texture2D>();
    left_->LoadTexture("./Resources/OutGame/triangle.png");
    left_->scale = { 55.0f, 55.0f };
    left_->pos = { -20.0f, 0.0f, 0.0f };
    leftEasing_.Start(false, 0.1f, Easing::OutSine);
    right_ = std::make_unique<Texture2D>();
    right_->LoadTexture("./Resources/OutGame/triangle.png");
    right_->scale = { 55.0f, 55.0f };
    right_->pos = { 570.0f, 0.0f, 0.0f };
    right_->rotate.z = 3.14f;
    rightEasing_.Start(false, 0.1f, Easing::OutSine);

    stageImage_ = std::make_unique<Texture2D>();
    stageImage_->LoadTexture("./Resources/OutGame/stageImage/stageImage.png");
    stageImage_->scale = { 340.0f, 250.0f };
    stageImage_->uvSize = { 1.0f / 12.0f, 1.0f };
    stageImage_->pos = { 280.0f, 1.0f, 0.0f };

    clearFrame_ = std::make_unique<Texture2D>();
    clearFrame_->LoadTexture("./Resources/OutGame/clearFrame.png");
    clearFrame_->scale = { 110.0f, 110.0f };
    clearFrame_->pos = { 465.0f, 135.0f, 0.0f };

    clear_ = std::make_unique<Texture2D>();
    clear_->LoadTexture("./Resources/OutGame/clear.png");
    clear_->scale = { 120.0f, 150.0f };
    clear_->pos = { 467.0f, 163.0f, 0.0f };

    toCustomize_ = std::make_unique<Texture2D>();
    toCustomize_->LoadTexture("./Resources/OutGame/toCustomize.png");
    toCustomize_->scale = { 360.0f, 127.0f };
    toCustomize_->pos = { 460.0f, -307.5f, 0.0f };

    stageIndex_ = StageManager::GetStage();

    maxInput_ = 15;
    isInput_ = true;
    inputCount_ = 0;

    stageSelectBGM_ = audioManager_->LoadWav("./Resources/Sound/stageSelect.wav", true);
    stageSelectBGM_->Start(0.1f);

    changeStageSE_= audioManager_->LoadWav("./Resources/Sound/changeSE.wav", false);
    inCustomizeSE_= audioManager_->LoadWav("./Resources/Sound/toCustomizeSE.wav", false);



    camera3D_ = std::make_unique<Camera>();

    camera3D_->pos.y = 3.6f;
    camera3D_->rotate.y = -0.17f;


    // 水のインスタンス追加
    water_ = Water::GetInstance();

    ship_.reset(new Model{ "./Resources/Player/titlePlayer.obj" });
    ship_->pos.x = -9.03f;
    ship_->pos.z = 23.37f;
    ship_->rotate.y = 1.401f;
    screw_.reset(new Model{ "./Resources/Player/screw.obj" });
    screw_->SetParent(ship_.get());
    screw_->pos.z = -3.0f;
    screwRotate_ = 0.0f;
    screwRotateSpeed_ = std::numbers::pi_v<float>;

    skydome_ = std::make_unique<Skydome>();
    skydome_->Initialize();

    cloud_ = Cloud::GetInstance();
    skydome_->SetTexture(cloud_->GetTex());

    cursor_ = std::make_unique<Cursor>();
    cursor_->Initialize();
}

void SelectStageScene::Finalize() {
    stageSelectBGM_->Stop();
}

void SelectStageScene::Update() {
    cursor_->Update();

    auto mouse = Input::GetInstance()->GetMouse();
    camera_->Update();
    
#pragma region 2024/03/19 キクタニ追記
    camera3D_->Debug("3DCamera");
    camera3D_->Update();

    ship_->Debug("Ship");
    ship_->Update();

    screwRotate_ += screwRotateSpeed_ * Lamb::DeltaTime();

    screw_->rotate.x = screwRotate_;

    screw_->Update();

    water_->Update(camera3D_->GetPos());

    skydome_->Update();
    cloud_->Update();
#pragma endregion



    // スクリュー回転
    //selectScrew_->rotate.z += 0.065f;

    // どこ海イージング
    if (!whichSeaEasing_.GetIsActive()) {
        whichSeaEasing_.Start(false, 0.5f, Easing::OutSine);
    }

    // イージングUpdate
    whichSeaEasing_.Update();
    whichSea_->scale = whichSeaEasing_.Get({ 720.0f * 1.05f, 160.0f * 1.05f }, { 720.0f, 160.0f });
    leftEasing_.Update();
    left_->scale = leftEasing_.Get({ 82.5f, 82.5f }, { 55.0f, 55.0f });
    rightEasing_.Update();
    right_->scale = rightEasing_.Get({ 82.5f, 82.5f }, { 55.0f, 55.0f });

    whichSea_->Update();
    stageFrame_->Update();
    //selectShip_->Update();
    //selectScrew_->Update();
    speechBubble_->Update();
    left_->Update();
    right_->Update();
    clearFrame_->Update();
    clear_->Update();
    toCustomize_->Update();

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
    stageName_->Update();
    stageImage_->Update();
    stageIndexTenNumber_->Update();
    stageIndexOneNumber_->Update();
    // ステージセレクト
    {
        auto gamepad = Input::GetInstance()->GetGamepad();
        if (isInput_ &&
            (std::fabs(gamepad->GetStick(Gamepad::Stick::LEFT_X) != 0.0f) ||
                std::fabs(gamepad->GetStick(Gamepad::Stick::LEFT_Y) != 0.0f))) {
            changeStageSE_->Start(0.05f);
            isInput_ = false;
            inputCount_ = 0;
            // Xの方がでかい
            if (std::fabs(gamepad->GetStick(Gamepad::Stick::LEFT_X)) > std::fabs(gamepad->GetStick(Gamepad::Stick::LEFT_Y))) {
                if (gamepad->GetStick(Gamepad::Stick::LEFT_X) > 0.0f) {
                    rightEasing_.Start(false, 0.2f, Easing::OutSine);
                    if (stageIndex_ >= StageManager::kMaxStageCount - 1) {
                        stageIndex_ = 0;
                    }
                    else {
                        stageIndex_++;
                    }
                }
                else {
                    leftEasing_.Start(false, 0.2f, Easing::OutSine);
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
            rightEasing_.Start(false, 0.2f, Easing::OutSine);
            changeStageSE_->Start(0.05f);
            if (stageIndex_ >= StageManager::kMaxStageCount - 1) {
                stageIndex_ = 0;
            }
            else {
                stageIndex_++;
            }
        }
        if (mouse->Pushed(Mouse::Button::Left) &&
            left_->Collision(ChangeMouseToTexture())) {
            changeStageSE_->Start(0.05f);
            leftEasing_.Start(false, 0.2f, Easing::OutSine);
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
            if (gamepad->GetStick(Gamepad::Stick::LEFT_X) == 0.0f &&
                gamepad->GetStick(Gamepad::Stick::LEFT_Y) == 0.0f) {
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
        inCustomizeSE_->Start(0.5f);
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

   
}

void SelectStageScene::Draw() {
#pragma region 2024/03/19 キクタニ追記
    cloud_->Draw();
    skydome_->Draw(*camera_);
    water_->Draw(camera3D_->GetViewProjection());
    ship_->Draw(camera3D_->GetViewProjection(), camera3D_->GetPos());
    screw_->Draw(camera3D_->GetViewProjection(), camera3D_->GetPos());
#pragma endregion

    //stageSelectBackGround_->Draw(camera_->GetOthographics());
    whichSea_->Draw(camera_->GetOthographics());
    stageFrame_->Draw(camera_->GetOthographics());
    stageName_->Draw(camera_->GetOthographics());
    //selectShip_->Draw(camera_->GetOthographics());
   // selectScrew_->Draw(camera_->GetOthographics());
    speechBubble_->Draw(camera_->GetOthographics());
    left_->Draw(camera_->GetOthographics());
    right_->Draw(camera_->GetOthographics());
    stageImage_->Draw(camera_->GetOthographics());
    clearFrame_->Draw(camera_->GetOthographics());
    if (StageManager::GetIsGoal(stageIndex_)) {
        clear_->Draw(camera_->GetOthographics());
    }
    toCustomize_->Draw(camera_->GetOthographics());

    stageIndexOneNumber_->Draw(camera_->GetOthographics());
    if (stageIndexTenNumber_->uvPibot.x != 0.0f) {
        stageIndexTenNumber_->Draw(camera_->GetOthographics());
    }

    cursor_->Draw(*camera_);
}

Vector2 SelectStageScene::ChangeMouseToTexture() {
    Vector2 result = Input::GetInstance()->GetMouse()->GetPos();
    return { result.x - 1280.0f * 0.5f, (result.y - 720.0f * 0.5f)*-1.0f};
}
