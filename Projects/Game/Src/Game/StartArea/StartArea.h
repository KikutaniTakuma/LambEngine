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
	std::unique_ptr<Model> start_;
	std::unique_ptr<Model> leftBeach_;
	std::unique_ptr<Model> rightBeach_;

};