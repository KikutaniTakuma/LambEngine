#include "SkyDome.hlsli"
#define M_PI 3.1415926535897932384626433832795028

struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float Molecules(float pascal, float kelvin){
	float N = pascal / (8.314 * kelvin) * 6.022f * pow(10.0f, 23.0f);

	return N;
}



float ScatteringCoefficient(float wavelength){
	float result;
	const float n = kRayleighScattering.air.refractiveIndex;
	const float N = Molecules(101325.0f, 298.0f);
	result = (8.0f * pow(M_PI,3.0f) * (pow(n * n - 1.0f, 2.0f)));
	result /= (3.0f * N * pow(wavelength, 4.0f) * pow(n * n + 2.0f, 2.0f));
	//result *= lightLength * 0.05f;

	return result;
}
/*
float3 CalculateMieScattering(float3 lightDir, float3 viewDir, float3 particleAlbedo, float atmosphereThickness) {
   

    float3 scatteredLight = ;
    return scatteredLight;
}*/

float3 CalculateLayryScattering(float R, float theta) {
	float3 light = kRayleighScattering.light.color.xyz * kRayleighScattering.light.intensity;
	float particleParam = 30e-9;
	float n = kRayleighScattering.air.refractiveIndex;
	float3 scatter = float3(kRayleighScattering.air.wavelengthR,kRayleighScattering.air.wavelengthG,kRayleighScattering.air.wavelengthB);

	float3 result;

	result = (light * pow(M_PI, 4.0f) /** pow(particleParam, 6.0f)*/) / (8.0f * pow(R, 2.0f) * pow(scatter, 4.0f));
	result *= (pow(n, 2.0f) - 1) / (pow(n, 2.0f) + 1);
	result *= 1+pow(theta, 2.0f);

	return result;
}

PixelShaderOutPut main(VertexShaderOutput input)
{
	//const float earthRadius = 6360.0f * 1000.0f;
	//const float atmosphericRadius = earthRadius + 100.0f * 1000.0f;



	PixelShaderOutPut output;
	input.normal *= -1.0f;
	
	output.color = tex.Sample(smp, input.uv);

	float3 lightDir = normalize(kRayleighScattering.light.pos - input.worldPosition.xyz);
	float3 viewDir = normalize(kRayleighScattering.cameraPos - input.worldPosition.xyz);
	float theta = dot(lightDir, viewDir);

	float lightLength = length(input.worldPosition.xyz - kRayleighScattering.cameraPos);
	output.color.xyz = CalculateLayryScattering(lightLength, theta);

/*
	float lightLength = length(input.worldPosition.xyz - kRayleighScattering.cameraPos);

	float3 rayleighCoefficient;
	rayleighCoefficient.r = ScatteringCoefficient(kRayleighScattering.air.wavelengthR);
	rayleighCoefficient.g = ScatteringCoefficient(kRayleighScattering.air.wavelengthG);
	rayleighCoefficient.b = ScatteringCoefficient(kRayleighScattering.air.wavelengthB);

	float theta = dot(kRayleighScattering.light.direction, kRayleighScattering.viewDirection);

	float3 rayleighScatter = rayleighCoefficient * ((1.0f + pow(theta, 2.0f)) / pow(lightLength, 2.0f));
	rayleighScatter.r /= pow(kRayleighScattering.air.wavelengthR, 4.0f);
	rayleighScatter.g /= pow(kRayleighScattering.air.wavelengthG, 4.0f);
	rayleighScatter.b /= pow(kRayleighScattering.air.wavelengthB, 4.0f);

	float3 scatterColor = kRayleighScattering.light.color.xyz * kRayleighScattering.light.intensity * rayleighScatter;

	output.color.xyz = scatterColor;
	*/
	
/*
	float3 light = kRayleighScattering.light.color.xyz * kRayleighScattering.light.intensity;


	float3 lightDir = normalize(kRayleighScattering.light.pos - input.worldPosition.xyz);
	float3 viewDir = normalize(kRayleighScattering.cameraPos - input.worldPosition.xyz);
	float theta = dot(lightDir, viewDir);
	float incidentLightAngle = (1.0f + pow((theta), 2.0f));

	float3 scattering = float3(kRayleighScattering.air.wavelengthR,kRayleighScattering.air.wavelengthG,kRayleighScattering.air.wavelengthB);
	float3 scatteringProc = pow((2.0f * M_PI) / scattering, 4.0f);

	float lightLength = length(input.worldPosition.xyz - kRayleighScattering.cameraPos);
	float particleParam = 30e-9;
	float particleProcParam = pow(particleParam, 6.0f) / pow(lightLength, 2.0f);


	const float n = kRayleighScattering.air.refractiveIndex;
	float refractiveIndex = pow(((n * n) - 1.0f) / ((n * n) + 2.0f), 2.0f);

	output.color.xyz = light * incidentLightAngle * scatteringProc * particleProcParam * refractiveIndex;
*/

	return output;
}