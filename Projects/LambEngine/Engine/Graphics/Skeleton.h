#pragma once
#include <optional>
#include <vector>
#include <unordered_map>

#include "Transform/Transform.h"


struct Joint {
	Joint() = default;
	Joint(const Joint&) = default;
	Joint(Joint&&) = default;
	~Joint() = default;

	Joint& operator=(const Joint&) = default;
	Joint& operator=(Joint&&) = default;

	QuaternionTransform transform;
	Mat4x4 localMatrix;
	Mat4x4 skeletonSpaceMatrix;
	std::string name;
	std::vector<int32_t> children;
	int32_t index = 0;
	std::optional<int32_t> parent;
};

struct Skeleton {
	Skeleton() = default;
	Skeleton(const Skeleton&) = default;
	Skeleton(Skeleton&&) = default;
	~Skeleton() = default;

	Skeleton& operator=(const Skeleton&) = default;
	Skeleton& operator=(Skeleton&&) = default;

	void Update();
	void Draw(const Mat4x4& camera);

	int32_t root = 0;
	std::unordered_map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};

Skeleton CreateSkeleton(const struct Node& rootNode);
int32_t CreateJoint(
	const struct Node& rootNode,
	const std::optional<int32_t> parent,
	std::vector<Joint>& joints
);