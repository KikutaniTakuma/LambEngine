#include "Post.hlsli"
#include "../OutputStructs.hlsli"

PixelShaderOutPut2 main(Output input) {
    PixelShaderOutPut2 output;
    output.color0 =  tex.Sample(smp, input.uv);
    output.color1 = output.color0;

    return output;
}