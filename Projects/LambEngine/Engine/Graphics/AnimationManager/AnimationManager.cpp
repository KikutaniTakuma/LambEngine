#include "AnimationManager.h"
#include "Error/Error.h"
#include "Utils/SafeDelete/SafeDelete.h"

#include "../MeshLoader/MeshLoader.h"

Lamb::SafePtr<AnimationManager> AnimationManager::instance_ = nullptr;

void AnimationManager::Initialize() {
	if (instance_) {
		throw Lamb::Error::Code<AnimationManager>("Already created", ErrorPlace);
	}

	instance_.reset(new AnimationManager());

	Lamb::AddLog("Initialize AnimationManager succeeded");
}

void AnimationManager::Finalize() {
	instance_.reset();
	Lamb::AddLog("Finalize AnimationManager succeeded");
}

AnimationManager* const AnimationManager::GetInstance()
{
	return instance_.get();
}

void AnimationManager::LoadAnimations(const std::string& fileName) {
	auto isExist = animationData_.find(fileName);

	if (isExist == animationData_.end()) {
		animationData_[fileName] = std::make_unique<Animations>();

		*animationData_[fileName] = MeshLoader::LoadAnimation(fileName);
	}
}

Animations* const AnimationManager::GetAnimations(const std::string& fileName)
{
	if (not animationData_[fileName]) {
		throw Lamb::Error::Code<AnimationManager>("this file does not load -> " + fileName, ErrorPlace);
	}
	return animationData_[fileName].get();
}


