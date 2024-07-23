#include "Post.hlsli"
#include "../OutputStructs.hlsli"
#include "../HSV.hlsli"

PixelShaderOutPut2 main(Output input) {
    PixelShaderOutPut2 output;
    output.color0 =  tex.Sample(smp, input.uv);
    float32_t3 hsv = RGBToHSV(output.color0.rgb);
    output.color0.rgb = HSVToRGB(hsv);

    output.color1 = output.color0;

    return output;
}