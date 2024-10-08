#include "Post.hlsli"
#include "../OutputStructs.hlsli"

Texture2D<float32_t4> gDistortionTexture : register(t1);

// ポイントサンプラー
SamplerState gPointSmp : register(s1);

PixelShaderOutPut2 main(Output input) {
    PixelShaderOutPut2 output;

    float32_t2 uvDistortion = gDistortionTexture.Sample(gPointSmp, input.uv).xy;
    float32_t2 scrollDistortion = gDistortionTexture.Sample(gPointSmp, input.uv + uvDistortion).xy;
    if(scrollDistortion.x == 0.0f && scrollDistortion.y == 0.0f) {
        uvDistortion = 0;
    }

    float32_t2 uv = input.uv + uvDistortion;
    uv = clamp(uv, float32_t2(0.0f,0.0f), float32_t2(1.0f, 1.0f));

    output.color0 = tex.Sample(smp, uv) * kColor.color;
    output.color1 = output.color0;

    return output;
}