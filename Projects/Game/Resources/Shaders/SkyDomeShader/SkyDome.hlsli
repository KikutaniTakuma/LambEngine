cbuffer WorldViewProjection : register(b0){
	float32_t4x4 worldMat;
	float32_t4x4 viewProjectionMat;
}

struct IncidentLight {
	// 光の色
	float4 color;

	// 方向
	float3 direction;

	// ポジション
	float3 pos;

	// 光の強さ
	float intensity;
};

struct AirStatus {
	// 屈折率
	float refractiveIndex;

	// 単位体積当たりの分子数(余りにも値が大きいのでスケールフィルターと分ける)
	float moleculesNum;

	// スケールフィルター
	float scaleFilter;
};

struct RayleighScatteringStatus {
	IncidentLight light;
	AirStatus air;
};

ConstantBuffer<RayleighScatteringStatus> rayleighScatteringStatus : register(b1);

struct VertexShaderOutput{
    float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD;
};