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
	std::unique_ptr<ModelInstance> start_;
	std::unique_ptr<ModelInstance> leftBeach_;
	std::unique_ptr<ModelInstance> rightBeach_;

};