#include "SkyBox.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    output.worldPosition = mul(input.position, gMaterialData.worldMat);
    output.position = mul(output.worldPosition, gMaterialData.viewProjectionMat);
	output.texcoord = input.position.xyz;

    return output;
}