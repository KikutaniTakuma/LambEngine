#include "Utils/Random.h"

namespace Lamb {
	RandomGenereator* const generator = RandomGenereator::GetInstance();

	float Random(float min, float max) {
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_real_distribution<float> dist{ min, max };

		return static_cast<float>(dist(generator->GetGenerator()));
	}

	double Random(double min, double max) {
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_real_distribution<double> dist{ min, max };

		return static_cast<double>(dist(generator->GetGenerator()));
	}


	Vector2 Random(const Vector2& min, const Vector2& max) {
		Vector2 result;
		result.x = Random(min.x, max.x);
		result.y = Random(min.y, max.y);

		return result;
	}

	Vector3 Random(const Vector3& min, const Vector3& max) {
		Vector3 result;
		result.x = Random(min.x, max.x);
		result.y = Random(min.y, max.y);
		result.z = Random(min.z, max.z);

		return result;
	}

	uint32_t RandomColor(uint32_t min, uint32_t max) {
		return Vector4ToUint(RandomColor(Vector4{ min }, Vector4{ max }));
	}
	Vector4 RandomColor(const Vector4& min, const Vector4& max) {
		Vector4 result;
		result.vec.x = Random(min.vec.x, max.vec.x);
		result.vec.y = Random(min.vec.y, max.vec.y);
		result.vec.z = Random(min.vec.z, max.vec.z);
		result.vec.w = Random(min.vec.w, max.vec.w);

		return result;
	}
	RandomGenereator::RandomGenereator() :  
		generator_()
	{
		generator_ = std::make_unique<std::mt19937_64>(seed_);
	}
	RandomGenereator* const RandomGenereator::GetInstance()
	{
		static RandomGenereator instance;
		return &instance;
	}
	uint32_t RandomGenereator::GetSeed() const
	{
		return seed_;
	}
	std::mt19937_64& RandomGenereator::GetGenerator()
	{
		return *generator_;
	}
}