#include "../OutputStructs.hlsli"

Texture2D<float32_t4> textures[] : register(t4);
SamplerState smp : register(s0);

struct MSOutput {
	float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD0;
	uint32_t textureID : BLENDINDICES0;
};

PixelShaderOutPut3 main(MSOutput input)
{
	float32_t4 textureColor = textures[input.textureID < 128 ? input.textureID : 0].Sample(smp, input.uv);

	PixelShaderOutPut3 output;
	output.color0 = textureColor;

	output.color1.xyz = input.normal;
	output.color1.w = 1.0f;

	output.color2.xyz = input.worldPosition.xyz;
	output.color2.w = 1.0f;

	return output;
}