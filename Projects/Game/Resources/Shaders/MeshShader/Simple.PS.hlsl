#include "../OutputStructs.hlsli"

struct MSOutput {
	float32_t4 position : SV_POSITION;
	float32_t4 color : COLOR;
};

PixelShaderOutPut main(MSOutput input)
{
	PixelShaderOutPut output;
	output.color = input.color;
    
	return output;
}