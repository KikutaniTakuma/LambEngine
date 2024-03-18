#pragma once
#include "../GraphicsStructs.h"

class MeshLoader final {
private:
	struct IndexData {
		uint32_t vertNum;
		uint32_t uvNum;
		uint32_t normalNum;
		uint32_t textureHandle;

		inline bool operator==(const IndexData& right) {
			return vertNum == right.vertNum
				&& uvNum == right.uvNum
				&& normalNum == right.normalNum
				&& textureHandle == right.textureHandle;
		}
		inline bool operator!=(const IndexData& right) {
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

	/// <summary>
	/// mtlファイルのロード
	/// </summary>
	/// <param name="fileName">mtlファイルパス</param>
	/// <returns>テクスチャハンドル</returns>
	static std::unordered_map<std::string, uint32_t> LoadMtl(const std::string& fileName);
};