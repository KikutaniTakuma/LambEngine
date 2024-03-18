#pragma once
#include <unordered_map>
#include <string>
#include <memory>

#include "../GraphicsStructs.h"

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
	[[noreturn]] void Load(const LoadFileNames& fileNames);

	[[nodiscard]] RenderContext<>* const Get(const LoadFileNames& fileNames);

private:
	[[nodiscard]] Shader LoadShader(const ShaderFileNames& shaderName);

	[[nodiscard]] RenderSetting CreateGraphicsPipelines(Shader shader);


private:
	std::unordered_map<Key, std::unique_ptr<RenderContext<>>> renderData_;
};