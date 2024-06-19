struct MaterialData {
	float32_t4x4 wvpMat;
	float32_t4 color;
};

ConstantBuffer<MaterialData> gMaterialData : register(b0);
TextureCube<float32_t4> gCubeTex : register(t0);
SamplerState smp : register(s0);

struct VertexInput{
    float32_t4 position : POSITION;
};

struct VertexOutput{
    float32_t4 position : SV_POSITION;
    float32_t3 texcoord : TEXCOORD;
};

struct PixelOutPut{
    float32_t4 color : SV_TARGET0;
};