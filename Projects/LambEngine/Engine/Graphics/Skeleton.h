#pragma once
#include <optional>
#include <vector>
#include <unordered_map>

#include "Transform/Transform.h"
#include "GraphicsStructs.h"

struct VertexInfluence {
	static constexpr uint32_t kNumMaxInfluence = 4u;
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGpu {
	Mat4x4 skeletonSoaceMatrix;
	Mat4x4 skeletonSpaceInverseTransposeMatrix;
};

struct SkinCluster {
	std::vector<Mat4x4> inversebindPoseMatrices;
	Lamb::LambPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW infliuenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	StructuredBuffer<WellForGpu> paletteBuffer;
};

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

namespace Lamb {
	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(
		const Node& rootNode,
		const std::optional<int32_t> parent,
		std::vector<Joint>& joints
	);

	SkinCluster CreateAkinCluster();
}