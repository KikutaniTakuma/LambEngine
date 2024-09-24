#include "../OutputStructs.hlsli"

struct MSOutput {
	float32_t4 position : SV_POSITION;
	float32_t4 color : COLOR;
};

PixelShaderOutPut3 main(MSOutput input)
{
	PixelShaderOutPut3 output;
	output.color0 = input.color;
    output.color1 = output.color0;
    output.color2 = output.color0;

	return output;
}