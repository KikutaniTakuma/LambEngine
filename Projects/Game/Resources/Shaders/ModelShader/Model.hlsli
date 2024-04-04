struct WVPMatrix {
	float32_t4x4 worldMat;
	float32_t4x4 cameraMat;
};

struct DirectionLight {
	float32_t3 ligDirection;
	float32_t3 ligColor;
};

struct Color {
	float32_t4 color;
};

struct IsLighting {
	uint32_t isLighting;
};

ConstantBuffer<DirectionLight> kLight : register(b0);
StructuredBuffer<WVPMatrix> kWvpMat : register(t0);
StructuredBuffer<Color> kColor : register(t1);
StructuredBuffer<IsLighting> kIsLighting : register(t2);
Texture2D<float4> textures[] : register(t3);
SamplerState smp : register(s0);

struct VertexShaderOutput{
    float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD;
	uint32_t textureID : BLENDINDICES0;
	uint32_t instanceID : BLENDINDICES1;
};