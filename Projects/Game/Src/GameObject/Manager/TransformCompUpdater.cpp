#include "TransformCompUpdater.h"

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
}
