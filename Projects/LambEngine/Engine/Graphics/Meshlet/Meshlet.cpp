#include "Meshlet.h"

#include "../VertexIndexDataManager/VertexIndexDataManager.h"

#include "meshoptimizer.h"

#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"

ResMesh* MeshLoader::LoadMesh(const std::string& fileName)
{
	std::unique_ptr<ResMesh> mesh = std::make_unique<ResMesh>();


	Lamb::SafePtr vertexIndexdDataManager = VertexIndexDataManager::GetInstance();
	vertexIndexdDataManager->LoadModel(fileName);

	Lamb::SafePtr<const ModelData> modelData = vertexIndexdDataManager->GetModelData(fileName);


	ParseMesh(*mesh, modelData);

	return mesh.release();
}

ResMesh* MeshLoader::LoadMesh(Lamb::SafePtr<const ModelData> pSrcMesh)
{
	std::unique_ptr<ResMesh> mesh = std::make_unique<ResMesh>();
	ParseMesh(*mesh, pSrcMesh);

	return mesh.release();
}

void MeshLoader::ParseMesh(ResMesh& dstMesh, Lamb::SafePtr<const ModelData> pSrcMesh) {
	// Nullチェック
	pSrcMesh.NullCheck(FilePlace);

	// 頂点データセット
	dstMesh.vertices = pSrcMesh->vertices;

	// インデックスデータセット
	dstMesh.indices = pSrcMesh->indices;

	// 最適化
	{
		std::vector<uint32_t> remap(dstMesh.indices.size());

		auto vertexCount = meshopt_generateVertexRemap(
			remap.data(),
			dstMesh.indices.data(),
			dstMesh.indices.size(),
			dstMesh.vertices.data(),
			dstMesh.vertices.size(),
			sizeof(decltype(pSrcMesh->vertices)::value_type)
		);

		std::vector<Vertex> vertices(vertexCount);
		std::vector<uint32_t> indices(dstMesh.indices.size());

		// 頂点インデックスを再マッピング
		meshopt_remapIndexBuffer(
			indices.data(),
			dstMesh.indices.data(),
			dstMesh.indices.size(),
			remap.data()
		);

		// 頂点データを再マッピング
		meshopt_remapVertexBuffer(
			vertices.data(),
			dstMesh.vertices.data(),
			dstMesh.vertices.size(),
			sizeof(decltype(pSrcMesh->vertices)::value_type),
			remap.data()
		);

		
		// メモリクリア
		remap.clear();
		remap.shrink_to_fit();

		dstMesh.vertices.resize(vertices.size());
		dstMesh.indices.resize(indices.size());

		// 頂点キャッシュ最適化
		meshopt_optimizeVertexCache(
			dstMesh.indices.data(),
			indices.data(),
			indices.size(),
			vertexCount
		);

		// メモリクリア
		indices.clear();
		indices.shrink_to_fit();

		
		// 頂点フェッチ最適化
		meshopt_optimizeVertexFetch(
			dstMesh.vertices.data(),
			dstMesh.indices.data(),
			dstMesh.indices.size(),
			vertices.data(),
			vertices.size(),
			sizeof(decltype(pSrcMesh->vertices)::value_type)
		);

		// メモリクリア
		vertices.clear();
		vertices.shrink_to_fit();
	}

	// メッシュレット生成
	{
		constexpr size_t kMaxVertices = 64;
		constexpr size_t kMaxPrimitives = 128;

		size_t maxMeshlets = meshopt_buildMeshletsBound(
			dstMesh.indices.size(),
			kMaxVertices,
			kMaxPrimitives
		);

		std::vector<meshopt_Meshlet> meshlets(maxMeshlets);

		dstMesh.uniqueVertexIndices.resize(maxMeshlets * kMaxVertices);
		dstMesh.primitiveIndices.resize(maxMeshlets * kMaxVertices * 3);

		size_t meshletCount = meshopt_buildMeshlets(
			meshlets.data(), 
			dstMesh.uniqueVertexIndices.data(),
			dstMesh.primitiveIndices.data(),
			dstMesh.indices.data(),
			dstMesh.indices.size(),
			dstMesh.vertices.front().pos.data(),
			dstMesh.vertices.size(),
			sizeof(Vertex), 
			kMaxVertices,
			kMaxPrimitives,
			0.0f
		);

		meshlets.resize(meshletCount);

		const meshopt_Meshlet& last = meshlets.back();

		dstMesh.uniqueVertexIndices.resize(last.vertex_offset + last.vertex_count);
		dstMesh.primitiveIndices.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));
		meshlets.resize(meshletCount);

		dstMesh.uniqueVertexIndices.shrink_to_fit();
		dstMesh.primitiveIndices.shrink_to_fit();

		for (auto& m : meshlets) {
			meshopt_optimizeMeshlet(
				&dstMesh.uniqueVertexIndices[m.vertex_offset],
				&dstMesh.primitiveIndices[m.triangle_offset],
				m.triangle_count,
				m.vertex_count
			);
		}

		dstMesh.meshlets.resize(meshlets.size());
		uint32_t primitiveCount = 0;

		for (size_t i = 0; i < meshlets.size(); ++i) {
			dstMesh.meshlets[i].vertexOffset = meshlets[i].vertex_offset;
			dstMesh.meshlets[i].vertexCount = meshlets[i].vertex_count;
			dstMesh.meshlets[i].primitiveOffset = meshlets[i].triangle_offset;
			dstMesh.meshlets[i].primitiveCount = meshlets[i].triangle_count;
			primitiveCount += dstMesh.meshlets[i].primitiveCount;
		}

		// プリミティブインデックスをパックする
		dstMesh.packedPrimitiveIndices.resize(primitiveCount);
		for (size_t count = 0; auto & i : dstMesh.packedPrimitiveIndices) {
			i = static_cast<uint32_t>(dstMesh.primitiveIndices[count + 0]);
			i <<= 8;
			i += static_cast<uint32_t>(dstMesh.primitiveIndices[count + 1]);
			i <<= 8;
			i += static_cast<uint32_t>(dstMesh.primitiveIndices[count + 2]);

			count += 3;
		}

		dstMesh.meshlets.shrink_to_fit();
	}


}

std::unique_ptr<MeshletManager> MeshletManager::instance_;

MeshletManager* const MeshletManager::GetInstance() {
	return instance_.get();
}

void MeshletManager::Initialize() {
	instance_.reset(new MeshletManager());
}

void MeshletManager::Finalize() {
	instance_.reset();
}

void MeshletManager::LoadMesh(const std::string& fileName) {
	if (not meshlets_.contains(fileName)) {
		meshlets_.insert(std::make_pair(fileName, std::make_pair(MeshLoader::LoadMesh(fileName), std::make_unique<MeshShaderData>())));

		auto& meshAndMeshData = meshlets_[fileName];

		// ディスクリプタヒープ
		CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

		auto& resMesh = meshAndMeshData.first;
		auto& shaderData = meshAndMeshData.second;

		shaderData->gVertices.Create(static_cast<uint32_t>(resMesh->vertices.size()));
		shaderData->gUniqueVertexIndices.Create(static_cast<uint32_t>(resMesh->uniqueVertexIndices.size()));
		shaderData->gUniquePrimitiveIndices.Create(static_cast<uint32_t>(resMesh->packedPrimitiveIndices.size()));
		shaderData->gMeshlets.Create(static_cast<uint32_t>(resMesh->meshlets.size()));

		for (size_t count = 0; auto & vertex : resMesh->vertices) {
			shaderData->gVertices[count] = vertex;
			count++;
		}

		for (size_t count = 0; auto & index : resMesh->uniqueVertexIndices) {
			shaderData->gUniqueVertexIndices[count] = index;
			count++;
		}

		for (size_t count = 0; auto & primitiveIndex : resMesh->packedPrimitiveIndices) {
			shaderData->gUniquePrimitiveIndices[count] = primitiveIndex;
			count++;
		}

		for (size_t count = 0; auto & meshlet : resMesh->meshlets) {
			shaderData->gMeshlets[count] = meshlet;
			count++;
		}

		shaderData->meshletCount = static_cast<uint32_t>(shaderData->gMeshlets.size());



		descriptorHeap->BookingHeapPos(5);
		descriptorHeap->CreateView(shaderData->gVertices);
		descriptorHeap->CreateView(shaderData->gUniqueVertexIndices);
		descriptorHeap->CreateView(shaderData->gUniquePrimitiveIndices);
		descriptorHeap->CreateView(shaderData->gMeshlets);
		descriptorHeap->CreateView(shaderData->gTransform);



		shaderData->gVertices.OffWright();
		shaderData->gUniqueVertexIndices.OffWright();
		shaderData->gUniquePrimitiveIndices.OffWright();
		shaderData->gMeshlets.OffWright();
		shaderData->gTransform.OffWright();
	}
}

const std::pair<std::unique_ptr<ResMesh>, std::unique_ptr<MeshShaderData>>& MeshletManager::GetMesh(const std::string& fileName)
{
	if (meshlets_.contains(fileName)) {
		return meshlets_[fileName];
	}
	else {
		throw Lamb::Error::Code<MeshletManager>("not loaded meshlet", ErrorPlace);
	}
}
