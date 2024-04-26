#include "MeshLoader.h"
#include "Error/Error.h"
#include "Utils/SafePtr/SafePtr.h"
#include "../TextureManager/TextureManager.h"
#include "../../Core/DirectXDevice/DirectXDevice.h"

#include <filesystem>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::unordered_map<std::string, Mesh::ResourceData> MeshLoader::LoadModel(
	const std::string& fileName, 
	std::unordered_map<std::string, Texture*>& texs
) {
	std::unordered_map<std::string, Mesh::ResourceData> result;

	Assimp::Importer importer;
	Lamb::SafePtr<const aiScene> scene = ReadFile(importer, fileName);
	if (not scene->HasMeshes()) {
		throw Lamb::Error::Code<MeshLoader>("this file does not have meshes -> " + fileName, __func__);
	}

	std::filesystem::path path = fileName;
	//bool isGltf = (path.extension() == ".gltf");

	std::vector<Mesh::VertData> vertices;


	std::string&& directorypath = path.parent_path().string();
	std::vector<Texture*> textures;

	LoadMtl(scene.get(), directorypath, textures);
	Texture* whiteTex = TextureManager::GetInstance()->GetWhiteTex();

	Node node = ReadNode(scene->mRootNode);
	

	// mesh解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		Lamb::SafePtr<aiMesh> mesh = scene->mMeshes[meshIndex];
		if (not mesh->HasNormals()) {
			throw Lamb::Error::Code<MeshLoader>("this file does not have normal -> " + fileName, __func__);
		}
		if (not mesh->HasTextureCoords(0)) {
			throw Lamb::Error::Code<MeshLoader>("this file does not have texcoord -> " + fileName, __func__);
		}

		uint32_t materialIndex = mesh->mMaterialIndex - 1;
		texs[mesh->mName.C_Str()] = (textures.empty() or textures.size() <= materialIndex) ? whiteTex : textures[materialIndex];

		result[mesh->mName.C_Str()].tex = (textures.empty() or textures.size() <= materialIndex) ? whiteTex : textures[materialIndex];


		// 要素数追加
		vertices.reserve(mesh->mNumVertices);

		// face解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			if (face.mNumIndices != 3) {
				throw Lamb::Error::Code<MeshLoader>("this file does not support -> " + fileName, __func__);
			}

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				Mesh::VertData vertex;
				vertex.position = Vector4{ -position.x, position.y, position.z, 1.0f };
				vertex.normal = Vector3{ -normal.x, normal.y, normal.z };
				vertex.uv = Vector2{ texcoord.x, texcoord.y };

				vertices.push_back(vertex);
			}
		}

		uint32_t vertexSizeInBytes = static_cast<uint32_t>(sizeof(Mesh::VertData) * vertices.size());

		result[mesh->mName.C_Str()].resource.first = DirectXDevice::GetInstance()->CreateBufferResuorce(vertexSizeInBytes);

		Mesh::VertData* vertMap = nullptr;
		result[mesh->mName.C_Str()].resource.first->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		std::copy(vertices.begin(), vertices.end(), vertMap);
		result[mesh->mName.C_Str()].resource.first->Unmap(0, nullptr);

		result[mesh->mName.C_Str()].vertNum = static_cast<uint32_t>(vertices.size());
		result[mesh->mName.C_Str()].resource.second.SizeInBytes = vertexSizeInBytes;
		result[mesh->mName.C_Str()].resource.second.StrideInBytes = static_cast<uint32_t>(sizeof(Mesh::VertData));
		result[mesh->mName.C_Str()].resource.second.BufferLocation = result[mesh->mName.C_Str()].resource.first->GetGPUVirtualAddress();
		
		result[mesh->mName.C_Str()].node = node;
		vertices.clear();
	}

	return result;
}

Animations MeshLoader::LoadAnimation(const std::string& fileName)
{
	Animations result;
	Assimp::Importer importer;
	Lamb::SafePtr<const aiScene> scene = ReadFile(importer, fileName);
	if (not (scene->mNumAnimations != 0)) {
		throw Lamb::Error::Code<MeshLoader>("this file does not have meshes -> " + fileName, __func__);
	}

	result.data.resize(scene->mNumAnimations);

	for (uint32_t animtionIndex = 0; animtionIndex < scene->mNumAnimations; animtionIndex++) {
		Lamb::SafePtr<aiAnimation> animationAssimp = scene->mAnimations[animtionIndex];
		Animation& animation = result.data[animtionIndex];
		animation.duration = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);
		
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; channelIndex++) {
			Lamb::SafePtr<aiNodeAnim> nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

			// Translation
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex++) {
				aiVectorKey& translation = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KeyFrameVector3 keyFrame;

				keyFrame.time = static_cast<float>(translation.mTime / animationAssimp->mTicksPerSecond);
				keyFrame.value = { -translation.mValue.x, translation.mValue.y, translation.mValue.z };
				nodeAnimation.translation.keyFrames.push_back(keyFrame);
			}
			// rotation
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex++) {
				aiQuatKey& rotate = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KeyFrameQuaternion keyFrame;

				keyFrame.time = static_cast<float>(rotate.mTime / animationAssimp->mTicksPerSecond);
				keyFrame.value = { rotate.mValue.x, -rotate.mValue.y, -rotate.mValue.z,  rotate.mValue.w };
				nodeAnimation.rotate.keyFrames.push_back(keyFrame);
			}
			// scale
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex++) {
				aiVectorKey& scale = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KeyFrameVector3 keyFrame;

				keyFrame.time = static_cast<float>(scale.mTime / animationAssimp->mTicksPerSecond);
				keyFrame.value = { scale.mValue.x, scale.mValue.y, scale.mValue.z };
				nodeAnimation.sacle.keyFrames.push_back(keyFrame);
			}
		}
	}

	return result;
}

const aiScene* MeshLoader::ReadFile(Assimp::Importer& importer, const std::string& fileName)
{
	std::filesystem::path path = fileName;

	// ファイル見つかんない
	if (not std::filesystem::exists(path)) {
		throw Lamb::Error::Code<MeshLoader>("this file does not find -> " + fileName, __func__);
	}
	// objかgltfではない
	if (not (path.extension() == ".obj" or path.extension() == ".gltf")) {
		throw Lamb::Error::Code<MeshLoader>("this file does not support -> " + fileName, __func__);
	}

	return importer.ReadFile(fileName.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
}

Node MeshLoader::ReadNode(aiNode* node)
{
	Node result;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();
	
	for (uint32_t y = 0; y < 4; y++) {
		for (uint32_t x = 0; x < 4; x++) {
			result.loacalMatrix[y][x] = aiLocalMatrix[y][x];
		}
	}

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);

	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

void MeshLoader::LoadMtl(const aiScene* scene, const std::string& directorypath, std::vector<Texture*>& result)
{
	std::vector<std::string> textureFileNames;

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		Lamb::SafePtr<aiMaterial> material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			std::filesystem::path tmpTextureFilePath = textureFilePath.C_Str();
			textureFileNames.push_back(directorypath + "/" + tmpTextureFilePath.filename().string());
		}
	}

	TextureManager* const textureMaanger = TextureManager::GetInstance();

	for (const auto& i : textureFileNames) {
		result.push_back(textureMaanger->LoadTexture(i));
	}
}

