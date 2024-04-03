#include "MeshLoader.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <unordered_map>

#include "../TextureManager/TextureManager.h"
#include "../../Core/DirectXDevice/DirectXDevice.h"

Mesh MeshLoader::LoadObj(const std::string& fileName)
{
	std::ifstream file{ fileName };
	if (file.fail()) {
		if (!std::filesystem::exists(fileName)) {
			throw Lamb::Error::Code<Mesh>("this file is not exist -> " + fileName, __func__);
		}
		else {
			throw Lamb::Error::Code<Mesh>("something error -> " + fileName, __func__);
		}
	}

	std::vector<Vector4> posDatas(0);

	std::vector<Vector3> normalDatas(0);

	std::vector<Vector2> uvDatas(0);

	std::unordered_map<std::string, std::vector<IndexData>> indexDatas(0);
	std::unordered_map<std::string, std::vector<IndexData>>::iterator indicesItr;

	std::unordered_map<std::string, uint32_t> texHandles;

	std::string lineBuf;

	while (std::getline(file, lineBuf)) {
		std::string identifier;
		std::istringstream line(lineBuf);
		line >> identifier;
		if (identifier == "v") {
			Vector4 buf;
			line >> buf.vec.x >> buf.vec.y >> buf.vec.z;
			buf.vec.x *= -1.0f;
			buf.vec.w = 1.0f;

			posDatas.push_back(buf);
		}
		else if (identifier == "vn") {
			Vector3 buf;
			line >> buf.x >> buf.y >> buf.z;
			buf.x *= -1.0f;
			normalDatas.push_back(buf);
		}
		else if (identifier == "vt") {
			Vector2 buf;
			line >> buf.x >> buf.y;
			buf.y = 1.0f - buf.y;
			uvDatas.push_back(buf);
		}
		else if (identifier == "f") {
			std::string buf;
			std::vector<float> posBuf(0);
			std::array<IndexData, 3> indcoes;
			auto idnexItr = indcoes.rbegin();
			while (std::getline(line, buf, ' '))
			{
				/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
				std::string num[3];
				int32_t count = 0;
				if (std::any_of(buf.cbegin(), buf.cend(), isdigit)) {
					for (auto& ch : buf) {
						if (ch == '/') {
							count++;
						}
						else { num[count] += ch; }
					}
				}

				// エラーチェック
				if (idnexItr == indcoes.rend()) {
					throw Lamb::Error::Code<Mesh>("Not supported for rectangles or more", __func__);
				}

				if (count == 2) {
					idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
					idnexItr->uvNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
					idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[2]) - 1);
					idnexItr++;
				}
				else if (count == 1) {
					idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
					idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
					idnexItr++;
				}
			}
			for (auto& i : indcoes) {
				indicesItr->second.push_back(i);
			}
		}
		else if (identifier == "usemtl") {
			std::string useMtlName;
			line >> useMtlName;
			indexDatas.insert({ useMtlName,std::vector<IndexData>(0) });
			indicesItr = indexDatas.find(useMtlName);
		}
		else if (identifier == "mtllib") {
			std::string mtlFileName;
			std::filesystem::path path = fileName;

			line >> mtlFileName;

			texHandles = LoadMtl(path.parent_path().string() + "/" + mtlFileName);
		}
	}
	file.close();



	std::vector<std::pair<IndexData, uint16_t>> indexTmp;
	uint16_t count = 0;

	for (auto& i : indexDatas) {
		for (auto& index : i.second) {
			index.textureHandle = texHandles[i.first];

			auto isExist = std::find_if(indexTmp.begin(), indexTmp.end(), 
				[&index](const std::pair<IndexData, uint16_t>& data)->bool {
					return index == data.first;
				}
				);

			if (isExist == indexTmp.end()) {
				indexTmp.push_back(std::make_pair(index, count));
				count++;
			}
		}
	}

	std::vector<Vertex> vertData;
	vertData.resize(count);

	for (auto& i : indexTmp) {
		vertData[i.second].pos = posDatas[i.first.vertNum];
		vertData[i.second].normal = normalDatas[i.first.normalNum];
		vertData[i.second].uv = uvDatas[i.first.uvNum];
		vertData[i.second].texIndex = i.first.textureHandle;
	}

	std::vector<uint16_t> indeces;
	size_t indecesCount = 0;

	for (auto& i : indexDatas) {
		indecesCount +=i.second.size();
	}

	indeces.reserve(indecesCount);


	for (auto& i : indexDatas) {
		for (auto& j : i.second) {
			auto indexTmpItr = std::find_if(indexTmp.begin(), indexTmp.end(),
				[&j](const std::pair<IndexData, uint16_t>& data)->bool {
					return j == data.first;
				}
			);

			if (indexTmpItr == indexTmp.end()) {
				throw Lamb::Error::Code<MeshLoader>("cannot find index", __func__);
			}

			indeces.push_back(indexTmpItr->second);
		}
	}

	Mesh result;
	uint32_t indexSizeInBytes = static_cast<uint32_t>(sizeof(uint16_t) * indeces.size());
	uint32_t vertexSizeInBytes = static_cast<uint32_t>(sizeof(Vertex) * vertData.size());

	result.indexResource = DirectXDevice::GetInstance()->CreateBufferResuorce(indexSizeInBytes);

	uint16_t* indexMap = nullptr;
	result.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (size_t i = 0; auto& index : indeces) {
		indexMap[i] = index;
		i++;
	}
	result.indexResource->Unmap(0, nullptr);

	result.indexNumber = static_cast<uint32_t>(indeces.size());
	result.indexView.SizeInBytes = indexSizeInBytes;
	result.indexView.Format = DXGI_FORMAT_R16_UINT;
	result.indexView.BufferLocation = result.indexResource->GetGPUVirtualAddress();


	result.vertexResource = DirectXDevice::GetInstance()->CreateBufferResuorce(vertexSizeInBytes);
	
	Vertex* vertMap = nullptr;
	result.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	for (size_t i = 0; auto& vertex : vertData) {
		vertMap[i] = vertex;
		i++;
	}
	result.vertexResource->Unmap(0, nullptr);

	result.vertexNumber = static_cast<uint32_t>(vertData.size());
	result.vertexView.SizeInBytes = vertexSizeInBytes;
	result.vertexView.StrideInBytes = static_cast<uint32_t>(sizeof(Vertex));
	result.vertexView.BufferLocation = result.vertexResource->GetGPUVirtualAddress();

	return result;
}

std::unordered_map<std::string, uint32_t> MeshLoader::LoadMtl(const std::string& fileName)
{

	std::ifstream file{ fileName };
	if (file.fail()) {
		if (!std::filesystem::exists(fileName)) {
			throw Lamb::Error::Code<Mesh>("this file is not exist -> " + fileName, __func__);
		}
		else {
			throw Lamb::Error::Code<Mesh>("something error -> " + fileName, __func__);
		}
	}

	TextureManager* const textureManager = TextureManager::GetInstance();

	std::unordered_map<std::string, uint32_t> result;

	std::string lineBuf;
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
	}

	return result;
}
