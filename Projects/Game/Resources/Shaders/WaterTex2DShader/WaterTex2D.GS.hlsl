#include "../Normal.hlsli"
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

	return maxHeight * pow(M_E, -time * timeAttenuation) * sin(waveLength * (length - waveSpeed * time));
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
	triangle WaterTex2DDomainShaderOutPut input[3], 
	inout TriangleStream<WaterTex2DGeometoryShaderOutPut> outStream
){
	WaterTex2DGeometoryShaderOutPut output[3];
	uint32_t instanceID = input[0].outputData.instanceID;

	float32_t3 ripplesPoint = kWaterData[instanceID].waveData.ripplesPoint;

	for(uint32_t i = 0; i < 3; ++i){
		// ワールドポジション計算
    	output[i].outputData.position = input[i].outputData.position;
		output[i].outputData.worldPosition = mul(output[i].outputData.position, kWvpMat[instanceID].worldMat);
		float32_t3 ripplesNormal = CalcNormal(output[i].outputData.worldPosition.xyz, 0.001f, instanceID);
		float32_t3 tangent = NormalToTangent(input[i].outputData.normal);
		float32_t3 binormal = CalcBinormal(input[i].outputData.normal, tangent);
		output[i].outputData.normal = BlendNormal(input[i].outputData.normal, tangent, binormal, ripplesNormal);
		
		// 波紋からの長さ
		float32_t ripplesPointToPos = length(output[i].outputData.worldPosition.xyz - ripplesPoint);
		float32_t height = Waves(ripplesPointToPos, instanceID);
		output[i].outputData.worldPosition.y += height;

		
		output[i].outputData.position = mul(output[i].outputData.worldPosition, kWvpMat[instanceID].cameraMat);

		output[i].outputData.uv = input[i].outputData.uv;
		output[i].outputData.textureID = input[i].outputData.textureID;
		output[i].outputData.instanceID = input[i].outputData.instanceID;

		float32_t3 N = normalize(mul(input[i].outputData.normal, (float32_t3x3) kWvpMat[instanceID].worldMat));
    	float32_t3 T = normalize(mul(NormalToTangent(input[i].outputData.normal), (float32_t3x3) kWvpMat[instanceID].worldMat));
    	float32_t3 B = normalize(cross(N, T));

		/*float32_t3 N = normalize(mul(kWaterData[instanceID].normal, (float32_t3x3) kWvpMat[instanceID].worldMat));
    	float32_t3 T = normalize(mul(kWaterData[instanceID].tangent, (float32_t3x3) kWvpMat[instanceID].worldMat));
    	float32_t3 B = normalize(cross(N, T));*/

		output[i].tangentBasis = transpose(float32_t3x3(T, B, N));
		output[i].causticsUv = input[i].causticsUv;

		outStream.Append(output[i]);
	}

	outStream.RestartStrip();
}