#pragma once
#include <memory>
#include <unordered_set>

#include "Utils/SafePtr.h"

#include "../Comp/TransformComp.h"

class TransformCompUpdater {
private:
	TransformCompUpdater() = default;
	TransformCompUpdater(const TransformCompUpdater&)  =delete;
	TransformCompUpdater(TransformCompUpdater&&) = delete;

	TransformCompUpdater& operator=(const TransformCompUpdater&) = delete;
	TransformCompUpdater& operator=(TransformCompUpdater&&) = delete;

public:
	~TransformCompUpdater();

public:
	static TransformCompUpdater* const GetInstance();

	static void Initialize();

	static void Finalize();

private:
	static std::unique_ptr<TransformCompUpdater> instance_;

public:
	void Set(const Lamb::SafePtr<TransformComp>& transformComp);
	void Erase(const Lamb::SafePtr<TransformComp>& transformComp);

	void UpdateMatrix();

	void SetCurretnGuizmoID(uint32_t currentGuizmoID);
	uint32_t GetGuizmoID() const;

#ifdef _DEBUG
	void Guizmo(CameraComp* cameraComp);
#endif // _DEBUG


private:
	std::unordered_set<Lamb::SafePtr<TransformComp>> transformComps_;
	uint32_t currentGuizmoID_;
	uint32_t setID = 0;
};