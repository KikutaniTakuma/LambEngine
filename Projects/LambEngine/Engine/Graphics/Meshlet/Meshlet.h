#pragma once
#include <cstdint>

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

