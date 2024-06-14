#include "SkyBox.hlsli"

PixelOutPut main(VertexOutput input)
{
	PixelOutPut output;

	float32_t4 textureColor = gCubeTex.Sample(smp, input.texcoord);

	output.color = textureColor * gMaterialData.color;

	return output;
}