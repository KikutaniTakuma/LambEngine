#pragma once
#include <memory>
#include <array>
#include "Drawer/Model/Model.h"


class Corals {
private:
	static constexpr size_t kLowCoralObjectNum_ = 22llu;

public:
	enum class ModelVertexLevel {
		LEVEL0_3584,
		LEVEL1_9866,
		LEVEL2_48522,
	};

public:
	Corals() = default;
	~Corals() = default;

private:
	Corals(const Corals&) = delete;
	Corals(Corals&&) = delete;

	Corals& operator=(const Corals&) = delete;
	Corals& operator=(Corals&&) = delete;

public:
	void Init(float32_t coralHeight = -8.0f, ModelVertexLevel modelLevel = ModelVertexLevel::LEVEL0_3584);

	void Draw(const float32_t4x4& cameraMat);

private:
	std::unique_ptr<std::array<ModelInstance, kLowCoralObjectNum_ * kLowCoralObjectNum_>> models_;
};