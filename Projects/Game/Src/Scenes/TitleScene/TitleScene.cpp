#include "TitleScene.h"
#include "Game/Water/Water.h"
#include "Utils/EngineInfo.h"
#include "AudioManager/AudioManager.h"
#include "GameExternals/StageManager/StageManager.h"

#include "Engine/Graphics/RenderingManager/RenderingManager.h"
#include <numbers>
#include <cmath>

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#include <format>
#endif // USE_DEBUG_CODE

#include "Utils/Random.h"


TitleScene::TitleScene() : BaseScene(BaseScene::ID::Title) {}

void TitleScene::Load()
{
}

void TitleScene::Initialize() {
    StageManager::ResetIsGoal();
    StageManager::SetStage(0);

    staticCamera_.reset(new Camera{});
    staticCamera_->pos.z = -10.0f;
    staticCamera_->Update();

    currentCamera_->offset.y = 2.0f;
    currentCamera_->offset.z = -21.0f;
    currentCamera_->rotate.x = 0.07f;

    title_ = std::make_unique<Tex2DInstance>();
    title_->Load("./Resources/OutGame/title.png");
    title_->scale = { 1280.0f, 720.0f };
    water_ = Water::GetInstance();

    titleString_.reset(new Tex2DInstance{});
    titleString_->Load("./Resources/OutGame/titleLogo.png");
    titleString_->scale = titleString_->GetTexSize() * 1.2f;

    //titleString_->scale = Vector2::identity * 3.0f;
    titleString_->pos = { 0.0f, 180.0f };

    hud_.reset(new Tex2DInstance{});
    hud_->Load("./Resources/OutGame/a.png");
    hud_->color = 0xFFFFFFFFu;
    hud_->scale = hud_->GetTexSize() * 0.36f;
    hud_->pos = { 0.0f, -240.0f };

    hudShadow_.reset(new Tex2DInstance{});
    hudShadow_->Load("./Resources/OutGame/a.png");
    hudShadow_->color = hud_->color;
    hudShadow_->scale = hud_->scale;
    hudShadow_->pos = hud_->pos;

    blinkingSpeed_ = std::numbers::pi_v<float> *0.5f;
    blinking_ = 0.0f;

    ship_.reset(new ModelInstance());
    ship_->Load("./Resources/Player/titlePlayer.obj");
    ship_->pos.y = 0.06f;
    ship_->pos.z = 100.0f;
    shipPos_ = ship_->pos;
    ship_->rotate.y = std::numbers::pi_v<float> *0.5f;
    screw_.reset(new ModelInstance());
    screw_->Load("./Resources/Player/screw.obj");
    screw_->SetParent(ship_.get());
    screw_->pos.z = -3.0f;
    screwRotate_ = 0.0f;
    screwRotateSpeed_ = std::numbers::pi_v<float>;

    rotate_ = 0.0f;
    rotateSpeed_ = std::numbers::pi_v<float> *0.01f;
    swaying_ = 0.0f;
    swayingSpeed_ = std::numbers::pi_v<float> *1.0f;

    isSceneChange_ = false;
    sceneChangeRotateBasis_ = 0.22f;
    sceneChangeRotateSpeed_ = rotateSpeed_ * 5.0f;

    easing_.reset(new Easeing{});
    volumeEasing_.reset(new Easeing{});

    audioManager_->Load("./Resources/Sound/sea.mp3");
    seaSE_ = audioManager_->Get("./Resources/Sound/sea.mp3");
    seaSE_->Start(0.05f, true);

    audioManager_->Load("./Resources/Sound/move.mp3");
    moveShipSE_ = audioManager_->Get("./Resources/Sound/move.mp3");

    moveShipSEVolume_ = { 0.4f, 0.02f };

    cursor_ = std::make_unique<Cursor>();
    cursor_->Initialize();

    renderingManager_->SetLightRotate(Vector3(22.0f, -8.0f, 0.0f) * Lamb::Math::toRadian<float>);
    renderingManager_->SetBloomKernelSize(128, 128);
    renderingManager_->SetEnvironmentCoefficient(0.3f);
    renderingManager_->SetTonemapParam({ 0.166f, 0.133f }, { 0.6f, 0.4f }, {15.0f, 1.0f});

    corals_.Init();

    cloud_ = std::make_unique<Cloud>();

    fishes_ = std::make_unique<Fishes>();
    fishes_->Init(200);

    tex2DData_.blend = BlendType::kNone;
    tex2DData_.uvTransform = float32_t4x4::MakeAffin(float32_t3::kIdentity * 10.0f, Quaternion::kIdentity, float32_t3::kZero);
    tex2DData_.worldMatrix = float32_t4x4::MakeAffin(float32_t3(400.f, 400.0f, 1.0f), Quaternion::MakeRotateXAxis(1.57f), water_->transform.translate);
    tex2DData_.worldMatrix[3][1] = -8.0f;
    tex2DData_.color = 0xffffffff;
    drawerManager_->LoadTexture("./Resources/sandybeach.png");
    tex2DData_.textureID = drawerManager_->GetTexture("./Resources/sandybeach.png");
}

void TitleScene::Finalize() {
    seaSE_->Stop();
    moveShipSE_->Stop();
}

void TitleScene::Update() {
    cursor_->Update();
    rotate_ += (isSceneChange_ ? easing_->Get(rotateSpeed_, sceneChangeRotateSpeed_) : rotateSpeed_) * Lamb::DeltaTime();
    swaying_ += swayingSpeed_ * Lamb::DeltaTime();

    rotateMatrix_ = Mat4x4::MakeRotateY(rotate_);
    ship_->pos = ((shipPos_ - water_->transform.translate) * rotateMatrix_) + water_->transform.translate;
    ship_->pos.y = std::abs(std::cos(swaying_)) * 0.05f;
    ship_->rotate.y = std::numbers::pi_v<float> *0.5f + rotate_;

    screwRotate_ += screwRotateSpeed_ * Lamb::DeltaTime();

    screw_->rotate.x = screwRotate_;

    currentCamera_->rotate.y = rotate_;

    ship_->Update();
    screw_->Update();

    currentCamera_->Debug("タイトルカメラ");
    if (!isSceneChange_) {
        Vector3 posTmp = ship_->pos;
        posTmp.y = 0.0f;
        currentCamera_->Update(posTmp);
    }


    renderingManager_->SetCameraPos(currentCamera_->GetPos());
    renderingManager_->SetViewMatrix(currentCamera_->GetView());
    renderingManager_->SetProjectionMatrix(currentCamera_->GetProjection());

    water_->Update();

    blinking_ += blinkingSpeed_ * Lamb::DeltaTime();
    if (blinking_ >= 0.8f) {
        blinking_ = 0.0f;
    }
    //hud_->color = 0xFFFFFF00u + static_cast<uint32_t>(255.0f * std::abs(std::sin(blinking_)));
    hudShadow_->color = 0xFFFFFFFFu - static_cast<uint32_t>(255.0f * (blinking_ / 0.8f));
    hudShadow_->scale = hudShadow_->GetTexSize() * (0.36f + 0.18f * blinking_);

    if (!isSceneChange_ &&
        Input::GetInstance()->GetMouse()->Pushed(Mouse::Button::Left)) {
        isSceneChange_ = true;
        sceneChangeRotate_ = rotate_;
        easing_->Start(false, 0.2f, Easeing::Type::kOutQuad);
        volumeEasing_->Start(false, 1.2f, Easeing::Type::kInSine);

        moveShipSE_->Start(moveShipSEVolume_.first, true);
        seaSE_->SetVolume(0.1f);
    }

    if (isSceneChange_) {
        moveShipSE_->SetVolume(volumeEasing_->Get(moveShipSEVolume_.first, moveShipSEVolume_.second));
    }

    #ifdef USE_DEBUG_CODE
    renderingManager_->Debug("RenderSetting");
    water_->Debug("water");
    #endif // USE_DEBUG_CODE
    if (isSceneChange_ && sceneChangeRotateBasis_ <= (rotate_ - sceneChangeRotate_)) {
        SceneManager::GetInstance()->SceneChange(BaseScene::ID::StageSelect);
    }

    volumeEasing_->Update();
    easing_->Update();

    fishes_->Update();

    water_->SetCameraPos(currentCamera_->GetPos());
}

void TitleScene::Draw() {
    water_->Draw(currentCamera_->GetViewProjection());

    ship_->Draw(currentCamera_->GetViewProjection());
    screw_->Draw(currentCamera_->GetViewProjection());


    // hud系
    titleString_->blend = BlendType::kUnenableDepthNormal;
    titleString_->Draw(staticCamera_->GetViewOthographics());
    hudShadow_->blend = BlendType::kUnenableDepthNormal;
    hudShadow_->Draw(staticCamera_->GetViewOthographics());
    hud_->blend = BlendType::kUnenableDepthNormal;
    hud_->pos.z = 1.0f;
    hud_->Draw(staticCamera_->GetViewOthographics());
    //title_->Draw(currentCamera_->GetViewOthographics(), Pipeline::Normal, false);

    cursor_->Draw(Mat4x4::MakeTranslate({ 0.0f, 0.0f, 10.0f }) * Camera::GetStaticViewOthographics());


    corals_.Draw(currentCamera_->GetViewProjection());

    cloud_->Draw(currentCamera_->GetViewProjection());
    fishes_->Draw(currentCamera_->GetViewProjection());

    tex2DData_.camera = currentCamera_->GetViewProjection();
    drawerManager_->GetTexture2D()->Draw(tex2DData_);
}
