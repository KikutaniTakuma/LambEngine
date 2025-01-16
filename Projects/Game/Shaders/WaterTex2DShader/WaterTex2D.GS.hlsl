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
		float32_t wavePower = 20.0f;
		float32_t height = CreateNoise(inputTmp.uv, kRandomVec, kDensity) * wavePower;
		float32_t up = CreateNoise(float32_t2(inputTmp.uv.x, inputTmp.uv.y + 0.00025f), kRandomVec, kDensity) * wavePower;
		float32_t down = CreateNoise(float32_t2(inputTmp.uv.x, inputTmp.uv.y - 0.00025f), kRandomVec, kDensity) * wavePower;
		float32_t right = CreateNoise(float32_t2(inputTmp.uv.x + 0.00025f, inputTmp.uv.y), kRandomVec, kDensity) * wavePower;
		float32_t left = CreateNoise(float32_t2(inputTmp.uv.x - 0.00025f, inputTmp.uv.y), kRandomVec, kDensity) * wavePower;

		float32_t3 currentPos = output[i].worldPosition.xyz;
		float32_t3 upPos = currentPos;
		upPos.z += 0.1f;
		upPos.y += up;
		float32_t3 downPos = currentPos;
		downPos.z -= 0.1f;
		downPos.y += down;
		float32_t3 rightPos = currentPos;
		rightPos.x += 0.1f;
		rightPos.y += right;
		float32_t3 leftPos = currentPos;
		leftPos.x -= 0.1f;
		leftPos.y += left;

		float32_t3 normal0 = normalize(cross(currentPos - upPos, leftPos - currentPos));
		float32_t3 normal1 = normalize(cross(currentPos - rightPos, upPos - currentPos));
		float32_t3 normal2 = normalize(cross(currentPos - downPos, rightPos - currentPos));
		float32_t3 normal3 = normalize(cross(currentPos - leftPos, downPos - currentPos));

		float32_t3 resultNormal = (normal0 + normal1 + normal2 + normal3) * 0.25f;
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