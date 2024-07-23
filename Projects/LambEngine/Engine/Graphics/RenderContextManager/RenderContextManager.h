#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <list>

#include "RenderContext/RenderContext.h"
#include "../MeshManager/MeshManager.h"

class RenderContextManager final {
private:
	using Key = LoadFileNames;

private:
	RenderContextManager() = default;
	RenderContextManager(const RenderContextManager&) = delete;
	RenderContextManager(RenderContextManager&&) = delete;
public:
	~RenderContextManager();

	RenderContextManager& operator=(const RenderContextManager&) = delete;
	RenderContextManager& operator=(RenderContextManager&&) = delete;

public:
	static [[nodiscard]] RenderContextManager* const GetInstance();

	static void Initialize();

	static void Finalize();

private:
	static Lamb::SafePtr<RenderContextManager> instance_;


public:
	template<IsBasedRenderContext RenderContextType = RenderContext<>>
	[[nodiscard]] RenderSet* const Create(const LoadFileNames& fileNames, uint32_t numRenderTarget = 1) {
		std::unique_ptr<RenderSet> renderSet = std::make_unique<RenderSet>();

		Shader shader = LoadShader(fileNames.shaderName);

		const std::array<Pipeline*, BlendType::kNum>& pipelines = CreateGraphicsPipelines(shader, numRenderTarget);

		Lamb::SafePtr meshManager = MeshManager::GetInstance();
		meshManager->LoadModel(fileNames.resourceFileName);
		Mesh* mesh = meshManager->GetMesh(fileNames.resourceFileName);
		ModelData* modelData = meshManager->GetModelData(fileNames.resourceFileName);

		for (uint32_t i = 0; i < BlendType::kNum; i++) {
			std::unique_ptr<RenderContextType> renderContext = std::make_unique<RenderContextType>();

			renderContext->SetMesh(mesh);
			renderContext->SetModelData(modelData);
			renderContext->SetPipeline(pipelines[i]);
			renderSet->Set(renderContext.release(), BlendType(i));
		}

		return renderSet.release();
	}

	template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
	[[nodiscard]] RenderSet* const CreateSkinAnimationModel(const LoadFileNames& fileNames, uint32_t numRenderTarget = 1) {
		std::unique_ptr<RenderSet> renderSet = std::make_unique<RenderSet>();

		Shader shader = LoadShader(fileNames.shaderName);

		const std::array<Pipeline*, BlendType::kNum>& pipelines = CreateSkinAnimationGraphicsPipelines(shader, numRenderTarget);

		Lamb::SafePtr meshManager = MeshManager::GetInstance();
		meshManager->LoadModel(fileNames.resourceFileName);
		Mesh* mesh = meshManager->GetMesh(fileNames.resourceFileName);
		ModelData* modelData = meshManager->GetModelData(fileNames.resourceFileName);

		for (uint32_t i = 0; i < BlendType::kNum; i++) {
			std::unique_ptr<SkinRenderContext<T, bufferSize>> renderContext = std::make_unique<SkinRenderContext<T, bufferSize>>();

			renderContext->SetMesh(mesh);
			renderContext->SetModelData(modelData);
			renderContext->SetPipeline(pipelines[i]);
			renderSet->Set(renderContext.release(), BlendType(i));
		}

		return renderSet.release();
	}

	/// <summary>
	/// 描画に必要なものをロードしてコンテナに追加(1度追加してたら追加しない)
	/// </summary>
	/// <typeparam name="RenderContextType">レンダーコンテキストタイプ</typeparam>
	/// <param name="fileNames">リソースファイル名</param>
	/// <param name="numRenderTarget">レンダーターゲットの数</param>
	template<IsBasedRenderContext RenderContextType = RenderContext<>>
	void Load(const LoadFileNames& fileNames, uint32_t numRenderTarget = 1) {
		auto isExist = renderData_.find(fileNames);

		if (isExist != renderData_.end()) {
			return;
		}

		auto& currentRenderData = (isNowThreading_ ? threadRenderData_ : renderData_);

		currentRenderData.insert(std::make_pair(fileNames, std::make_unique<RenderSet>()));

		RenderSet& currentRenderSet = *currentRenderData[fileNames];

		Shader shader = LoadShader(fileNames.shaderName);

		const std::array<Pipeline*, BlendType::kNum>& pipelines = CreateGraphicsPipelines(shader, numRenderTarget);

		Lamb::SafePtr meshManager = MeshManager::GetInstance();
		meshManager->LoadModel(fileNames.resourceFileName);
		Mesh* mesh = meshManager->GetMesh(fileNames.resourceFileName);
		ModelData* modelData = meshManager->GetModelData(fileNames.resourceFileName);

		for (uint32_t i = 0; i < BlendType::kNum; i++) {
			std::unique_ptr<RenderContextType> renderContext = std::make_unique<RenderContextType>();

			renderContext->SetMesh(mesh);
			renderContext->SetModelData(modelData);
			renderContext->SetPipeline(pipelines[i]);
			currentRenderSet.Set(renderContext.release(), BlendType(i));
		}
	}

	/// <summary>
	/// スキンアニメーション用。描画に必要なものをロードしてコンテナに追加(1度追加してたら追加しない)
	/// </summary>
	/// <typeparam name="RenderContextType">レンダーコンテキストタイプ</typeparam>
	/// <param name="fileNames">リソースファイル名</param>
	/// <param name="numRenderTarget">レンダーターゲットの数</param>
	template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
	void LoadSkinAnimationModel(const LoadFileNames& fileNames, uint32_t numRenderTarget = 1) {
		auto isExist = renderData_.find(fileNames);

		if (isExist != renderData_.end()) {
			return;
		}

		auto& currentRenderData = (isNowThreading_ ? threadRenderData_ : renderData_);

		currentRenderData.insert(std::make_pair(fileNames, std::make_unique<RenderSet>()));

		RenderSet& currentRenderSet = *currentRenderData[fileNames];

		Shader shader = LoadShader(fileNames.shaderName);

		const std::array<Pipeline*, BlendType::kNum>& pipelines = CreateSkinAnimationGraphicsPipelines(shader, numRenderTarget);

		Lamb::SafePtr meshManager = MeshManager::GetInstance();
		meshManager->LoadModel(fileNames.resourceFileName);
		Mesh* mesh = meshManager->GetMesh(fileNames.resourceFileName);
		ModelData* modelData = meshManager->GetModelData(fileNames.resourceFileName);

		for (uint32_t i = 0; i < BlendType::kNum; i++) {
			std::unique_ptr<SkinRenderContext<T, bufferSize>> renderContext = std::make_unique<SkinRenderContext<T, bufferSize>>();

			renderContext->SetMesh(mesh);
			renderContext->SetModelData(modelData);
			renderContext->SetPipeline(pipelines[i]);
			currentRenderSet.Set(renderContext.release(), BlendType(i));
		}
	}

	/// <summary>
	/// リソースファイルから描画構造体を取得する(deleteしてはいけない)
	/// </summary>
	/// <param name="fileNames"></param>
	/// <returns></returns>
	[[nodiscard]] RenderSet* const Get(const LoadFileNames& fileNames);

	void SetIsNowThreading(bool isNowThreading);
public:
	std::pair<size_t, const std::list<RenderData*>&> CreateRenderList(BlendType blend);
	void ResizeRenderList();
	
	void Draw();
	void ResetDrawCount();

private:
	[[nodiscard]] Shader LoadShader(const ShaderFileNames& shaderName);

	[[nodiscard]] std::array<Pipeline*, BlendType::kNum> CreateGraphicsPipelines(Shader shader, uint32_t numRenderTarget = 1);
	[[nodiscard]] std::array<Pipeline*, BlendType::kNum> CreateSkinAnimationGraphicsPipelines(Shader shader, uint32_t numRenderTarget = 1);


private:
	std::unordered_map<Key, std::unique_ptr<RenderSet>> renderData_;
	std::unordered_map<Key, std::unique_ptr<RenderSet>> threadRenderData_;
	bool isNowThreading_ = false;

	std::array<std::list<RenderData*>, BlendType::kNum> renderDataLists_;
};