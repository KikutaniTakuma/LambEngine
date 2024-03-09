#include "SkyDome.hlsli"
#define M_PI 3.1415926535897932384626433832795028

struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float CalculateOpticalPathLength(float startHeight, float endHeight, float stepSize) {
    const float scaleHeight = 8500.0;
    
    float opticalPathLength = 0.0;
    float currentHeight = startHeight;

    while (currentHeight < endHeight) {
        float density = exp(-currentHeight / scaleHeight);
        opticalPathLength += density * stepSize;
        currentHeight += stepSize;
    }

    return opticalPathLength;
}

float ScatteringCoefficient(float wavelength, float lightLength){
	float result;
	const float n = kRayleighScattering.air.refractiveIndex;
	const float N = kRayleighScattering.air.moleculesNum;
	result = (8.0 * pow(M_PI,3.0) * (pow(n * n - 1.0, 2.0)));
	result /= (3.0 * N * pow(wavelength, 4.0) * pow(n * n + 2.0, 2.0));
	result *= lightLength * 0.05;

	return result;
}
/*
float3 CalculateMieScattering(float3 lightDir, float3 viewDir, float3 particleAlbedo, float atmosphereThickness) {
   

    float3 scatteredLight = ;
    return scatteredLight;
}*/

PixelShaderOutPut main(VertexShaderOutput input)
{
	const float earthRadius = 6360.0f * 1000.0f;
	const float atmosphericRadius = earthRadius + 100.0f * 1000.0f;



	PixelShaderOutPut output;
	input.normal *= -1.0f;
	
	output.color = tex.Sample(smp, input.uv);

	float3 atmosphericPos = normalize(input.worldPosition.xyz);
	atmosphericPos *= atmosphericRadius;

	float3 earthPos = normalize(kRayleighScattering.cameraPos);
	earthPos *= earthRadius;

	float lightLength = length(atmosphericPos - earthPos);


	float3 rayleighCoefficient;
	rayleighCoefficient.r = ScatteringCoefficient(kRayleighScattering.air.wavelengthR,lightLength);
	rayleighCoefficient.g = ScatteringCoefficient(kRayleighScattering.air.wavelengthG,lightLength);
	rayleighCoefficient.b = ScatteringCoefficient(kRayleighScattering.air.wavelengthB,lightLength);

	float theta = dot(kRayleighScattering.light.direction, kRayleighScattering.viewDirection);

	float3 rayleighScatter = rayleighCoefficient * (1.0f + pow(theta, 2.0f));

	float3 scatterColor = kRayleighScattering.light.color.xyz * kRayleighScattering.light.intensity * rayleighScatter;

	output.color.xyz *= scatterColor;

	return output;
}