#include "WaterTex2D.hlsli"

[maxvertexcount(3)]
void main(
	triangle WaterTex2DDomainShaderOutPut input[3], 
	inout TriangleStream<WaterTex2DGeometoryShaderOutPut> outStream
){
	WaterTex2DGeometoryShaderOutPut output[3];
	for(uint32_t i = 0; i < 3; ++i){
    	output[i].outputData.position = input[i].outputData.position;
		output[i].outputData.normal = input[i].outputData.normal;
		output[i].outputData.worldPosition = input[i].outputData.worldPosition;
		output[i].outputData.uv = input[i].outputData.uv;
		output[i].outputData.textureID = input[i].outputData.textureID;
		output[i].outputData.instanceID = input[i].outputData.instanceID;
		output[i].tangentBasis = input[i].tangentBasis;
		output[i].causticsUv = input[i].causticsUv;

		outStream.Append(output[i]);
	}

	outStream.RestartStrip();
}