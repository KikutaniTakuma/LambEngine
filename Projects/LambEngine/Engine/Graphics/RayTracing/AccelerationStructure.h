#pragma once
#include <d3d12.h>
#include <vector>

#include "Utils/SafePtr.h"

struct BLAS{
	D3D12_RAYTRACING_GEOMETRY_DESC geometry = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
};

struct TLAS {
	std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDesces;
};


Lamb::SafePtr<BLAS> CreateBLAS(
	Lamb::SafePtr<class VertexIndexData> viData
);