#include "../Normal.hlsli"
#include "../PerlinNoise.hlsli"
#include "WaterTex2D.hlsli"

#define M_PI 3.141592653589793238462643
#define M_E 2.71828182846



static float32_t radBasis = 0.5f;

float32_t Waves(float32_t length, uint32_t instanceID){
	float32_t maxHeight = kWaterData[instanceID].waveData.waveStrength;
	float32_t waveLength = 2.0f * M_PI * rcp(kWaterData[instanceID].waveData.ripples);
	float32_t waveSpeed = kWaterData[instanceID].waveData.waveSpeed;
	float32_t time = kWaterData[instanceID].waveData.time;
	float32_t timeAttenuation = kWaterData[instanceID].waveData.timeAttenuation;

	return maxHeight * pow(M_E, -time * timeAttenuation) * sin(waveLength * (length - waveSpeed * time)) * min(1.0f, max(0.0f, (waveSpeed * time- length)));
}

float32_t3 CalcNormal(float32_t3 position, float32_t delta, uint32_t instanceID){
	float32_t3 ripplesPoint = kWaterData[instanceID].waveData.ripplesPoint;
	float32_t3 upPos = position;
	upPos.z += delta;
	float32_t3 bottomPos = position;
	bottomPos.z -= delta;
	float32_t3 rightPos = position;
	rightPos.x += delta;
	float32_t3 leftPos = position;
	leftPos.x -= delta;

	float32_t upLength = length(upPos - ripplesPoint);
	float32_t bottomLength = length(bottomPos - ripplesPoint);
	float32_t rightLength = length(rightPos - ripplesPoint);
	float32_t leftLength = length(leftPos - ripplesPoint);

	float32_t up = Waves(upLength, instanceID);
	float32_t bottom = Waves(bottomLength, instanceID);
	float32_t right = Waves(rightLength, instanceID);
	float32_t left = Waves(leftLength, instanceID);

	float32_t dx = (right - left) * rcp(2.0f * delta);
	float32_t dy = (up - bottom) * rcp(2.0f * delta);

	float32_t3 normal = normalize(float32_t3(-dx, -dy, 1.0));

	return normal;
}

[maxvertexcount(3)]
void main(
	triangle DomainShaderOutPutToGeometory input[3], 
	inout TriangleStream<GeometoryOutPut> outStream
){
	GeometoryOutPut output[3];
	uint32_t instanceID = input[0].instanceID;

	float32_t3 ripplesPoint = kWaterData[instanceID].waveData.ripplesPoint;

	// お水の処理
	const float32_t2 kRandomVec = kWaterData[instanceID].randomVec;
    const float32_t kDensity = kWaterData[instanceID].density;

	for(uint32_t i = 0; i < 3; ++i){
		DomainShaderOutPutToGeometory inputTmp = input[i];

		// ワールドポジション計算
    	output[i].position = inputTmp.position;
		output[i].worldPosition = mul(output[i].position, kWvpMat[instanceID].worldMat);

		
		// 波の高さ
		float32_t ripplesPointToPos = length(output[i].worldPosition.xyz - ripplesPoint);
		float32_t waveHeight = Waves(ripplesPointToPos, instanceID);


		float32_t wavePower = 10.0f;
		float32_t epsilon = 0.0001f;
		float32_t subUV = 1.0f * rcp(400.0f) * epsilon;
		float32_t height = CreateNoise(inputTmp.uv, kRandomVec, kDensity) * wavePower + waveHeight;
		float32_t up = CreateNoise(float32_t2(inputTmp.uv.x, inputTmp.uv.y + subUV), kRandomVec, kDensity) * wavePower + Waves(length(float32_t3(output[i].worldPosition.x, output[i].worldPosition.y, output[i].worldPosition.z + epsilon) - ripplesPoint), instanceID);
		float32_t down = CreateNoise(float32_t2(inputTmp.uv.x, inputTmp.uv.y - subUV), kRandomVec, kDensity) * wavePower + Waves(length(float32_t3(output[i].worldPosition.x, output[i].worldPosition.y, output[i].worldPosition.z - epsilon) - ripplesPoint), instanceID);
		float32_t right = CreateNoise(float32_t2(inputTmp.uv.x + subUV, inputTmp.uv.y), kRandomVec, kDensity) * wavePower + Waves(length(float32_t3(output[i].worldPosition.x + epsilon, output[i].worldPosition.y, output[i].worldPosition.z) - ripplesPoint), instanceID);
		float32_t left = CreateNoise(float32_t2(inputTmp.uv.x - subUV, inputTmp.uv.y), kRandomVec, kDensity) * wavePower + Waves(length(float32_t3(output[i].worldPosition.x - epsilon, output[i].worldPosition.y, output[i].worldPosition.z) - ripplesPoint), instanceID);

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