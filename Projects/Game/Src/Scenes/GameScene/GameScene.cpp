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
		3.2f,0.7f,9.6f,4.4f,
		5.5f,1.3f,7.8f,2.1f,
		6.9f,8.0f,2.6f,1.0f,
		0.5f,7.2f,5.1f,3.3f
	};

	mat2 = decltype(mat2)::VectorType{
		4.1f,6.5f,3.3f,2.2f,
		8.8f,0.6f,9.9f,7.7f,
		1.1f,5.5f,6.6f,0.0f,
		3.3f,9.9f,8.8f,2.2f
	};

	inverse = mat.Inverse();
	inverse2 = mat2.Inverse();

	result = mat * mat2;

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

	mat4x4inverse = Mat4x4::MakeInverse(mat4x4);
	mat4x4inverse2 = Mat4x4::MakeInverse(mat4x4_2);

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
		<< result.GetString()
		<< "mat4x4" << Lamb::endline
		<< result2.GetMatrixString() << Lamb::endline
		<< "new mat inverse" << Lamb::endline
		<< inverse.GetString()
		<< "new mat inverse2" << Lamb::endline
		<< inverse2.GetString()
		<< "mat4x4 invese" << Lamb::endline
		<< mat4x4inverse.GetMatrixString() << Lamb::endline
		<< "mat4x4_2 invese" << Lamb::endline
		<< mat4x4inverse2.GetMatrixString() << Lamb::endline;
}