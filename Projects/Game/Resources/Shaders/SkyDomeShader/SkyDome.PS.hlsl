#include "SkyDome.hlsli"
#define M_PI 3.1415926535897932384626433832795028

struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float ScatteringCoefficient(float wavelength){
	float result;
	const float n = kRayleighScattering.air.refractiveIndex;
	const float N = kRayleighScattering.air.moleculesNum;
	result = (8.0 * pow(M_PI,3.0) * (pow(n * n - 1.0, 2.0)));
	result /= (3.0 * N * pow(wavelength, 4.0) * pow(n * n + 2.0, 2.0));

	return result;
}

PixelShaderOutPut main(VertexShaderOutput input)
{
	PixelShaderOutPut output;
	input.normal *= -1.0f;
	
	output.color = tex.Sample(smp, input.uv);

	float3 rayleighCoefficient;
	rayleighCoefficient.r = ScatteringCoefficient(kRayleighScattering.air.wavelengthR);
	rayleighCoefficient.g = ScatteringCoefficient(kRayleighScattering.air.wavelengthG);
	rayleighCoefficient.b = ScatteringCoefficient(kRayleighScattering.air.wavelengthB);

	float theta = dot(kRayleighScattering.light.direction, kRayleighScattering.viewDirection);

	float3 rayleighScatter = rayleighCoefficient * (1.0f + pow(theta, 2.0f));

	float3 scatterColor = kRayleighScattering.light.color.xyz * kRayleighScattering.light.intensity * rayleighScatter;

	output.color.xyz *= scatterColor;

	return output;
}