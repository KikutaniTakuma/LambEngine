#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include "Mesh/Mesh.h"
#include "Utils/UtilsLib/UtilsLib.h"

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
	static MeshManager* instance_;

public:
	/// <summary>
	/// Objファイル読み込み
	/// </summary>
	/// <param name="objFileName">ファイルの名前</param>
	/// <returns>読み込んだメッシュ</returns>
	Mesh* LoadObj(const std::string& objFileName);

public:
	/// <summary>
	/// アンロードする
	/// </summary>
	/// <param name="objFileName">ファイルの名前</param>
	void Unload(const std::string& objFileName);

public:
	void ResetDrawCount();

	void Draw();

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshs_;
};