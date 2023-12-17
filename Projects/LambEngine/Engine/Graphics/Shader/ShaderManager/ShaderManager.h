#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include <unordered_map>
#include <string>
#include <wrl.h>

#include "../Shader.h"

/// <summary>
/// シェーダーを管理するクラス
/// </summary>
class ShaderManager {
private:
	ShaderManager();
	ShaderManager(const ShaderManager&)  =delete;
	ShaderManager(ShaderManager&&) = delete;
	
	ShaderManager& operator=(const ShaderManager&) = delete;
	ShaderManager& operator=(ShaderManager&&) = delete;
public:
	~ShaderManager();

public:
	static void Initialize();

	static void Finalize();

	static inline ShaderManager* const GetInstance() {
		return instance_;
	}

public:
	IDxcBlob* const LoadVertexShader(const std::string& fileName);
	IDxcBlob* const LoadHullShader(const std::string& fileName);
	IDxcBlob* const LoadDomainShader(const std::string& fileName);
	IDxcBlob* const LoadGeometoryShader(const std::string& fileName);
	IDxcBlob* const LoadPixelShader(const std::string& fileName);

private:
	static ShaderManager* instance_;

private:
	class ShaderFactory* shaderFactory_;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> vertexShader_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> hullShader_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> domainShader_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> geometoryShader_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> pixelShader_;
};