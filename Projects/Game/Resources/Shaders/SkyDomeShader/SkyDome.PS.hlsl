#include "SkyDome.hlsli"

struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

PixelShaderOutPut main(VertexShaderOutput input)
{
	PixelShaderOutPut output;
	input.normal = normalize(input.normal);
	input.normal *= -1.0f;
	
	output.color = tex.Sample(smp, input.uv);

	return output;
}