#pragma once
#include <memory>
#include <unordered_set>

#include "Utils/SafePtr.h"

#include "../Comp/ObbPushComp.h"
#include "../Comp/LineCollisionComp.h"

class CompCollisionManager {
private:
	CompCollisionManager() = default;
	CompCollisionManager(const CompCollisionManager&) = delete;
	CompCollisionManager(CompCollisionManager&&) = delete;

	CompCollisionManager& operator=(const CompCollisionManager&) = delete;
	CompCollisionManager& operator=(CompCollisionManager&&) = delete;

public:
	~CompCollisionManager();

public:
	static CompCollisionManager* const GetInstance();

	static void Initialize();

	static void Finalize();

private:
	static std::unique_ptr<CompCollisionManager> pInstance_;

public:
	void Set(const Lamb::SafePtr<ObbComp>& obbComp);
	void Erase(const Lamb::SafePtr<ObbComp>& obbComp);
	void Set(const Lamb::SafePtr<ObbPushComp>& obbPushComp);
	void Erase(const Lamb::SafePtr<ObbPushComp>& obbPushComp);
	void Set(const Lamb::SafePtr<LineCollisionComp>& lineCollisionComp);
	void Erase(const Lamb::SafePtr<LineCollisionComp>& lineCollisionComp);

	void Collision();

	void Clear();

	void MakeCollisionPair();


private:
	std::unordered_set<Lamb::SafePtr<ObbComp>> obbComps_;
	std::unordered_set<Lamb::SafePtr<ObbPushComp>> obbPushComps_;
	std::unordered_set<Lamb::SafePtr<LineCollisionComp>> lineCollisionComps_;

	std::vector<std::pair<Lamb::SafePtr<ObbComp>, Lamb::SafePtr<ObbComp>>> collisionPairsObbObb_;
	std::vector<std::pair<Lamb::SafePtr<ObbPushComp>, Lamb::SafePtr<ObbPushComp>>> collisionPairsObbPushObbPush_;
	std::vector<std::pair<Lamb::SafePtr<LineCollisionComp>, Lamb::SafePtr<ObbComp>>> collisionPairsObbLine_;
};