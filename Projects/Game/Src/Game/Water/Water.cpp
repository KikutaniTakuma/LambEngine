#include "Water.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "imgui.h"


void Water::Init() {
	waterPipelineObject_ = new WaterPipeline{};
	try {
		waterPipelineObject_->Init();
	}
	catch (const Lamb::Error& err) {
		delete waterPipelineObject_;
		throw err;
	}
	pera_ = std::make_unique<PeraRender>();
	pera_->Initialize(waterPipelineObject_);
	pos.y = -0.1f;
	scale.x = 150.0f;
	scale.y = 150.0f;
	rotate.x = 1.57f;

	staticCamera_ = std::make_unique<Camera>();
	staticCamera_->pos.z = -0.01f;
	staticCamera_->Update();

	waterSurface_.reset(new Texture2D{});

	waterSurface_->scale = Lamb::ClientSize();
	waterSurface_->pos.z += 100.0f;
	waterSurface_->color = Vector4ToUint(Vector4{ 0.1f, 0.25f, 0.5f, 0.0f });

	luminate_ = std::make_unique<PeraRender>();
	bloom_ = std::make_unique<PeraRender>();
	luminate_->Initialize("./Resources/Shaders/PostShader/PostGrayScale.PS.hlsl");
	bloom_->Initialize("./Resources/Shaders/PostShader/PostAveraging.PS.hlsl");
}

void Water::Update() {
	pera_->pos = pos;
	pera_->scale = scale;
	pera_->rotate = rotate;
	pera_->Update();


	waterSurface_->Update();

	luminate_->Update();
	bloom_->Update();
}

void Water::Draw(const Mat4x4& cameraMat, const Vector3& cameraPos) {
	waterPipelineObject_->SetCameraPos(cameraPos);

	pera_->PreDraw();
	waterSurface_->Draw(staticCamera_->GetViewOthographics(), Pipeline::None, false);
	pera_->Draw(cameraMat, Pipeline::None, nullptr, true);

	pera_->Update();

	pera_->PreDraw();
	waterSurface_->Draw(staticCamera_->GetViewOthographics(), Pipeline::None, false);
	//model_->Draw(camera_->GetViewProjection(), camera_->GetPos());
	pera_->Draw(cameraMat, Pipeline::None, luminate_.get());
	luminate_->Draw(staticCamera_->GetViewOthographics(), Pipeline::Add, bloom_.get());
	bloom_->Draw(staticCamera_->GetViewOthographics(), Pipeline::Add);
}

void Water::Debug([[maybe_unused]]const std::string& guiName){
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	waterSurface_->Debug(guiName.c_str());
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::End();
#endif // _DEBUG
}
