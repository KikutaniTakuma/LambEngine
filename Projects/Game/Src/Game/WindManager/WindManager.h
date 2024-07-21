#pragma once


#include <memory>
#include <list>

#include "Wind/Wind.h"
#include "Math/Mat4x4.h"
#include "Math/Vector3.h"
#include "../WindParticle/WindParticle.h"

class Camera;
class Player;
class WindManager {
public:
	WindManager();
	void Initialize();
	void ResourceUpdate();
	void Update();
	void Draw(const Camera& camera);
	void Create(const Vector3& scale, const Vector3& rotate, const Vector3& pos, const Vector3& vector);

	void SetPlayer(Player* player) { player_ = player; }
	const std::list<std::unique_ptr<Wind>>& GetWinds() const { return winds_; }
	void DeleteWind(size_t index);

	void LoadJson();
private:
	Player* player_;
	std::list<std::unique_ptr<Wind>> winds_;
	std::list<std::unique_ptr<WindParticle>> particles_;
};