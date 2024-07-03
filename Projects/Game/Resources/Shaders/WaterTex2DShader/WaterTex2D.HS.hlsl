#include "WaterTex2D.hlsli"

ConstantHullShaderOutPut ConstantsHS_Main( InputPatch<WaterTex2DVertexOutPut, 3> input, uint32_t PatchID : SV_PrimitiveID )
{
  ConstantHullShaderOutPut output;
  uint32_t instanceID = input[0].outputData.instanceID;

  output.Edges[0] = output.Edges[1] = output.Edges[2] = kWaterData[instanceID].edgeDivision;
  output.Inside = kWaterData[instanceID].insideDivision;

  return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantsHS_Main")]
WaterTex2DHullShaderOutPut main( InputPatch<WaterTex2DVertexOutPut, 3> input, uint32_t i : SV_OutputControlPointID, uint32_t PatchID : SV_PrimitiveID ) {
	WaterTex2DHullShaderOutPut output;
	uint32_t ID = 2 - i;

	output.outputData.position = input[ID].outputData.position;
	output.outputData.normal = input[ID].outputData.normal;
	output.outputData.worldPosition = input[ID].outputData.worldPosition;
	output.outputData.uv = input[ID].outputData.uv;
	output.outputData.textureID = input[ID].outputData.textureID;
	output.outputData.instanceID = input[ID].outputData.instanceID;
	
	output.causticsUv = input[ID].causticsUv;

	return output;
}