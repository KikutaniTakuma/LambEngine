#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"
#include "Game/Water/Water.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"

#include "Math/Matrix.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.y = 15.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.25f;
	camera_->offset.z = -60.0f;
	camera_->offset.y = 8.0f;

	mat = decltype(mat)::VectorType{
		/*3.2f,0.7f,9.6f,4.4f,
		5.5f,1.3f,7.8f,2.1f,
		6.9f,8.0f,2.6f,1.0f,
		0.5f,7.2f,5.1f,3.3f*/
		1.0f, 2.0f, 0.0f, 1.0f,
		0.0f, 3.0f, 0.0f, 1.0f,
		4.0f, 1.0f, 1.0f, 0.0f,
	};

	mat_2 = decltype(mat_2)::VectorType{
		/*4.1f,6.5f,3.3f,2.2f,
		8.8f,0.6f,9.9f,7.7f,
		1.1f,5.5f,6.6f,0.0f,
		3.3f,9.9f,8.8f,2.2f*/
		1.0f,
		2.0f,
		3.0f,
		0.0f
	};

	auto matResult = mat * mat_2;

	for (size_t y = 0; y < matResult.HeightSize(); y++) {
		for (size_t x = 0; x < matResult.WidthSize(); x++) {
			result[y][x] = matResult[y][x];
		}
	}

	mat4x4 = {
		3.2f,0.7f,9.6f,4.4f,
		5.5f,1.3f,7.8f,2.1f,
		6.9f,8.0f,2.6f,1.0f,
		0.5f,7.2f,5.1f,3.3f
	};
	mat4x4_2 = {
		4.1f,6.5f,3.3f,2.2f,
		8.8f,0.6f,9.9f,7.7f,
		1.1f,5.5f,6.6f,0.0f,
		3.3f,9.9f,8.8f,2.2f
	};

	result2 = mat4x4 * mat4x4_2;
}

void GameScene::Finalize() {
	
}

void GameScene::Update() {
	camera_->Debug("カメラ");
	camera_->Update();

	
}

void GameScene::Draw() {
	Lamb::screenout << "new mat" << Lamb::endline
		<< result.GetMatrixString() << Lamb::endline
		<< "mat4x4" << Lamb::endline
		<< result2.GetMatrixString();
}