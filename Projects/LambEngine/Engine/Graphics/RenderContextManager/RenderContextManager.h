#pragma once
#include <unordered_map>
#include <string>
#include <memory>

#include "RenderContext/RenderContext.h"
#include "../MeshLoader/MeshLoader.h"

namespace std {
	template<>
	struct hash<LoadFileNames> {
	public:
		size_t operator()(const LoadFileNames& data)const {
			size_t result{};

			result = std::hash<std::string>{}(
				data.reourceFileName +
				data.shaderName.vsFileName +
				data.shaderName.psFileName +
				data.shaderName.gsFileName +
				data.shaderName.dsFileName +
				data.shaderName.hsFileName
				);

			return result;
		}

	};
}

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
	static RenderContextManager* instance_;


public:
	template<IsBasedRenderContext RenderContextType = RenderContext<>>
	void Load(const LoadFileNames& fileNames) {
		auto isExist = renderData_.find(fileNames);

		if (isExist == renderData_.end()) {
			return;
		}

		renderData_.insert(std::make_pair(fileNames, std::make_unique<RenderSet>()));

		RenderSet& currentRenderSet = *renderData_[fileNames];

		Shader shader = LoadShader(fileNames.shaderName);

		const std::array<Pipeline*, BlendType::kNum>& pipelines = CreateGraphicsPipelines(shader);

		Mesh&& mesh = MeshLoader::LoadObj(fileNames.reourceFileName);

		for (uint32_t i = 0; i < BlendType::kNum; i++) {
			RenderContextType* renderContext = new RenderContextType();

			renderContext->SetMesh(mesh);
			renderContext->SetPipeline(pipelines[i]);
			currentRenderSet.Set(renderContext, BlendType(i));
		}
	}

	[[nodiscard]] RenderSet* const Get(const LoadFileNames& fileNames);

private:
	[[nodiscard]] Shader LoadShader(const ShaderFileNames& shaderName);

	[[nodiscard]] std::array<Pipeline*, BlendType::kNum> CreateGraphicsPipelines(Shader shader);


private:
	std::unordered_map<Key, std::unique_ptr<RenderSet>> renderData_;
};