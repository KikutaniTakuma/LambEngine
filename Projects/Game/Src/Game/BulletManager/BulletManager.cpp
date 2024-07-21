#include "BulletManager.h"

#include "Utils/Camera/Camera.h"

BulletManager* BulletManager::GetInstance() {
	static BulletManager instance;
	return &instance;
}

void BulletManager::Initialize() {
	for (auto& bullet : bullets_) {
		bullet = std::make_unique<Bullet>();
		bullet->Initialize();
		bullet->SetIsAlive(false);
	}
}

void BulletManager::Update() {
	for (auto& bullet : bullets_) {
		if (bullet->GetIsAlive()) {
			bullet->Update();
		}
		bullet->ModelUpdate();
	}
}

void BulletManager::Draw(const Camera& camera) {
	for (auto& bullet : bullets_) {
		if (bullet->GetIsAlive()) {
			bullet->Draw(camera);
		}
	}
}

void BulletManager::Finalize() {
	for (auto& bullet : bullets_) {
		bullet.reset();
	}
}

void BulletManager::Clear() {
	for (auto& bullet : bullets_) {
		bullet->SetIsAlive(false);
	}
}

void BulletManager::Create(const Vector3& position, const Vector3& vector) {
	for (auto& bullet : bullets_) {
		if (!bullet->GetIsAlive()) {
			bullet->SetBullet(position, vector);
			break;
		}
	}
}
