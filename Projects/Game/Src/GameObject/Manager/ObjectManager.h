#pragma once
#include <memory>
#include <unordered_set>
#include <list>

#include "Utils/SafePtr.h"

#include "../Object.h"

#include "Level/LevelData.h"

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
	void SetLevelData(Lamb::SafePtr<LevelData> levelData, Lamb::SafePtr<Camera> camera);

	const Mat4x4& GetCameraMatrix() const;
	const Vector3& GetCameraPos() const;

	void Set(const Lamb::SafePtr<Object>& object);
	void Erase(const Lamb::SafePtr<Object>& object);

	void Clear();

	void SetCamera(const Lamb::SafePtr<Camera>& camera);
	bool SetCamera();

	void Update();

	void Draw();

private:
	std::unordered_set<Lamb::SafePtr<Object>> objects_;
	std::list<Lamb::SafePtr<class ObbPushComp>> obbObjects_;
	Lamb::SafePtr<class Camera3DComp> cameraComp_;
	Lamb::SafePtr<Camera> camera_;
};