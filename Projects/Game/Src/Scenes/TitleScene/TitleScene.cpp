#include "TitleScene.h"
#include "Game/Water/Water.h"
#include "Utils/EngineInfo.h"
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

void TitleScene::Load() {
    drawerManager_->LoadTexture("./Resources/OutGame/title.png");
    drawerManager_->LoadTexture("./Resources/OutGame/titleLogo.png");
    drawerManager_->LoadTexture("./Resources/OutGame/a.png");
    drawerManager_->LoadTexture("./Resources/cursor.png");
    drawerManager_->LoadModel("./Resources/Player/titlePlayer.obj");
    drawerManager_->LoadModel("./Resources/Player/screw.obj");

    audioManager_->Load("./Resources/Sound/sea.wav");
    audioManager_->Load("./Resources/Sound/move.wav");
}

void TitleScene::Initialize() {
    StageManager::ResetIsGoal();
    StageManager::SetStage(0);

    staticCamera_.reset(new Camera{});
    staticCamera_->Update();

    currentCamera_->offset.y = 2.0f;
    currentCamera_->offset.z = -21.0f;
    currentCamera_->rotate.x = 0.07f;

    tex2D_ = drawerManager_->GetTexture2D();

    title_ = std::make_unique<Texture2D::Instance>();
    title_->textureID = drawerManager_->GetTexture("./Resources/OutGame/title.png");
    title_->transform.scale = { 1280.0f, 720.0f };

    water_ = Water::GetInstance();

    titleString_ = std::make_unique<Texture2D::Instance>();
    titleString_->textureID = drawerManager_->GetTexture("./Resources/OutGame/titleLogo.png");
    titleString_->transform.scale = TextureManager::GetInstance()->GetTexture("./Resources/OutGame/titleLogo.png")->getSize() * 1.2f;
    titleString_->transform.translate = { 0.0f, 180.0f };

    hud_ = std::make_unique<Texture2D::Instance>();
    hud_->textureID = drawerManager_->GetTexture("./Resources/OutGame/a.png");
    hud_->transform.scale = TextureManager::GetInstance()->GetTexture("./Resources/OutGame/a.png")->getSize() * 0.36f;
    hud_->color = 0xFFFFFFFFu;
    hud_->transform.translate = { 0.0f, -240.0f };

    hudShadow_ = std::make_unique<Texture2D::Instance>();
    hudShadow_->textureID = drawerManager_->GetTexture("./Resources/OutGame/a.png");
    hudShadowScale_ = TextureManager::GetInstance()->GetTexture("./Resources/OutGame/a.png")->getSize();
    hudShadow_->transform.scale = hudShadowScale_ * 0.36f;
    hudShadow_->color = hud_->color;

    blinkingSpeed_ = std::numbers::pi_v<float> *0.5f;
    blinking_ = 0.0f;

    ship_ = drawerManager_->GetModel("./Resources/Player/titlePlayer.obj");
    shipInstance_->transform.translate.y = 0.06f;
    shipInstance_->transform.translate.z = 100.0f;
    shipPos_ = shipInstance_->transform.translate;
    shipInstance_->transform.rotate = Quaternion::MakeRotateYAxis(std::numbers::pi_v<float> *0.5f);
    screw_ = drawerManager_->GetModel("./Resources/Player/screw.obj");
    shipInstance_->transform.SetParent(&shipInstance_->transform);
    shipInstance_->transform.translate.z = -3.0f;
    screwRotate_ = 0.0f;
    screwRotateSpeed_ = std::numbers::pi_v<float>;

    rotate_ = 0.0f;
    rotateSpeed_ = std::numbers::pi_v<float> *0.01f;
    swaying_ = 0.0f;
    swayingSpeed_ = std::numbers::pi_v<float> *1.0f;

    isSceneChange_ = false;
    sceneChangeRotateBasis_ = 0.22f;
    sceneChangeRotateSpeed_ = rotateSpeed_ * 5.0f;

    easeing_ = std::make_unique<Easeing>();
    volumeEaseing_ = std::make_unique<Easeing>();

    seaSE_ = audioManager_->Get("./Resources/Sound/sea.wav");
    seaSE_->Start(0.05f, true);

    moveShipSE_ = audioManager_->Get("./Resources/Sound/move.wav");

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
    rotate_ += (isSceneChange_ ? easeing_->Get(rotateSpeed_, sceneChangeRotateSpeed_) : rotateSpeed_) * Lamb::DeltaTime();
    swaying_ += swayingSpeed_ * Lamb::DeltaTime();

    rotateMatrix_ = Quaternion::MakeRotateYAxis(rotate_).GetMatrix();
    shipInstance_->transform.translate = ((shipPos_ - water_->transform.translate) * rotateMatrix_) + water_->transform.translate;
    shipInstance_->transform.translate.y = std::abs(std::cos(swaying_)) * 0.05f;
    shipInstance_->transform.rotate = Quaternion::MakeRotateYAxis(std::numbers::pi_v<float> *0.5f + rotate_);

    screwRotate_ += screwRotateSpeed_ * Lamb::DeltaTime();

    screwInstance_->transform.rotate = Quaternion::MakeRotateXAxis(screwRotate_);

    currentCamera_->rotate.y = rotate_;

    shipInstance_->transform.CalcMatrix();
    screwInstance_->transform.CalcMatrix();

    currentCamera_->Debug("タイトルカメラ");
    if (!isSceneChange_) {
        Vector3 posTmp = shipInstance_->transform.translate;
        posTmp.y = 0.0f;
        currentCamera_->Update(posTmp);
    }

    title_->transform.CalcMatrix();

    water_->Update(currentCamera_->GetPos());

    titleString_->transform.CalcMatrix();
    hud_->transform.CalcMatrix();

    blinking_ += blinkingSpeed_ * Lamb::DeltaTime();
    if (blinking_ >= 0.8f) {
        blinking_ = 0.0f;
    }
    //hud_->color = 0xFFFFFF00u + static_cast<uint32_t>(255.0f * std::abs(std::sin(blinking_)));
    hudShadow_->color = 0xFFFFFFFFu - static_cast<uint32_t>(255.0f * (blinking_ / 0.8f));
    hudShadow_->transform.scale = hudShadowScale_ * (0.36f + 0.18f * blinking_);
    hudShadow_->transform.CalcMatrix();

    if (!isSceneChange_ &&
        Input::GetInstance()->GetMouse()->Pushed(Mouse::Button::Left)) {
        isSceneChange_ = true;
        sceneChangeRotate_ = rotate_;
        easeing_->Start(false, 0.2f, Easeing::OutQuad);
        volumeEaseing_->Start(false, 1.2f, Easeing::InSine);

        moveShipSE_->Start(moveShipSEVolume_.first, true);
        seaSE_->SetVolume(0.1f);
    }

    if (isSceneChange_) {
        moveShipSE_->SetVolume(volumeEaseing_->Get(moveShipSEVolume_.first, moveShipSEVolume_.second));
    }

    //#ifdef _DEBUG
    //		ImGui::Begin("Title");
    //		ImGui::DragFloat("sceneChangeRotateBasis_", &sceneChangeRotateBasis_, 0.01f);
    //		ImGui::Checkbox("isSceneChange_", &isSceneChange_);
    //		ImGui::Text(std::format("isChange : {}", sceneChangeRotateBasis_ <= (rotate_ - sceneChangeRotate_)).c_str());
    //		ImGui::End();
    //#endif // _DEBUG
    if (isSceneChange_ && sceneChangeRotateBasis_ <= (rotate_ - sceneChangeRotate_)) {
        sceneManager_->SceneChange(BaseScene::ID::StageSelect);
    }

    volumeEaseing_->Update();
    easeing_->Update();
}

void TitleScene::Draw() {
    water_->Draw(currentCamera_->GetViewProjection());

    ship_->Draw(
        shipInstance_->transform.GetMatrix(), 
        currentCamera_->GetViewProjection(), 
        shipInstance_->color,
        BlendType::kNone,
        shipInstance_->isLighting
        );
    screw_->Draw(
        screwInstance_->transform.GetMatrix(),
        currentCamera_->GetViewProjection(),
        screwInstance_->color,
        BlendType::kNone,
        screwInstance_->isLighting
    );


    // hud系
    tex2D_->Draw(
        titleString_->transform.GetMatrix(),
        Mat4x4::kIdentity,
        staticCamera_->GetViewOthographics(),
        titleString_->textureID,
        titleString_->color,
        BlendType::kUnenableDepthNone
    );
    tex2D_->Draw(
        hudShadow_->transform.GetMatrix(),
        Mat4x4::kIdentity,
        staticCamera_->GetViewOthographics(),
        hudShadow_->textureID,
        hudShadow_->color,
        BlendType::kUnenableDepthNone
    );
    tex2D_->Draw(
        hud_->transform.GetMatrix(),
        Mat4x4::kIdentity,
        staticCamera_->GetViewOthographics(),
        hud_->textureID,
        hud_->color,
        BlendType::kUnenableDepthNone
    );

    cursor_->Draw(staticCamera_->GetViewOthographics());
}
