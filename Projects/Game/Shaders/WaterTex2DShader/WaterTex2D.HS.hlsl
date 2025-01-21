#include "WaterTex2D.hlsli"

ConstantHullShaderOutPut ConstantsHS_Main( InputPatch<VertexShaderOutputToHull, 3> input, uint32_t PatchID : SV_PrimitiveID ) {
	ConstantHullShaderOutPut output;
	uint32_t instanceID = input[0].instanceID;

	WaterData data = kWaterData[instanceID];

	float32_t3 avgPos = 0;
	for(uint32_t i = 0; i < 3; ++i){
		float32_t3 worldPos = mul(input[i].worldPosition, kWvpMat[instanceID].worldMat).xyz;
		float32_t posToCameraLength = length(worldPos - data.cameraPosition);

		float32_t edgeDivision = clamp(float32_t(data.edgeDivision) * ((data.distanceThreshold) * rcp(max(posToCameraLength, data.distanceThreshold))), 1.0f, float32_t(data.edgeDivision));

		output.Edges[i] = int32_t(edgeDivision);

		avgPos += worldPos;
	}

	avgPos *= rcp(3.0f);

	float32_t posToCameraLength = length(avgPos - data.cameraPosition);
	float32_t insideDivision = clamp(float32_t(data.insideDivision) * ((data.distanceThreshold) * rcp(max(posToCameraLength, data.distanceThreshold))), 1.0f, float32_t(data.insideDivision));

	output.Inside = int32_t(insideDivision);

	return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantsHS_Main")]
HullShaderOutPut main( InputPatch<VertexShaderOutputToHull, 3> input, uint32_t i : SV_OutputControlPointID, uint32_t PatchID : SV_PrimitiveID ) {
	HullShaderOutPut output;
	uint32_t ID = 2 - i;

	output.position = input[ID].position;
	output.normal = input[ID].normal;
	output.worldPosition = input[ID].worldPosition;
	output.uv = input[ID].uv;
	output.textureID = input[ID].textureID;
	output.instanceID = input[ID].instanceID;

	return output;
}