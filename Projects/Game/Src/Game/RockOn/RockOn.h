#pragma once
#include "Drawers/Texture2D/Texture2D.h"
#include <utility>
#include <initializer_list>
#include <vector>

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

	Vector3 SortRockOn(std::initializer_list<Vector3> positions);
	void SetRockOnPositions(const std::vector<Vector3>& positions);

	Vector3 NowRockOnPos() const;

public:
	bool isRockOn_;
private:
	std::unique_ptr<Texture2D> rockOnMark_;

	Vector3 targetPos_;

	class Camera* camera_;

	std::unique_ptr<class Camera> staticCamera_;

	std::pair<float, float> rockOnRange_;
	std::pair<float, float> rockOnRotateRange_;

	bool isAutoRock_;

	int32_t currentRockOn_;

	std::vector<Vector3> positions_;
};