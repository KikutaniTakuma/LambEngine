#include "SkyBox.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    output.position = mul(input.position, gMaterialData.wvpMat);
	output.texcoord = input.position.xyz;

    return output;
}