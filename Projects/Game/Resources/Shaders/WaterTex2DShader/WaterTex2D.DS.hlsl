#include "WaterTex2D.hlsli"

[domain("tri")]
WaterTex2DDomainShaderOutPut main(ConstantHullShaderOutPut input,  float32_t3 domain : SV_DomainLocation, const OutputPatch<WaterTex2DHullShaderOutPut, 3> patch){
    WaterTex2DDomainShaderOutPut output;

    output.outputData.position = 
		patch[0].outputData.position * domain.x 
		+ patch[1].outputData.position * domain.y 
		+ patch[2].outputData.position * domain.z;
	output.outputData.worldPosition = output.outputData.position;
	
	output.outputData.normal = patch[0].outputData.normal;
	output.outputData.uv = 
		patch[0].outputData.uv * domain.x 
		+ patch[1].outputData.uv * domain.y 
		+ patch[2].outputData.uv * domain.z;

	output.outputData.textureID = patch[0].outputData.textureID;
	output.outputData.instanceID = patch[0].outputData.instanceID;
	
	output.causticsUv = patch[0].causticsUv;

    return output;
}