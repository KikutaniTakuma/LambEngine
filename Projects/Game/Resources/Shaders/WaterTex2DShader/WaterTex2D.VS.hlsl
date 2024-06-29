#include "WaterTex2D.hlsli"

WaterTex2DVertexOutPut main(VertexShaderInput input,uint32_t instanceID : SV_InstanceID)
{
    VertexShaderOutput output;

	output.worldPosition = input.position;
	output.position =output.worldPosition;
	output.normal = mul(input.normal, (float32_t3x3)kWvpMat[instanceID].worldMat);

	output.uv = input.uv;
	output.instanceID = instanceID;
	output.textureID = input.textureID;

	WaterTex2DVertexOutPut outputData;
	outputData.outputData = output;

	float32_t3 N = normalize(mul(kWaterData[instanceID].normal, (float32_t3x3) kWvpMat[instanceID].worldMat));
    float32_t3 T = normalize(mul(kWaterData[instanceID].tangent, (float32_t3x3) kWvpMat[instanceID].worldMat));
    float32_t3 B = normalize(cross(N, T));
    
    outputData.tangentBasis = transpose(float32_t3x3(T, B, N));
    outputData.causticsUv = output.uv * 10.0f;

    return outputData;
}