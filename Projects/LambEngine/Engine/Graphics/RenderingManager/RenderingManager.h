#pragma once
#include "Utils/SafePtr.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Engine/Graphics/DepthBuffer/DepthBuffer.h"
#include "Engine/Graphics/PipelineObject/GaussianBlur/GaussianBlur.h"
#include "Engine/Graphics/PipelineObject/Outline/Outline.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

#include "Engine/Graphics/PipelineObject/DeferredRendering/DeferredRendering.h"

#include <array>

#include <list>

class RenderingManager {
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

private:
	// アルファ値がないものを描画
	void DrawRGB(std::pair<size_t, const std::list<const RenderData*>&> renderList);

	// cubemapの描画
	void DrawSkyBox();

	// アルファ値があるものを描画
	void DrawRGBA();

	// パーティクルの描画
	void DrawParticle();

	// ディファード描画
	void DrawDefferd();

	// ポストエフェクトを描画する
	void DrawPostEffect();

	// UIの描画
	void DrawUI();

private:
	// アルファ値があるものを順番を並び替える
	void ZSrot();

	// メインのレンダーターゲットをセットする
	void SetMainRenderTarget();


private:
	// ディファードレンダリング用オフスクリーン
	std::unique_ptr<PeraRender> deferredRenderingPera_;
	Lamb::SafePtr<DeferredRendering> deferredRendering_;
	DeferredRendering::DeferredRenderingData deferredRenderingData_;

	// 法線書き込み用オフスクリーン
	std::unique_ptr<RenderTarget> normalTexture_;
	// 色書き込み用オフスクリーン
	std::unique_ptr<RenderTarget> colorTexture_;
	// ワールドポジション書き込み用オフスクリーン
	std::unique_ptr<RenderTarget> worldPositionTexture_;

	// 深度値(法線書き込みと色書き込み、アウトラインで使用する)
	std::unique_ptr<DepthBuffer> depthStencil_;

	// ブルームで使用する輝度抽出用オフスクリーン
	std::unique_ptr<PeraRender> luminateTexture_;
	// ブルームで使用するガウシアンフィルタ(横)用オフスクリーン
	std::unique_ptr<PeraRender> gaussianHorizontalTexture_;
	// ブルームで使用するガウシアンフィルタ(縦)用オフスクリーン
	std::unique_ptr<PeraRender> gaussianVerticalTexture_;

	// ガウシアンフィルタ用パイプラインオジェクト
	std::array<Lamb::SafePtr<GaussianBlur>, 2> gaussianPipeline_;


	// アウトライン用オフスクリーン
	std::unique_ptr<PeraRender> outlineTexture_;
	// アウトライン用パイプラインオジェクト
	Lamb::SafePtr<Outline> outlinePipeline_;


	/// 
	/// その他ポストエフェクトは増える
	/// 

	// ラジアルブラー用オフスクリーン
	//std::unique_ptr<PeraRender>radialTexture_;

	// 色収差用オフスクリーン
	//std::unique_ptr<PeraRender>radialTexture_;

};