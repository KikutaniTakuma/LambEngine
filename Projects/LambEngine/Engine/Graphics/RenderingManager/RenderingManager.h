#pragma once
#include "Utils/SafePtr.h"
#include "Drawer/PeraRender/PeraRender.h"
#include "Engine/Graphics/DepthBuffer/DepthBuffer.h"
#include "Engine/Graphics/PipelineObject/GaussianBlur/GaussianBlur.h"
#include "Engine/Graphics/PipelineObject/Outline/Outline.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

#include "Engine/Graphics/PipelineObject/DeferredRendering/DeferredRendering.h"

#include "Drawer/AirSkyBox/AirSkyBox.h"
#include "json.hpp"

#include <list>

class RenderingManager {
public:
	struct State {
		Vector3 cameraPos;
		Mat4x4 cameraMatrix;
		Mat4x4 projectionMatrix;
		bool isLighting = false;
		bool isUseMeshShader = false;
		Vector3 lightRotate;
		Vector3 hsv;
		int32_t bloomHorizontalSize = 1;
		int32_t bloomVerticalSize = 1;
		float32_t luminanceThreshold = 1.0f;
		bool isDrawSkyBox = false;
		float32_t skyBoxEnvironmentCoefficient = 0.0f;
		Vector3 skyBoxScale = Vector3::kIdentity * 500.0f;
		bool isDrawOutline = false;
		float32_t outlineWeight = 0.0f;
	};

private:
	using RenderDataLists = std::vector<std::pair<size_t, const std::list<RenderData*>&>>;

public:
	RenderingManager();
	RenderingManager(const RenderingManager&) = delete;
	RenderingManager(RenderingManager&&) = delete;
	~RenderingManager();

public:
	RenderingManager& operator=(const RenderingManager&) = delete;
	RenderingManager& operator=(RenderingManager&&) = delete;

public:
	static void Initialize();
	static void Finalize();

	static const Lamb::SafePtr<RenderingManager> GetInstance();

private:
	static std::unique_ptr<RenderingManager> instance_;

public:
	void FrameStart();

	void FrameEnd();

private:
	void Draw();

public:

	DepthBuffer* GetDepthBuffer();

	void SetState(const State& state);

	void SetCameraPos(const Vector3& cameraPos);
	void SetCameraMatrix(const Mat4x4& camera);
	void SetProjectionInverseMatrix(const Mat4x4& projectionInverce);
	void SetHsv(const Vector3& hsv);
	void SetColor(const Vector4& color);
	void SetIsLighting(bool isLighting);
	void SetLightRotate(const Vector3& lightRotate);
	void SetBloomKernelSize(int32_t x, int32_t y);
	void SetEnvironmentCoefficient(float32_t environmentCoefficient);

	void Debug(const std::string& guiName);

	void Save(nlohmann::json& jsonFile);
	void Load(nlohmann::json& jsonFile);

	void SetIsUseMeshShader(bool isUseMesh);

	bool GetIsUseMeshShader() const;

	uint32_t GetBufferIndex()const;

private:
	// アルファ値がないものを描画
	void DrawRGB(std::pair<size_t, const std::list<RenderData*>&> renderList);

	// cubemapの描画
	void DrawSkyBox();

	// アルファ値があるものを描画
	void DrawRGBA(const RenderDataLists& rgbaList);

	// ディファード描画
	void DrawDeferred();

	// ポストエフェクトを描画する
	void DrawPostEffect();

	// 深度値を使わないものの描画
	void DrawNoDepth(const RenderDataLists& nodepthList);

private:

	// アルファ値があるものを順番を並び替える
	void ZSort(const RenderDataLists& rgbaList);


private:
	// ディファードレンダリング用オフスクリーン
	std::unique_ptr<DeferredRendering> deferredRendering_;
	DeferredRendering::DeferredRenderingData deferredRenderingData_;

	// 法線書き込み用オフスクリーン
	std::unique_ptr<RenderTarget> normalTexture_;
	// 色書き込み用オフスクリーン
	std::unique_ptr<RenderTarget> colorTexture_;
	// ワールドポジション書き込み用オフスクリーン
	std::unique_ptr<RenderTarget> worldPositionTexture_;
	// 歪み書き込み用オフスクリーン
	std::unique_ptr<RenderTarget> distortionTexture_;

	// 歪み書き込み用オフスクリーン(アルファ値付きの歪みを書き込む)
	std::unique_ptr<RenderTarget> distortionTextureRGBA_;

	// ライティング後のrgbaテクスチャを描画
	std::unique_ptr<PeraRender> rgbaTexture_;
	Vector3 hsv_;

	// 深度値(法線書き込みと色書き込み、アウトラインで使用する)
	std::unique_ptr<DepthBuffer> depthStencil_;

	// ブルームで使用する輝度抽出用オフスクリーン
	std::unique_ptr<PeraRender> luminateTexture_;
	Lamb::SafePtr<class Luminate> luminate_;
	float32_t luminanceThreshold = 0.0f;

	// ブルームで使用するガウシアンフィルタ(横)用オフスクリーン
	std::unique_ptr<PeraRender> gaussianHorizontalTexture_;
	// ブルームで使用するガウシアンフィルタ(縦)用オフスクリーン
	std::unique_ptr<PeraRender> gaussianVerticalTexture_;

	GaussianBlur::State gaussianBlurStateHorizontal_;
	GaussianBlur::State gaussianBlurStateVertical_;

	// ガウシアンフィルタ用パイプラインオジェクト
	std::array<Lamb::SafePtr<GaussianBlur>, 2> gaussianPipeline_;


	// アウトライン用オフスクリーン
	std::unique_ptr<PeraRender> outlineTexture_;
	// アウトライン用パイプラインオジェクト
	Lamb::SafePtr<Outline> outlinePipeline_;
	float32_t outlineWeight_ = 0.0f;
	bool isDrawOutLine_ = false;


	// skybox
	std::unique_ptr<AirSkyBox> skyBox_;
	QuaternionTransform skyBoxTransform_;
	AirSkyBox::AtmosphericParams atmosphericParams_;
	Vector3 lightRotate_;
	bool isDrawSkyBox_ = true;

	Mat4x4 cameraMatrix_;

	/// 
	/// その他ポストエフェクトは増える予定
	/// 

	// ラジアルブラー用オフスクリーン
	//std::unique_ptr<PeraRender>radialTexture_;

	// 色収差用オフスクリーン
	//std::unique_ptr<PeraRender>radialTexture_;


	bool isUseMesh_ = false;


	bool isFirstFrame_ = true;

	uint32_t bufferIndex_ = 0;
	uint32_t preBufferIndex_ = 0;
};