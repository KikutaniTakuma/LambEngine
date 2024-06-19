#pragma once
#include <memory>
#include <thread>
#include <queue>

#include "Utils/SafePtr.h"
#include "../GraphicsStructs.h"

/// <summary>
/// メッシュを管理
/// </summary>
class MeshManager final {
private:
	MeshManager() = default;
	MeshManager(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;

	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;
public:
	~MeshManager();

public:
	static MeshManager* const GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static Lamb::SafePtr<MeshManager> instance_;

public:
	void LoadModel(const std::string& objFileName);
	[[nodiscard]] Mesh* GetMesh(const std::string& objFileName);
	[[nodiscard]] ModelData* GetModelData(const std::string& objFileName);

private:
	[[nodiscard]] Mesh* CreateMesh(const ModelData& modelData);

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshs_;
	std::unordered_map<std::string, std::unique_ptr<ModelData>> modelData_;
};