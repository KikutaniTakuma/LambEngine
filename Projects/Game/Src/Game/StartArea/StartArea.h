#pragma once

#include <memory>

#include "Drawers/Model/Model.h"

class Camera;
class StartArea {
public:
	void Initialize();
	void Update();
	void ResourceUpdate();
	void Draw(const Camera& camera);
private:
	Lamb::SafePtr<Model> start_;
	std::unique_ptr<Model::Instance> startInstance_;
	Lamb::SafePtr<Model> leftBeach_;
	std::unique_ptr<Model::Instance> leftBeachInstance_;
	Lamb::SafePtr<Model> rightBeach_;
	std::unique_ptr<Model::Instance> rightBeachInstance_;

};