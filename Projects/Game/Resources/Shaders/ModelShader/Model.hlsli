struct WVPMatrix {
	float32_t4x4 worldMat;
	float32_t4x4 cameraMat;
};

struct DirectionLight {
	float32_t3 ligDirection;
	float32_t3 ligColor;
	float32_t3 eyePos;

	float32_t3 ptPos;
	float32_t3 ptColor;
	float32_t ptRange;
};

struct Color {
	float32_t4 color;
};

struct ShaderStruct {
	uint32_t num;
};

ConstantBuffer<DirectionLight> kLight : register(b0);
StructuredBuffer<WVPMatrix> kWvpMat : register(t0);
StructuredBuffer<Color> kColor : register(t1);
StructuredBuffer<ShaderStruct> kNumbers : register(t2);
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