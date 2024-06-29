#include "WaterTex2D.hlsli"

[maxvertexcount(3)]
void main(
	triangle WaterTex2DDomainShaderOutPut input[3], 
	inout TriangleStream<WaterTex2DGeometoryShaderOutPut> outStream
){
	WaterTex2DGeometoryShaderOutPut output[3];
	uint32_t instanceID = input[0].outputData.instanceID;

	for(uint32_t i = 0; i < 3; ++i){
    	output[i].outputData.position = input[i].outputData.position;
		output[i].outputData.worldPosition = mul(output[i].outputData.position, kWvpMat[instanceID].worldMat);
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