#pragma once

#include <array>
#include <memory>

#include "Bullet/Bullet.h"
#include "Math/Vector3.h"

class Camera;
class BulletManager {
private:
	static const uint32_t kMaxBullet = 10;
public:
	static BulletManager* GetInstance();

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void Finalize();

	void Clear();

	void Create(const Vector3& position, const Vector3& vector);

	const std::array<std::unique_ptr<Bullet>, kMaxBullet>& GetBullets() const { return bullets_; }
private:
	std::array<std::unique_ptr<Bullet>, kMaxBullet> bullets_;
};