#include "Coin.h"
#include "Drawers/DrawerManager.h"
#include "Camera/Camera.h"

void Coin::Init(const Transform& transform)
{
	transform_ = transform;
	transform_.rotate.x += 90.0f;
	obb_ = MakeObb();
	obb_->transform = transform_;

 	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("./Resources/Common/Coin/Coin.gltf");
	model_ = drawerManager->GetModel("./Resources/Common/Coin/Coin.gltf");

	animator_ = std::make_unique<Animator>();
	animator_->Load("./Resources/Common/Coin/Coin.gltf");
	animator_->SetLoopAnimation(true);
	animator_->Start();
}

void Coin::Update()
{
	transform_.Debug("coin");
	animator_->Update(model_->GetNode().name);


	obb_->transform = transform_;
	obb_->Update();
}

void Coin::Draw(const Camera& camera)
{
	if (not isCollision_) {
		model_->Draw(
			animator_->GetLocalMat4x4() * transform_.GetMatrix(),
			camera.GetViewProjection(),
			0xffffffff,
			BlendType::kNormal,
			true
		);
	}

	obb_->Draw(camera.GetViewProjection());
}
