#include "TitleScene.h"
#include "Game/Water/Water.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Game/StageManager/StageManager.h"
#include <numbers>
#include <cmath>

#ifdef _DEBUG
#include "imgui.h"
#include <format>
#endif // _DEBUG


TitleScene::TitleScene() : BaseScene(BaseScene::ID::Title) {}

void TitleScene::Initialize() {
    StageManager::ResetIsGoal();
    StageManager::SetStage(0);

    staticCamera_.reset(new Camera{});
    staticCamera_->Update();

    camera_->offset.y = 2.0f;
    camera_->offset.z = -21.0f;
    camera_->rotate.x = 0.07f;

    title_ = std::make_unique<Texture2D>();
    title_->LoadTexture("./Resources/OutGame/title.png");
    title_->scale = { 1280.0f, 720.0f };
    water_ = Water::GetInstance();

    titleString_.reset(new Texture2D{});
    titleString_->LoadTexture("./Resources/OutGame/titleLogo.png");
    titleString_->isSameTexSize = true;
    titleString_->texScalar = 1.2f;

    //titleString_->scale = Vector2::identity * 3.0f;
    titleString_->pos = { 0.0f, 180.0f };

    hud_.reset(new Texture2D{});
    hud_->LoadTexture("./Resources/OutGame/a.png");
    hud_->isSameTexSize = true;
    hud_->texScalar = 0.36f;
    hud_->color = 0xFFFFFFFFu;
    hud_->scale = Vector2::identity * 2.0f;
    hud_->pos = { 0.0f, -240.0f };

    hudShadow_.reset(new Texture2D{});
    hudShadow_->LoadTexture("./Resources/OutGame/a.png");
    hudShadow_->isSameTexSize = true;
    hudShadow_->texScalar = hud_->texScalar;
    hudShadow_->color = hud_->color;
    hudShadow_->scale = hud_->scale;
    hudShadow_->pos = hud_->pos;

    blinkingSpeed_ = std::numbers::pi_v<float> *0.5f;
    blinking_ = 0.0f;

    ship_.reset(new Model{ "./Resources/Player/titlePlayer.obj" });
    ship_->pos.y = 0.06f;
    ship_->pos.z = 100.0f;
    shipPos_ = ship_->pos;
    ship_->rotate.y = std::numbers::pi_v<float> *0.5f;
    screw_.reset(new Model{ "./Resources/Player/screw.obj" });
    screw_->SetParent(ship_.get());
    screw_->pos.z = -3.0f;
    screwRotate_ = 0.0f;
    screwRotateSpeed_ = std::numbers::pi_v<float>;

    rotate_ = 0.0f;
    rotateSpeed_ = std::numbers::pi_v<float> *0.01f;
    swaying_ = 0.0f;
    swayingSpeed_ = std::numbers::pi_v<float> *1.0f;

    skydome_ = std::make_unique<Skydome>();
    skydome_->Initialize();

    cloud_ = Cloud::GetInstance();
    skydome_->SetTexture(cloud_->GetTex());

    isSceneChange_ = false;
    sceneChangeRotateBasis_ = 0.22f;
    sceneChangeRotateSpeed_ = rotateSpeed_ * 5.0f;

    easing_.reset(new Easing{});
    volumeEasing_.reset(new Easing{});

    seaSE_ = audioManager_->LoadWav("./Resources/Sound/sea.wav", true);
    seaSE_->Start(0.05f);

    moveShipSE_ = audioManager_->LoadWav("./Resources/Sound/move.wav", true);

    moveShipSEVolume_ = { 0.4f, 0.02f };

    cursor_ = std::make_unique<Cursor>();
    cursor_->Initialize();
}

void TitleScene::Finalize() {
    seaSE_->Stop();
    moveShipSE_->Stop();
}

void TitleScene::Update() {
    cursor_->Update();
    rotate_ += (isSceneChange_ ? easing_->Get(rotateSpeed_, sceneChangeRotateSpeed_) : rotateSpeed_) * Lamb::DeltaTime();
    swaying_ += swayingSpeed_ * Lamb::DeltaTime();

    rotateMatrix_.RotateY(rotate_);
    ship_->pos = ((shipPos_ - water_->pos) * rotateMatrix_) + water_->pos;
    ship_->pos.y = std::abs(std::cos(swaying_)) * 0.05f;
    ship_->rotate.y = std::numbers::pi_v<float> *0.5f + rotate_;

    screwRotate_ += screwRotateSpeed_ * Lamb::DeltaTime();

    screw_->rotate.x = screwRotate_;

    camera_->rotate.y = rotate_;

    ship_->Debug("ship");
    ship_->Update();
    screw_->Update();

    camera_->Debug("タイトルカメラ");
    if (!isSceneChange_) {
        Vector3 posTmp = ship_->pos;
        posTmp.y = 0.0f;
        camera_->Update(posTmp);
    }

    title_->Update();

    water_->Update(camera_->GetPos());

    titleString_->Debug("タイトル");
    hud_->Debug("HUD");
    titleString_->Update();
    hud_->Update();
    hudShadow_->Update();

    blinking_ += blinkingSpeed_ * Lamb::DeltaTime();
    if (blinking_ >= 0.8f) {
        blinking_ = 0.0f;
    }
    //hud_->color = 0xFFFFFF00u + static_cast<uint32_t>(255.0f * std::abs(std::sin(blinking_)));
    hudShadow_->color = 0xFFFFFFFFu - static_cast<uint32_t>(255.0f * (blinking_ / 0.8f));
    hudShadow_->texScalar = 0.36f + 0.18f * blinking_;

    skydome_->Update();
    cloud_->Update();

    if (!isSceneChange_ &&
        Input::GetInstance()->GetMouse()->Pushed(Mouse::Button::Left)) {
        isSceneChange_ = true;
        sceneChangeRotate_ = rotate_;
        easing_->Start(false, 0.2f, Easing::OutQuad);
        volumeEasing_->Start(false, 1.2f, Easing::InSine);

        moveShipSE_->Start(moveShipSEVolume_.first);
        seaSE_->SetVolume(0.1f);
    }

    if (isSceneChange_) {
        moveShipSE_->SetVolume(volumeEasing_->Get(moveShipSEVolume_.first, moveShipSEVolume_.second));
    }

    //#ifdef _DEBUG
    //		ImGui::Begin("Title");
    //		ImGui::DragFloat("sceneChangeRotateBasis_", &sceneChangeRotateBasis_, 0.01f);
    //		ImGui::Checkbox("isSceneChange_", &isSceneChange_);
    //		ImGui::Text(std::format("isChange : {}", sceneChangeRotateBasis_ <= (rotate_ - sceneChangeRotate_)).c_str());
    //		ImGui::End();
    //#endif // _DEBUG
    if (isSceneChange_ && sceneChangeRotateBasis_ <= (rotate_ - sceneChangeRotate_)) {
        SceneManager::GetInstance()->SceneChange(BaseScene::ID::StageSelect);
    }

    volumeEasing_->Update();
    easing_->Update();
}

void TitleScene::Draw() {
    cloud_->Draw();
    skydome_->Draw(*camera_);
    water_->Draw(camera_->GetViewProjection());

    ship_->Draw(camera_->GetViewProjection(), camera_->GetPos());
    screw_->Draw(camera_->GetViewProjection(), camera_->GetPos());


    // hud系
    titleString_->Draw(staticCamera_->GetViewOthographics());
    hudShadow_->Draw(staticCamera_->GetViewOthographics());
    hud_->Draw(staticCamera_->GetViewOthographics());
    //title_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);

    cursor_->Draw(staticCamera_->GetViewOthographics());
}
