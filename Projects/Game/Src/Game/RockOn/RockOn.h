#pragma once
#include "Drawers/Texture2D/Texture2D.h"
#include <utility>

class RockOn {
public:
	RockOn() = default;
	RockOn(const RockOn&) = delete;
	RockOn(RockOn&&) = delete;
	~RockOn() = default;

	RockOn& operator=(const RockOn&) = delete;
	RockOn& operator=(RockOn&&) = delete;

public:
	void Initialize(class Camera* camera);

	void Update();

	void Draw();

	/// <summary>
	/// 毎フレーム呼んで引数のポジションにロックオンする
	/// </summary>
	/// <param name="pos"></param>
	void SetRockOnTarget(const Vector3& pos);

	bool IsRockOnRange(const Vector3& pos) const;

public:
	bool isRockOn_;
private:
	std::unique_ptr<Texture2D> rockOnMark_;


	Vector3 targetPos_;

	class Camera* camera_;

	std::unique_ptr<class Camera> staticCamera_;

	std::pair<float, float> rockOnRange_;
	std::pair<float, float> rockOnRotateRange_;
};