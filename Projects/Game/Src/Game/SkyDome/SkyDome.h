#pragma once
#include <memory>
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include <unordered_map>

class SkyDome final {
private:
	struct IncidentLight {
		// 光の色
		Vector4 color;

		// 方向
		Vector3 direction;

		float pad;

		// ポジション
		Vector3 pos;

		// 光の強さ
		float intensity;
	};

	struct AirStatus {
		// 屈折率
		float refractiveIndex;

		// 単位体積当たりの分子数
		float moleculesNum;

		float wavelengthR;
		float wavelengthG;
		float wavelengthB;

	};

public:
	struct RayleighScatteringStatus {
		IncidentLight light;
		AirStatus air;
		Vector3 viewDirection;
		Vector3 cameraPos;
	};

public:
	SkyDome() = default;
	SkyDome(const SkyDome&) = delete;
	SkyDome(SkyDome&&) = delete;
	~SkyDome() = default;

	SkyDome& operator=(const SkyDome&) = delete;
	SkyDome& operator=(SkyDome&&) = delete;

public:
	void Initialize();

	void Finalize();

	void Debug(const std::string& guiName);

	void Upadate();

	void Draw(const class Camera& camera);

	const Vector3& GetSunPos() const;

public:
	void SetTexture(Texture* const tex);

private:
	Pipeline* CreatePipeline();

	Shader LoadShader();

public:
	Vector3 pos;
	Vector3 rotate;
	Vector3 scale;

private:
	Pipeline* pipeline_ = nullptr;

	Mesh* mesh_ = nullptr;

	Texture* tex_;

	std::unordered_map<std::string, Mesh::CopyData> data_;

	ConstBuffer<Mesh::MatrixData> wvpData_;

	ConstBuffer<RayleighScatteringStatus> rayleighScattering_;

	Vector3 sunPos;
	float sunRotate;
};