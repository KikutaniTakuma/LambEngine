#pragma once
#include <cstdint>
#include <vector>
#include <string>

#include "../GraphicsStructs.h"

struct ResMeshlet {
	uint32_t vertexOffset;
	uint32_t vertexCount;
	uint32_t primitiveOffset;
	uint32_t primitiveCount;
};

struct ResMesh {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t materialID;

	std::vector<ResMeshlet> meshlets;
	std::vector<uint32_t> uniqueVertexIndices;
	std::vector<uint8_t> primitiveIndices;
};

class MeshLoader final {
private:
	MeshLoader() = default;
	~MeshLoader() = default;
	
private:
	MeshLoader(const MeshLoader&) = delete;
	MeshLoader(MeshLoader&&) = delete;

	MeshLoader& operator=(const MeshLoader&) = delete;
	MeshLoader& operator=(MeshLoader&&) = delete;


public:
	static ResMesh* LoadMesh(const std::string& fileName);

	static void ParseMesh(
		ResMesh& dstMesh, 
		Lamb::SafePtr<const ModelData> pSrcMesh
	);
};