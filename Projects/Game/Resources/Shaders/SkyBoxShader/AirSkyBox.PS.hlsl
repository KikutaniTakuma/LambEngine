#include "SkyBox.hlsli"


struct AtmosphericParams {
	float32_t3 cameraPosition;   // カメラの位置
	float32_t atmosphereHeight;  // 大気の高さ
	float32_t3 lightDirection;   // 太陽光の方向（正規化ベクトル）
	float32_t humidity;          // 大気の湿度
	float32_t3 rayleighScattering; // Rayleigh散乱係数
	float32_t mieScattering;     // Mie散乱係数
	float32_t mieG;              // Mie散乱位相関数のg値（0から1の範囲）
};

ConstantBuffer<AtmosphericParams> gAtmosphericParams : register(b1);

float32_t3 ComputeRayleighScattering(
	float32_t3 lightDir, 
	float32_t3 viewDir, 
	float32_t height, 
	float32_t3 rayleighScattering
) {
    float32_t3 rayleigh = rayleighScattering * (1.0f + dot(viewDir, lightDir) * dot(viewDir, lightDir));
    float32_t hr = exp(-height * rcp(gAtmosphericParams.atmosphereHeight));
    return rayleigh * hr;
}

float32_t3 ComputeMieScattering(
	float32_t3 lightDir, 
	float32_t3 viewDir, 
	float32_t height, 
	float32_t mieScattering, 
	float32_t mieG
) {
    float32_t g2 = mieG * mieG;
    float32_t phase = (1.0f - g2) * rcp(pow(1.0f + g2 - 2.0f * mieG * dot(viewDir, lightDir), 1.5f));
    float32_t hm = exp(-height * rcp(gAtmosphericParams.atmosphereHeight));
    return mieScattering * phase * hm;
}

PixelOutPut main(VertexOutput input)
{

	float32_t3 worldPos = input.worldPosition.xyz;
    float32_t3 viewDirection = normalize(worldPos - gAtmosphericParams.cameraPosition);
    
    // カメラの高さを計算
    float32_t height = gAtmosphericParams.cameraPosition.y;

    // 大気散乱の計算
    float32_t3 rayleigh = ComputeRayleighScattering(
			gAtmosphericParams.lightDirection, 
			viewDirection, 
			height, 
			gAtmosphericParams.rayleighScattering
		);
    float32_t3 mie = ComputeMieScattering(
			gAtmosphericParams.lightDirection, 
			viewDirection, height, 
			gAtmosphericParams.mieScattering, 
			gAtmosphericParams.mieG
		);

	float32_t3 airColor = lerp(rayleigh * 1e4, mie * 1e4, gAtmosphericParams.humidity);

	PixelOutPut output;
	output.color.rgb = airColor * gMaterialData.color.xyz;

	output.color.a = 1.0f;

	return output;
}