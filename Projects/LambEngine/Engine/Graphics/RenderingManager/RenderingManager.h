#pragma once
#include "Utils/SafePtr.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Engine/Graphics/DepthBuffer/DepthBuffer.h"
#include "Engine/Graphics/PipelineObject/GaussianBlur/GaussianBlur.h"
#include "Engine/Graphics/PipelineObject/Outline/Outline.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

#include "Engine/Graphics/PipelineObject/DeferredRendering/DeferredRendering.h"

#include "Drawers/AirSkyBox/AirSkyBox.h"

#include <list>

class RenderingManager {
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

	static Lamb::SafePtr<RenderingManager> const GetInstance();

private:
	static std::unique_ptr<RenderingManager> instance_;

public:
	void FrameStart();

	void FrameEnd();


public:
	void Draw();

	DepthBuffer& GetDepthBuffer();

	void SetCameraPos(const Vector3& cameraPos);
	void SetCameraMatrix(const Mat4x4& camera);
	void SetProjectionInverseMatrix(const Mat4x4& projectionInverce);
	void SetHsv(const Vector3& hsv);
	void SetColor(const Vector4& color);

	void SetTime(float32_t time);

	void Debug(const std::string& guiName);

private:
	// アルファ値がないものを描画
	void DrawRGB(std::pair<size_t, const std::list<RenderData*>&> renderList);

	// cubemapの描画
	void DrawSkyBox();

	// アルファ値があるものを描画
	void DrawRGBA(const RenderDataLists& rgbaList);

	// ディファード描画
	void DrawDefferd();

	// ポストエフェクトを描画する
	void DrawPostEffect();

	// 深度値を使わないものの描画
	void DrawNoDepth(const RenderDataLists& nodepthList);

private:

	// アルファ値があるものを順番を並び替える
	void ZSrot(const RenderDataLists& rgbaList);


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

	GaussianBlur::GaussianBlurState gaussianBlurStateHorizontal_;
	GaussianBlur::GaussianBlurState gaussianBlurStateVertical_;

	// ガウシアンフィルタ用パイプラインオジェクト
	std::array<Lamb::SafePtr<GaussianBlur>, 2> gaussianPipeline_;


	// アウトライン用オフスクリーン
	std::unique_ptr<PeraRender> outlineTexture_;
	// アウトライン用パイプラインオジェクト
	Lamb::SafePtr<Outline> outlinePipeline_;
	float32_t weight_ = 0.0f;

	
	// skybox
	std::unique_ptr<AirSkyBox> skyBox_;
	QuaternionTransform transform_;
	AirSkyBox::AtmosphericParams atmosphericParams_;
	float32_t atmosphericTime_ = 6.0f;

	Mat4x4 cameraMatrix_;

	/// 
	/// その他ポストエフェクトは増える
	/// 

	// ラジアルブラー用オフスクリーン
	//std::unique_ptr<PeraRender>radialTexture_;

	// 色収差用オフスクリーン
	//std::unique_ptr<PeraRender>radialTexture_;

};