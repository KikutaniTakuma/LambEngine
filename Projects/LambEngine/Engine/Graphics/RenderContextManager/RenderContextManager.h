#pragma once
#include <unordered_map>
#include <string>
#include <memory>

#include "RenderContext/RenderContext.h"
#include "../MeshLoader/MeshLoader.h"

class RenderContextManager final {
public:
	struct ShaderFileNames {
		std::string vsFileName;
		std::string psFileName;
		std::string gsFileName;
		std::string dsFileName;
		std::string hsFileName;

		[[nodiscard]] bool operator==(const ShaderFileNames&) const = default;
	};

	struct LoadFileNames {
		std::string reourceFileName;
		ShaderFileNames shaderName;

		[[nodiscard]] bool operator==(const LoadFileNames&) const = default;
	};

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

	static [[noreturn]] void Initialize();

	static [[noreturn]] void Finalize();

private:
	static RenderContextManager* instance_;


public:
	template<IsBasedRenderContext RenderContextType = RenderContext<>>
	[[noreturn]] void Load(const LoadFileNames& fileNames) {
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