#include "Tonemap.h"

float32_t3 Tonemap(const TonemapParams& tc, float32_t3 x)
{
	float32_t3 toe = -tc.mToe.x / (x + tc.mToe.y) + tc.mToe.z;
	float32_t3 mid = tc.mMid.x * x + tc.mMid.y;
	float32_t3 shoulder = -tc.mShoulder.x / (x + tc.mShoulder.y) + tc.mShoulder.z;

	float32_t3 result = Vector3::Lerp(toe, mid, Vector3::Step(tc.mBx.x, x));
	result = Vector3::Lerp(result, shoulder, Vector3::Step(tc.mBx.y, x));
	return result;
}

TonemapParams PrepareTonemapParams(float32_t2 p1, float32_t2 p2, float32_t2 p3)
{
	TonemapParams tc;

	constexpr float kEpsilon = 1e-5f;

	float denom = p2.x - p1.x;
	denom = std::abs(denom) > kEpsilon ? denom : kEpsilon;
	float slope = (p2.y - p1.y) / denom;

	{
		tc.mMid.x = slope;
		tc.mMid.y = p1.y - slope * p1.x;
	}

	{
		denom = p1.y - slope * p1.x;
		denom = std::abs(denom) > kEpsilon ? denom : kEpsilon;
		tc.mToe.x = slope * p1.x * p1.x * p1.y * p1.y / (denom * denom);
		tc.mToe.y = slope * p1.x * p1.x / denom;
		tc.mToe.z = p1.y * p1.y / denom;
	}

	{
		denom = slope * (p2.x - p3.x) - p2.y + p3.y;
		denom = std::abs(denom) > kEpsilon ? denom : kEpsilon;
		tc.mShoulder.x = slope * std::pow(p2.x - p3.x, 2.0f) * std::pow(p2.y - p3.y, 2.0f) / (denom * denom);
		tc.mShoulder.y = (slope * p2.x * (p3.x - p2.x) + p3.x * (p2.y - p3.y)) / denom;
		tc.mShoulder.z = (-p2.y * p2.y + p3.y * (slope * (p2.x - p3.x) + p2.y)) / denom;
	}

	tc.mBx = float32_t2(p1.x, p2.x);
	tc.mBy = float32_t2(p1.y, p2.y);

	return tc;
}