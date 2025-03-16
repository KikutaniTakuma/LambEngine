#include "../Normal.hlsli"
#include "../PerlinNoise.hlsli"
#include "WaterTex2D.hlsli"
#include "./Gerstner.hlsli"

#define M_PI 3.141592653589793238462643
#define M_E 2.71828182846



static float32_t radBasis = 0.5f;

float32_t Waves(float32_t3 worldPos, uint32_t instanceID){

	float32_t maxHeight = kWaterData[instanceID].waveData.waveStrength;
	float32_t waveSpeed = kWaterData[instanceID].waveData.waveSpeed;
	float32_t lengthAttenuation = kWaterData[instanceID].waveData.lengthAttenuation;
	float32_t timeAttenuation = kWaterData[instanceID].waveData.timeAttenuation;

	float32_t ripples = kWaterData[instanceID].waveData.ripples;
	float32_t waveLength = 2.0f * M_PI * rcp(ripples);


	float32_t wave = 0.0f;

	for(int32_t i = 0; i < kMaxRipplePoints; i++){
		float32_t3 ripplesPoint = kWaterData[instanceID].waveData.ripplesPoint[i];
		float32_t len = length(worldPos - ripplesPoint);
		float32_t time = kWaterData[instanceID].waveData.time[i];
		float32_t waveLen = waveSpeed * time;

		// 波からの距離(波が通り過ぎてれば+)
		float32_t waveToPosLen = waveLen - len;

		// 最大の高さ * sin波 * 波が到達してたら1をそれ以外は0を返す * 距離減衰 * 時間減衰
		wave += maxHeight * sin(waveLength * -waveToPosLen) 
			* min(1.0f, ceil(max(0.0f, waveToPosLen))) 
			* pow(M_E, -len * lengthAttenuation) * pow(M_E, -max(waveToPosLen, 0.0f) * timeAttenuation);
	}

	return wave;
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
		float32_t4 worldPosition = mul(output[i].position, kWvpMat[instanceID].worldMat);
		output[i].worldPosition = worldPosition;

		

		/*
		float32_t wavePower = 6.0f;
		float32_t epsilon = 0.0001f;
		float32_t subUV = 1.0f * rcp(400.0f) * epsilon;
		float32_t height = CreateNoise(inputTmp.uv, kRandomVec, kDensity) * wavePower + waveHeight;
		float32_t up = CreateNoise(float32_t2(inputTmp.uv.x, inputTmp.uv.y + subUV), kRandomVec, kDensity) * wavePower + Waves(float32_t3(output[i].worldPosition.x, output[i].worldPosition.y, output[i].worldPosition.z + epsilon), instanceID);
		float32_t down = CreateNoise(float32_t2(inputTmp.uv.x, inputTmp.uv.y - subUV), kRandomVec, kDensity) * wavePower + Waves(float32_t3(output[i].worldPosition.x, output[i].worldPosition.y, output[i].worldPosition.z - epsilon), instanceID);
		float32_t right = CreateNoise(float32_t2(inputTmp.uv.x + subUV, inputTmp.uv.y), kRandomVec, kDensity) * wavePower + Waves(float32_t3(output[i].worldPosition.x + epsilon, output[i].worldPosition.y, output[i].worldPosition.z), instanceID);
		float32_t left = CreateNoise(float32_t2(inputTmp.uv.x - subUV, inputTmp.uv.y), kRandomVec, kDensity) * wavePower + Waves(float32_t3(output[i].worldPosition.x - epsilon, output[i].worldPosition.y, output[i].worldPosition.z), instanceID);
		*/

		float32_t wavePower = 0.2f;
		float32_t time = kWaterData[instanceID].time;

		// 波の高さ
		float32_t3 gerstner = GenerateWave(worldPosition.xyz, time) * wavePower;
		gerstner.y += Waves(worldPosition.xyz, instanceID);

		float32_t kEpsilon = 0.0001f;

		float32_t3 upPos = worldPosition.xyz;
		upPos.z += kEpsilon;
		float32_t3 downPos = worldPosition.xyz;
		downPos.z -= kEpsilon;
		float32_t3 rightPos = worldPosition.xyz;
		rightPos.x += kEpsilon;
		float32_t3 leftPos = worldPosition.xyz;
		leftPos.x -= kEpsilon;
		float32_t up = GenerateWave(upPos, time).y * wavePower + Waves(upPos, instanceID);
		float32_t down = GenerateWave(downPos, time).y * wavePower + Waves(downPos.xyz, instanceID);
		float32_t right = GenerateWave(rightPos, time).y * wavePower + Waves(rightPos, instanceID);
		float32_t left = GenerateWave(leftPos, time).y * wavePower + Waves(leftPos, instanceID);

		float32_t yx = (right - left) * rcp(2.0f * kEpsilon);
		float32_t yz = (up - down) * rcp(2.0f * kEpsilon);

		float32_t3 resultNormal = normalize(float32_t3(-yx, 1.0f, -yz));
		output[i].normal = resultNormal;

		output[i].worldPosition.xyz += gerstner;

		
		output[i].position = mul(output[i].worldPosition, kWvpMat[instanceID].cameraMat);

		output[i].uv = inputTmp.uv;
		output[i].textureID = inputTmp.textureID;
		output[i].instanceID = inputTmp.instanceID;

		outStream.Append(output[i]);
	}

	outStream.RestartStrip();
}