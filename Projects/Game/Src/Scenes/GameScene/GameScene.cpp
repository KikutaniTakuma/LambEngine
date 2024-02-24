#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Game/Water/Water.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->offset.z = -10.0f;

	//water_ = Water::GetInstance();
	

	//model_.reset(new Model{});

	Vector2 size = Vector2::identity * 40.0f;

	Vector3 texPos = Lamb::ClientSize();
	texPos /= 2.0f;
	texPos.x *= -1.0f;
	texPos.x += size.x * 0.5f;
	texPos.y -= size.y * 0.5f;
	Vector3 fisrtPos = texPos;
	constexpr size_t width = 32;

	uint32_t color = 0xff;
	const uint32_t addColor = (std::numeric_limits<uint32_t>::max() / color) / static_cast<uint32_t>(tex_.size());
	color += addColor;

	for (size_t count = 0; auto & i : tex_) {
		i = std::make_unique<Texture2D>();
		i->scale = size;
		i->pos = texPos;
		i->color = std::min(color, std::numeric_limits<uint32_t>::max());
		color += addColor;
		count++;

		texPos.x += size.x;
		if (width <= count) {
			count = 0;
			texPos.y -= size.y;
			texPos.x = fisrtPos.x;
		}
	}

	//tex_.front()->color = 0xff0000ff;
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");

	//water_->Debug("water");
	//water_->Update();

	/*model_->Debug("model");
	model_->Update();*/

	for (size_t i = 0; i < tex_.size(); i++) {
		//tex_[i]->color += 0xff;
		tex_[i]->Update();
	}
}

void GameScene::Draw() {
	meshManager_->ResetDrawCount();

	camera_->Update(/*Vector3::kZero*/);

	/*model_->Draw(camera_->GetViewProjection(), camera_->GetPos());*/

	//water_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	for (auto& i:tex_) {
		i->Draw(camera_->GetViewOthographics());
	}
}