#pragma once
#include "Drawers/Model/Model.h"
#include <memory>

class SkyDome final {
public:
	SkyDome() = default;
	SkyDome(const SkyDome&) = delete;
	SkyDome(SkyDome&&) = delete;
	~SkyDome() = default;

	SkyDome& operator=(const SkyDome&) = delete;
	SkyDome& operator=(SkyDome&&) = delete;

public:
	void Initialize();

	void Upadate();

	void Draw(const class Camera& camera);

public:
	void SetTexture(Texture* const tex);

public:
	Vector3 pos;
	Vector3 rotate;
	Vector3 scale;

private:
	std::unique_ptr<Model> model_;
};