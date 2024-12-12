#include "TransformCompUpdater.h"
#ifdef USE_DEBUG_CODE
#include "../Comp/CameraComp.h"
#include "imgui.h"
#endif // USE_DEBUG_CODE


std::unique_ptr<TransformCompUpdater> TransformCompUpdater::pInstance_;

TransformCompUpdater::~TransformCompUpdater() {
	transformComps_.clear();
}

TransformCompUpdater* const TransformCompUpdater::GetInstance() {
	return pInstance_.get();
}

void TransformCompUpdater::Initialize(){
	if(pInstance_){
		return;
	}
	pInstance_.reset(new TransformCompUpdater());
}

void TransformCompUpdater::Finalize()
{
	pInstance_.reset();
}

void TransformCompUpdater::Set(const Lamb::SafePtr<class TransformComp>& transformComp) {
	if (not transformComps_.contains(transformComp)) {
#ifdef USE_DEBUG_CODE
		transformComp->SetGuizmoID(setID);
#endif // USE_DEBUG_CODE
		setID++;
		transformComps_.insert(transformComp);
	}
}

void TransformCompUpdater::Erase(const Lamb::SafePtr<class TransformComp>& transformComp) {
	if (transformComps_.contains(transformComp)) {
		transformComps_.erase(transformComp);
	}
}

void TransformCompUpdater::UpdateMatrix() {
	for (auto& i : transformComps_) {
		i->UpdateMatrix();
	}
	for (auto& i : transformComps_) {
		if (not i->HaveParent()) {
			i->UpdateChildrenMatrix();
		}
	}
}

void TransformCompUpdater::SetCurretnGuizmoID(uint32_t currentGuizmoID)
{
	currentGuizmoID_ = currentGuizmoID;
}

uint32_t TransformCompUpdater::GetGuizmoID() const
{
	return currentGuizmoID_;
}

#ifdef USE_DEBUG_CODE
void TransformCompUpdater::Guizmo(CameraComp* cameraComp) {
	for (auto& i : transformComps_) {
		i->Guizmo(cameraComp);
	}
}
#endif // USE_DEBUG_CODE

