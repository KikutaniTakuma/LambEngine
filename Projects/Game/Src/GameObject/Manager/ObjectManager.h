#pragma once
#include <memory>
#include <unordered_set>
#include <list>

#include "Utils/SafePtr.h"

#include "../Object.h"

#include "Level/LevelData.h"
#include "Scenes/Manager/BaseScene/BaseScene.h"

class ObjectManager {
private:
	ObjectManager() = default;
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager(ObjectManager&&) = delete;

	ObjectManager& operator=(const ObjectManager&) = delete;
	ObjectManager& operator=(ObjectManager&&) = delete;

public:
	~ObjectManager();

public:
	static ObjectManager* const GetInstance();

	static void Initialize();

	static void Finalize();

private:
	static std::unique_ptr<ObjectManager> pInstance_;

public:
	void SetLevelData(Lamb::SafePtr<LevelData> levelData);

	const Mat4x4& GetCameraMatrix() const;
	const Vector3& GetCameraPos() const;

	void Set(const Lamb::SafePtr<Object>& object);
	void Erase(const Lamb::SafePtr<Object>& object);

	void Clear();

	bool SetCamera();

	void Update();

	void Draw();

private:
	void Debug();

	void Save();

public:
	void Load(const std::string& jsonFileName);

private:
	std::unordered_set<std::unique_ptr<Object>> objects_;
	Lamb::SafePtr<class CompCollisionManager> collisionManager_;
	std::unordered_map<std::string, bool> objectTags_;
	std::vector<std::string> objectTagKeys_;
	Lamb::SafePtr<class CameraComp> cameraComp_;

	std::unordered_map<std::string, std::unique_ptr<LevelData>> levelDatas_;
	std::string inputSceneName_;
	std::string currentSceneFilePath_;
	std::string currentSceneName_;
#ifdef USE_DEBUG_CODE
	std::vector<std::filesystem::path> levelDataFilePathes_;
	bool isLoad_ = false;
	bool isSelectInverse_ = false;
#endif // USE_DEBUG_CODE

};