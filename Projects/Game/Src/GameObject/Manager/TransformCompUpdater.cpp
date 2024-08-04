#include "TransformCompUpdater.h"
#ifdef _DEBUG
#include "../Comp/CameraComp.h"
#include "imgui.h"
#endif // _DEBUG


std::unique_ptr<TransformCompUpdater> TransformCompUpdater::instance_;

TransformCompUpdater::~TransformCompUpdater() {
	transformComps_.clear();
}

TransformCompUpdater* const TransformCompUpdater::GetInstance() {
	return instance_.get();
}

void TransformCompUpdater::Initialize(){
	if(instance_){
		return;
	}
	instance_.reset(new TransformCompUpdater());
}

void TransformCompUpdater::Finalize()
{
	instance_.reset();
}

void TransformCompUpdater::Set(const Lamb::SafePtr<class TransformComp>& transformComp) {
	if (not transformComps_.contains(transformComp)) {
#ifdef _DEBUG
		transformComp->SetGuizmoID(setID);
#endif // _DEBUG
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

#ifdef _DEBUG
void TransformCompUpdater::Guizmo(CameraComp* cameraComp) {
	for (auto& i : transformComps_) {
		i->Guizmo(cameraComp);
	}
}
#endif // _DEBUG

