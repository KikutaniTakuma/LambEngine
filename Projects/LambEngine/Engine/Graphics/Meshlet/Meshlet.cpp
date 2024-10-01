#include "Meshlet.h"

#include "../VertexIndexDataManager/VertexIndexDataManager.h"

#include "DirectXMesh/DirectXMesh.h"

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

	// メッシュレット作成用
	std::vector<uint32_t> indices;
	std::vector<DirectX::XMFLOAT3> positions;

	indices.resize(dstMesh.indices.size());
	positions.resize(dstMesh.vertices.size());

	// データ代入
	std::copy(dstMesh.indices.cbegin(), dstMesh.indices.cend(), indices.begin());
	for (size_t count = 0; const auto & vertex : dstMesh.vertices) {
		SetVector3ToDirectXXMFLOAT3(positions[count], vertex.pos.GetVector3());
		count++;
	}

	// 出力変数
	std::vector<DirectX::Meshlet> meshlets;
	std::vector<uint8_t> uniqueVertexIB;
	std::vector<DirectX::MeshletTriangle> primitiveIndices;

	// メッシュレット作成
	HRESULT hr = DirectX::ComputeMeshlets(
		indices.data(), indices.size() / 3,
		positions.data(), positions.size(),
		nullptr,
		meshlets,
		uniqueVertexIB,
		primitiveIndices,
		kMaxMeshletVertices, 
		kMaxMeshletPrimirives
	);

	// 失敗したらthrowする
	if (FAILED(hr)) {
		throw Lamb::Error::Code<MeshLoader>("DirectX::ComputeMeshlets failed", ErrorPlace);
	}


	// データを設定

	// meshletの数が1024を超えていた場合、meshletを分ける
	if (kMaxThreadGroup < meshlets.size()) {
		// サイズ
		size_t divisionMeshletNum = (meshlets.size() / kMaxThreadGroup);
		// 余り
		size_t remainder = (meshlets.size() % kMaxThreadGroup);

		if (remainder != 0) {
			divisionMeshletNum += 1;
		}

		// 分割する分の要素を確保
		dstMesh.meshletsArray.resize(divisionMeshletNum);

		// 生成したmeshletのイテレーター
		auto meshletsItr = meshlets.begin();

		for (size_t count = 0; auto& meshletsElement : dstMesh.meshletsArray) {
			// 最後だけ余りの数にする
			// それ以外はThreadGroupの最大数
			bool isLast = not (count < (divisionMeshletNum - 1));
			size_t meshletSize = isLast ? remainder : kMaxThreadGroup;

			// 要素確保
			meshletsElement.resize(meshletSize);
			// 代入していく
			std::copy(meshletsItr, meshletsItr + meshletSize, meshletsElement.begin());

			// メモリ最適化
			meshletsElement.shrink_to_fit();

			// イテレータを進める
			meshletsItr = meshletsItr + meshletSize;
			count++;
		}
	}
	else {
		dstMesh.meshletsArray.resize(1);
		dstMesh.meshletsArray.push_back(std::move(meshlets));
	}

	dstMesh.uniqueVertexIndices.resize(uniqueVertexIB.size());
	std::memcpy(dstMesh.uniqueVertexIndices.data(), uniqueVertexIB.data(), sizeof(uint8_t) * uniqueVertexIB.size());

	dstMesh.primitiveIndices.resize(primitiveIndices.size());
	for (size_t count = 0; auto& primitiveIndex : primitiveIndices) {
		dstMesh.primitiveIndices[count] = primitiveIndex.i0;
		dstMesh.primitiveIndices[count] <<= 10;
		dstMesh.primitiveIndices[count] += primitiveIndex.i1;
		dstMesh.primitiveIndices[count] <<= 10;
		dstMesh.primitiveIndices[count] += primitiveIndex.i2;


		count++;
	}

	std::memcpy(dstMesh.primitiveIndices.data(), primitiveIndices.data(), sizeof(DirectX::MeshletTriangle) * primitiveIndices.size());
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

		auto& resMesh = meshAndMeshData.first;
		auto& shaderData = meshAndMeshData.second;

		// meshletsの数分だけ確保
		shaderData->gMeshletsArray.resize(resMesh->meshletsArray.size());
		// メモリを最適化
		shaderData->gMeshletsArray.shrink_to_fit();

		for (size_t i = 0; i < shaderData->gMeshletsArray.size(); i++) {
			// SturucturedBufferを作成
			shaderData->gMeshletsArray[i].Create(static_cast<uint32_t>(resMesh->meshletsArray[i].size()));

			// 値を代入
			shaderData->gMeshletsArray[i].MemCpy(
				resMesh->meshletsArray[i].data(),
				sizeof(DirectX::Meshlet) * resMesh->meshletsArray[i].size()
			);
		}

		// SturucturedBufferを作成
		shaderData->gVertices.Create(static_cast<uint32_t>(resMesh->vertices.size()));
		shaderData->gUniqueVertexIndices.Create(static_cast<uint32_t>(resMesh->uniqueVertexIndices.size()));
		shaderData->gPrimitiveIndices.Create(static_cast<uint32_t>(resMesh->primitiveIndices.size()));

		// 値を代入
		shaderData->gVertices.MemCpy(resMesh->vertices.data(), sizeof(Vertex) * resMesh->vertices.size());
		shaderData->gUniqueVertexIndices.MemCpy(resMesh->uniqueVertexIndices.data(), sizeof(uint32_t) * resMesh->uniqueVertexIndices.size());
		shaderData->gPrimitiveIndices.MemCpy(resMesh->primitiveIndices.data(), sizeof(uint32_t) * resMesh->primitiveIndices.size());
		
		// meshletsの数分だけ確保
		shaderData->meshletCounts.resize(shaderData->gMeshletsArray.size());
		// メモリを最適化
		shaderData->meshletCounts.shrink_to_fit();

		// 各meshletsのサイズを代入
		for (size_t i = 0; i < shaderData->meshletCounts.size(); i++) {
			shaderData->meshletCounts[i] = static_cast<uint32_t>(shaderData->gMeshletsArray[i].size());
		}
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
