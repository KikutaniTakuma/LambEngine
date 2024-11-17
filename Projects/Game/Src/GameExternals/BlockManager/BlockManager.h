#pragma once

#include <memory>
#include <vector>

#include "Block/Block.h"
#include "Math/Matrix.h"
#include "Math/Vector3.h"

class Camera;
class Player;
class BlockManager {
public:
	BlockManager();
	void Initialize();
	void Update();
	void ResourceUpdate();
	void Draw(const Camera& camera);

	void Create(const Vector3& scale, const Vector3& rotate, const Vector3& position);

	const std::vector<std::unique_ptr<Block>>& GetBlocks() const { return blocks_; }
	void DeleteBlock(size_t index);
	void SetPlayer(Player* player) { player_ = player; }

	void LoadJson();
private:
	std::vector<std::unique_ptr<Block>> blocks_;
	Player* player_;
};