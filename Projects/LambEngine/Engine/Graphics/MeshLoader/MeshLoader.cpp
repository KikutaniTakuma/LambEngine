#include "MeshLoader.h"
#include "Error/Error.h"
#include "../TextureManager/TextureManager.h"
#include "../../Core/DirectXDevice/DirectXDevice.h"

#include <filesystem>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Mesh MeshLoader::LoadObj(const std::string& fileName)
{
	if (not std::filesystem::exists(fileName.c_str())) {
		throw Lamb::Error::Code<MeshLoader>("this file does not find -> " + fileName, __func__);
	}

	std::unordered_map<Vertex, size_t> vertices;
	size_t vertexCount = 0llu;
	std::vector<uint32_t> indices;
	uint32_t indexCount = 0;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	if (not scene->HasMeshes()) {
		throw Lamb::Error::Code<MeshLoader>("this file does not have meshes -> " + fileName, __func__);
	}

	std::string directorypath = std::filesystem::path(fileName).parent_path().string();
	std::vector<std::string> textureFileNames;

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			textureFileNames.push_back(directorypath + "/" + textureFilePath.C_Str());
		}
	}

	std::vector<Texture*> textures;
	TextureManager* const textureMaanger = TextureManager::GetInstance();

	for (const auto& i : textureFileNames) {
		textures.push_back(textureMaanger->LoadTexture(i));
	}

	// mesh解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		if (not mesh->HasNormals()) {
			throw Lamb::Error::Code<MeshLoader>("this file does not have normal -> " + fileName, __func__);
		}
		if (not mesh->HasTextureCoords(0)) {
			throw Lamb::Error::Code<MeshLoader>("this file does not have texcoord -> " + fileName, __func__);
		}

		// face解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			if (face.mNumIndices != 3) {
				throw Lamb::Error::Code<MeshLoader>("this file does not surport -> " + fileName, __func__);
			}

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				Vertex vertex;
				vertex.pos = Vector4{ -position.x, position.y, position.z, 1.0f };
				vertex.normal = Vector3{ -normal.x, normal.y, normal.z };
				vertex.uv = Vector2{ texcoord.x, texcoord.y };

				uint32_t materialIndex = mesh->mMaterialIndex - 1;

				vertex.texIndex = textures.empty() ? 0 : textures[materialIndex]->GetHandleUINT();

				// 頂点コンテナが空
				if (vertices.empty()) {
					// 追加
					vertices.insert(std::make_pair(vertex, vertexCount));
					vertexCount++;

					// インデックス追加
					indices.emplace_back(indexCount);
					indexCount++;
				}
				// それ以外
				else {
					// そもそも同じ頂点が追加されているか
					auto isExist = vertices.find(vertex);

					// 追加されてない
					if (isExist == vertices.end()) {
						vertices.insert(std::make_pair(vertex, vertexCount));
						vertexCount++;

						// インデックス追加
						indices.emplace_back(indexCount);
						indexCount++;
					}
					// 追加してた
					else {
						// インデックス追加
						indices.emplace_back(static_cast<uint32_t>(vertices[vertex]));
					}
				}
			}
		}
	}

	std::vector<Vertex> verticesTmp;
	verticesTmp.resize(vertices.size());
	for (const auto& i : vertices) {
		verticesTmp[i.second] = i.first;
	}


	Mesh result;
	uint32_t indexSizeInBytes = static_cast<uint32_t>(sizeof(uint32_t) * indices.size());
	uint32_t vertexSizeInBytes = static_cast<uint32_t>(sizeof(Vertex) * verticesTmp.size());

	result.indexResource = DirectXDevice::GetInstance()->CreateBufferResuorce(indexSizeInBytes);

	uint32_t* indexMap = nullptr;
	result.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	std::copy(indices.begin(), indices.end(), indexMap);
	result.indexResource->Unmap(0, nullptr);

	result.indexNumber = static_cast<uint32_t>(indices.size());
	result.indexView.SizeInBytes = indexSizeInBytes;
	result.indexView.Format = DXGI_FORMAT_R32_UINT;
	result.indexView.BufferLocation = result.indexResource->GetGPUVirtualAddress();


	result.vertexResource = DirectXDevice::GetInstance()->CreateBufferResuorce(vertexSizeInBytes);
	
	Vertex* vertMap = nullptr;
	result.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	std::copy(verticesTmp.begin(), verticesTmp.end(), vertMap);
	result.vertexResource->Unmap(0, nullptr);

	result.vertexNumber = static_cast<uint32_t>(verticesTmp.size());
	result.vertexView.SizeInBytes = vertexSizeInBytes;
	result.vertexView.StrideInBytes = static_cast<uint32_t>(sizeof(Vertex));
	result.vertexView.BufferLocation = result.vertexResource->GetGPUVirtualAddress();

	return result;
}

std::unordered_map<std::string, uint32_t> MeshLoader::LoadMtl([[maybe_unused]]const std::string& fileName)
{

	//std::ifstream file{ fileName };
	//if (file.fail()) {
	//	if (!std::filesystem::exists(fileName)) {
	//		throw Lamb::Error::Code<Mesh>("this file is not exist -> " + fileName, __func__);
	//	}
	//	else {
	//		throw Lamb::Error::Code<Mesh>("something error -> " + fileName, __func__);
	//	}
	//}

	//TextureManager* const textureManager = TextureManager::GetInstance();

	std::unordered_map<std::string, uint32_t> result;

	/*std::string lineBuf;
	std::unordered_map<std::string, uint32_t>::iterator texItr;

	std::string useMtlName;
	while (std::getline(file, lineBuf)) {
		std::string identifier;
		std::istringstream line(lineBuf);

		line >> identifier;
		if (identifier == "map_Kd") {
			std::string texName;
			std::filesystem::path path = fileName;

			line >> texName;

			Texture* tex = textureManager->LoadTexture(path.parent_path().string() + "/" + texName);
			if (tex == nullptr || !(*tex)) {
				texItr->second = textureManager->GetWhiteTex()->GetHandleUINT();
			}
			else {
				texItr->second = tex->GetHandleUINT();
			}
		}
		else if (identifier == "newmtl") {
			line >> useMtlName;
			result[useMtlName];
			texItr = result.find(useMtlName);
		}
	}*/

	return result;
}
