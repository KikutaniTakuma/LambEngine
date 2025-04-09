#include "AccelerationStructure.h"

#include "Engine/Graphics/VertexIndexDataManager/VertexIndexDataManager.h"

Lamb::SafePtr<BLAS> CreateBLAS(Lamb::SafePtr<class VertexIndexData> viData)
{
	Lamb::SafePtr viDataManager = VertexIndexDataManager::GetInstance();

	std::unique_ptr<BLAS> blas = std::make_unique<BLAS>();
	blas->geometry = {};
	blas->geometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	blas->geometry.Triangles.VertexBuffer.StartAddress = viData->vertexResource->GetGPUVirtualAddress();
	blas->geometry.Triangles.VertexBuffer.StrideInBytes = viData->vertexView.StrideInBytes;
	blas->geometry.Triangles.VertexCount = viData->vertexNumber;
	blas->geometry.Triangles.IndexBuffer = viData->indexResource->GetGPUVirtualAddress();
	blas->geometry.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	blas->geometry.Triangles.IndexCount = viData->indexNumber;
	blas->geometry.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	blas->inputs = {};
	blas->inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	blas->inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	blas->inputs.NumDescs = 1;
	blas->inputs.pGeometryDescs = &(blas->geometry);
	blas->inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;


	return blas.release();
}
