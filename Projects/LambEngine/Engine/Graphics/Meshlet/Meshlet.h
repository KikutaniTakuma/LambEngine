#pragma once
#include <cstdint>
#include <vector>

#include "../GraphicsStructs.h"

struct Meshlet {
	uint32_t vertexOffset;
	uint32_t vertexCount;
	uint32_t primitiveOffset;
	uint32_t primitiveCount;
};

struct PrimitiveIndex {
	uint32_t index0 = 10u;
	uint32_t index1 = 10u;
	uint32_t index2 = 10u;
	uint32_t reserved = 2;
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t materialID;

	std::vector<Meshlet> meshlets;
	std::vector<uint32_t> uniqueVertexIndices;
	std::vector<PrimitiveIndex> primitiveIndices;
};

class MeshLoader final {
private:
	MeshLoader();
	
public:
	~MeshLoader() = default;

private:
	MeshLoader(const MeshLoader&) = delete;
	MeshLoader(MeshLoader&&) = delete;

	MeshLoader& operator=(const MeshLoader&) = delete;
	MeshLoader& operator=(MeshLoader&&) = delete;

public:
	static MeshLoader* const GetInstance();

private:
	static Lamb::SafePtr<MeshLoader> instance_;
};