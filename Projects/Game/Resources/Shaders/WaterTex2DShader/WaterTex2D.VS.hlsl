#include "WaterTex2D.hlsli"

WaterTex2DVertexOutPut main(VertexShaderInput input,uint32_t instanceID : SV_InstanceID)
{
    VertexShaderOutput output;

    input.position = mul(input.position, kWvpMat[instanceID].worldMat);
	output.worldPosition = input.position;
	output.position = mul(input.position, kWvpMat[instanceID].cameraMat);
	input.normal = normalize(input.normal);
	output.normal = mul(input.normal, (float32_t3x3)kWvpMat[instanceID].worldMat);

	output.uv = input.uv;
	output.instanceID = instanceID;
	output.textureID = input.textureID;

	WaterTex2DVertexOutPut.outputData = output;

	float32_t3 N = normalize(mul(kWaterData[instanceID].normal, (float32_t3x3) kWvpMat[instanceID].worldMat));
    float32_t3 T = normalize(mul(kWaterData[instanceID].tangent, (float32_t3x3) kWvpMat[instanceID].worldMat));
    float32_t3 B = normalize(cross(N, T));
    
    output.tangentBasis = transpose(float32_t3x3(T, B, N));

    return output;
}