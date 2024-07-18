#include "Post.hlsli"
#include "../OutputStructs.hlsli"

PixelShaderOutPut2 main(Output input) {
    float32_t4 color;

    color = tex.Sample(smp, input.uv);
    
    float32_t t = dot(color.xyz, float32_t3(0.299f, 0.587f, 0.144f));

    if(t < 0.9f){
        discard;
    }

    color.r = t;
    color.g = t;
    color.b = t;

    PixelShaderOutPut2 output;
    output.color0 = color;
    output.color1 = color;

    return output;
}