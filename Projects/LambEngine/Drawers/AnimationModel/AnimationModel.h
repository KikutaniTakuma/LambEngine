#pragma once
#include "../Model/Model.h"
#include "Engine/Graphics/Animator/Animator.h"

class AnimationModel : public Model {
public:
	AnimationModel() = default;
	AnimationModel(const std::string& fileName);
	AnimationModel(const AnimationModel&) = default;
	AnimationModel(AnimationModel&&) = default;
	virtual ~AnimationModel() = default;

	AnimationModel& operator=(const AnimationModel&) = default;
	AnimationModel& operator=(AnimationModel&&) = default;

public:
	virtual void Load(const std::string& fileName) override;

	virtual void Update();

	virtual void Draw(const Camera* camera, BlendType blend, bool isLighting) override;

private:
	void Draw(const Camera* camera, BlendType blend) override;

public:
	Animator animator;
};