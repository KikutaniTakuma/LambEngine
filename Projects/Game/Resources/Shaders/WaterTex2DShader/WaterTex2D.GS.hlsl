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
		
		// 波紋からの長さ
		float32_t ripplesPointToPos = length(output[i].outputData.worldPosition.xyz - ripplesPoint);
		float32_t height = Waves(ripplesPointToPos, instanceID);
		output[i].outputData.worldPosition.y += height;
		
		output[i].outputData.position = mul(output[i].outputData.worldPosition, kWvpMat[instanceID].cameraMat);

		output[i].outputData.normal = input[i].outputData.normal;
		output[i].outputData.uv = input[i].outputData.uv;
		output[i].outputData.textureID = input[i].outputData.textureID;
		output[i].outputData.instanceID = input[i].outputData.instanceID;
		output[i].tangentBasis = input[i].tangentBasis;
		output[i].causticsUv = input[i].causticsUv;

		outStream.Append(output[i]);
	}

	outStream.RestartStrip();
}