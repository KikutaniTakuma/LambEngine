#pragma once
#include <memory>
#include <unordered_set>
#include <list>

#include "Utils/SafePtr.h"

#include "../Object.h"

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

private:
	static std::unique_ptr<ObjectManager> instance_;

public:
	void Set(const Lamb::SafePtr<Object>& object);
	void Erase(const Lamb::SafePtr<Object>& object);

	void SetCamera(const Lamb::SafePtr<Camera>& camera);

	void Update();

	void Draw();

private:
	std::unordered_set<Lamb::SafePtr<Object>> objects_;
	std::list<const Lamb::SafePtr<class ObbPushComp>> obbObjects_;
};