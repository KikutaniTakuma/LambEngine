#pragma once

#include <memory>
#include <vector>

#include "Whirlpool/Whirlpool.h"
#include "Math/Matrix.h"
#include "Math/Vector3.h"

class Camera;
class Player;
class WhirlpoolManager {
public:
	WhirlpoolManager();
	void Initialize();
	void ResourceUpdate();
	void Update();
	void Draw(const Camera& camera);
	void Create(const Vector3& scale, const Vector3& rotate, const Vector3& pos, float gameOverSpeed);

	void SetPlayer(Player* player) { player_ = player; }
	const std::vector<std::unique_ptr<Whirlpool>>& GetWhirlpools() const { return whirlpools_; }
	void DeleteWhirlpool(size_t index);

	void LoadJson();
private:
	Player* player_;
	std::vector<std::unique_ptr<Whirlpool>> whirlpools_;
};