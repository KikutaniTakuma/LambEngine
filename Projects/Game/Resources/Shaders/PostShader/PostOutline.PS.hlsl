#include "Post.hlsli"

static const float32_t PI = 3.14159265f;

struct Weight{
    float32_t weight;
};

ConstantBuffer<Weight> kWeight : register(b2);

Texture2D<float32_t4> depthTexture : register(t0);
SamplerState pSmp : register(s1);

float32_t4 main(Output input) : SV_TARGET{
    float32_t4 texColor = 0;
    texColor = tex.Sample(pSmp, input.uv);
    float32_t depth;
    return result;
}