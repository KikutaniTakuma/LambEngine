#include "../Normal.hlsli"
#include "../PerlinNoise.hlsli"
#include "WaterTex2D.hlsli"

#define M_PI 3.141592653589793238462643
#define M_E 2.71828182846



static float32_t radBasis = 0.5f;

float32_t Waves(float32_t3 worldPos, uint32_t instanceID){
	float32_t3 ripplesPoint = kWaterData[instanceID].waveData.ripplesPoint;
	float32_t3 preRipplesPoint = kWaterData[instanceID].waveData.preRipplesPoint;
	float32_t ripplePointSpeed = kWaterData[instanceID].waveData.ripplePointSpeed;

	float32_t len = length(worldPos - ripplesPoint);
	float32_t preLen = length(worldPos - preRipplesPoint);
	
	// 以前の距離のほうが長かったら近づいてる(-)
	// 以前の距離のほうが近かったら遠ざかってる(+)
	float32_t singned = sign(len - preLen);


	float32_t maxHeight = kWaterData[instanceID].waveData.waveStrength;
	float32_t waveSpeed = kWaterData[instanceID].waveData.waveSpeed;
	float32_t time = kWaterData[instanceID].waveData.time;
	float32_t lengthAttenuation = kWaterData[instanceID].waveData.lengthAttenuation;

	float32_t ripples = kWaterData[instanceID].waveData.ripples;
	ripples = waveSpeed * rcp(waveSpeed + (ripplePointSpeed * singned)) * ripples;
	float32_t waveLength = 2.0f * M_PI * rcp(ripples);

	return maxHeight * pow(M_E, -len * lengthAttenuation) * sin(waveLength * (len - waveSpeed * time)) * min(1.0f, max(0.0f, (waveSpeed * time- len)));
}

[maxvertexcount(3)]
void main(
	triangle DomainShaderOutPutToGeometory input[3], 
	inout TriangleStream<GeometoryOutPut> outStream
){
	GeometoryOutPut output[3];
	uint32_t instanceID = input[0].instanceID;

	// お水の処理
	const float32_t2 kRandomVec = kWaterData[instanceID].randomVec;
    const float32_t kDensity = kWaterData[instanceID].density;

	for(uint32_t i = 0; i < 3; ++i){
		DomainShaderOutPutToGeometory inputTmp = input[i];

		// ワールドポジション計算
    	output[i].position = inputTmp.position;
		output[i].worldPosition = mul(output[i].position, kWvpMat[instanceID].worldMat);

		
		// 波の高さ
		float32_t waveHeight = Waves(output[i].worldPosition.xyz, instanceID);


		float32_t wavePower = 10.0f;
		float32_t epsilon = 0.0001f;
		float32_t subUV = 1.0f * rcp(400.0f) * epsilon;
		float32_t height = CreateNoise(inputTmp.uv, kRandomVec, kDensity) * wavePower + waveHeight;
		float32_t up = CreateNoise(float32_t2(inputTmp.uv.x, inputTmp.uv.y + subUV), kRandomVec, kDensity) * wavePower + Waves(float32_t3(output[i].worldPosition.x, output[i].worldPosition.y, output[i].worldPosition.z + epsilon), instanceID);
		float32_t down = CreateNoise(float32_t2(inputTmp.uv.x, inputTmp.uv.y - subUV), kRandomVec, kDensity) * wavePower + Waves(float32_t3(output[i].worldPosition.x, output[i].worldPosition.y, output[i].worldPosition.z - epsilon), instanceID);
		float32_t right = CreateNoise(float32_t2(inputTmp.uv.x + subUV, inputTmp.uv.y), kRandomVec, kDensity) * wavePower + Waves(float32_t3(output[i].worldPosition.x + epsilon, output[i].worldPosition.y, output[i].worldPosition.z), instanceID);
		float32_t left = CreateNoise(float32_t2(inputTmp.uv.x - subUV, inputTmp.uv.y), kRandomVec, kDensity) * wavePower + Waves(float32_t3(output[i].worldPosition.x - epsilon, output[i].worldPosition.y, output[i].worldPosition.z), instanceID);

		float32_t yx = (right - left) * rcp(2.0f * epsilon);
		float32_t yz = (up - down) * rcp(2.0f * epsilon);

		float32_t3 resultNormal = normalize(float32_t3(-yx, 1.0f, -yz));
		output[i].normal = resultNormal;

		output[i].worldPosition.y += height;

		
		output[i].position = mul(output[i].worldPosition, kWvpMat[instanceID].cameraMat);

		output[i].uv = inputTmp.uv;
		output[i].textureID = inputTmp.textureID;
		output[i].instanceID = inputTmp.instanceID;

		outStream.Append(output[i]);
	}

	outStream.RestartStrip();
}