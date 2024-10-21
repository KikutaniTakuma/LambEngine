#include "Post.hlsli"
#include "../OutputStructs.hlsli"

Texture2D<float32_t4> gDistortionTexture : register(t1);
Texture2D<float32_t> gDepthTexture : register(t2);
Texture2D<float32_t4> gCausticsTexture : register(t3);

struct Mat4x4{
    float32_t4x4 value;
};

struct Float{
    float32_t value;
};

ConstantBuffer<Mat4x4> gUVScroll : register(b1);
ConstantBuffer<Float> gDepthFloat : register(b2);

// ポイントサンプラー
SamplerState gPointSmp : register(s1);

PixelShaderOutPut2 main(Output input) {
    PixelShaderOutPut2 output;


    float32_t depth = gDepthTexture.Sample(gPointSmp, input.uv);
    depth = 1.0f - pow(depth, 120.0f);

    float32_t2 uvDistortion = gDistortionTexture.Sample(gPointSmp, input.uv).xy;
    float32_t2 scrollDistortion = gDistortionTexture.Sample(gPointSmp, input.uv + uvDistortion).xy;
    if(scrollDistortion.x == 0.0f && scrollDistortion.y == 0.0f) {
        uvDistortion = 0;
    }

    float32_t2 uv = input.uv + uvDistortion;
    uv = clamp(uv, float32_t2(0.0f,0.0f), float32_t2(1.0f, 1.0f));

    float32_t4 inputUV = 0;
    inputUV.xy = input.uv;
    inputUV.z = 0.0f;
    inputUV.w = 1.0f;

    float32_t4 color = tex.Sample(gPointSmp, uv);
    if(depth < gDepthFloat.value && !(scrollDistortion.x == 0.0f && scrollDistortion.y == 0.0f)){
        float32_t4 uvDistortionTmp = 0;
        uvDistortionTmp.xy = uvDistortion;
        uvDistortionTmp.z = 0;
        uvDistortionTmp.w = 1.0f;
        float32_t4 caustics = gCausticsTexture.Sample(smp, mul(inputUV, gUVScroll.value).xy + mul(uvDistortionTmp, gUVScroll.value).xy);
        color.rgb += caustics.rgb * depth;
    }

    output.color0 = color * kColor.color;
    output.color1 = output.color0;

    return output;
}