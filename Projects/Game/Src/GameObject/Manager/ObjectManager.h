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
	static std::unique_ptr<ObjectManager> instance_;

public:
	template<IsBasedBaseScene Scene>
	void SetLevelData(Lamb::SafePtr<LevelData> levelData) {
		assert(levelData.have());
		currentScene_ = typeid(Scene).name();
		levelDatas_[currentScene_].reset(levelData.get());
		for (auto& i : levelDatas_[currentScene_]->objects) {
			this->Set(i);
		}

		SetCamera();
	}

	const Mat4x4& GetCameraMatrix() const;
	const Vector3& GetCameraPos() const;

	void Set(const Lamb::SafePtr<Object>& object);
	void Erase(const Lamb::SafePtr<Object>& object);

	void Clear();

	bool SetCamera();

	void Update();

	void Draw();

private:
	void Collision();

	void Debug();

	void Save();

	void Load();

private:
	std::unordered_set<std::unique_ptr<Object>> objects_;
	std::list<Lamb::SafePtr<class ObbPushComp>> obbObjects_;
	std::unordered_map<std::string, bool> objectTags_;
	Lamb::SafePtr<class CameraComp> cameraComp_;

	std::unordered_map<std::string, std::unique_ptr<LevelData>> levelDatas_;
	std::string currentScene_;
};