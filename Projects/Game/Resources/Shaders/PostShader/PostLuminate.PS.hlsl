#include "Post.hlsli"
#include "../OutputStructs.hlsli"

PixelShaderOutPut main(Output input) {
    float32_t4 color;

    color = tex.Sample(smp, input.uv);
    
    float32_t t = dot(color.xyz, float32_t3(0.299f, 0.587f, 0.144f));

    if(t < 0.99f){
        discard;
    }

    color.r = t;
    color.g = t;
    color.b = t;

    PixelShaderOutPut output;
    output.color = color;

    return output;
}