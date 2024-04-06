#pragma once
#include "../GraphicsStructs.h"

class MeshLoader final {
public:
	struct IndexData {
		uint32_t vertNum;
		uint32_t uvNum;
		uint32_t normalNum;
		uint32_t textureHandle;

		inline bool operator==(const IndexData& right) const {
			return vertNum == right.vertNum
				&& uvNum == right.uvNum
				&& normalNum == right.normalNum
				&& textureHandle == right.textureHandle;
		}
		inline bool operator!=(const IndexData& right) const {
			return !(*this == right);
		}
	};

public:
	/// <summary>
	/// Objファイルのロード
	/// </summary>
	/// <param name="fileName">Objファイルパス</param>
	/// <returns>頂点とインデックス情報</returns>
	static Mesh LoadObj(const std::string& fileName);

private:
	static void LoadMtl(const struct aiScene* scene, const std::string& directorypath, std::vector<class Texture*>& result);
};