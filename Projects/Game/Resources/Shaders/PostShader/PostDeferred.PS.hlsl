#include "Post.hlsli"

static const float32_t PI = 3.14159265f;

struct DirectionLight{
    float32_t3 ligDirection;
    float32_t3 ligColor;
};

struct DeferredRenderingData{
    float32_t3 eyePos;
    uint32_t rightNum;
    DirectionLight directionLight;
    uint32_t isDirectionLight;
};

struct PointLight {
    float32_t3 ptPos;
    float32_t3 ptColor;
    float32_t ptRange;
};

ConstantBuffer<DeferredRenderingData> kDeferredRenderingState : register(b1);
StructuredBuffer<PointLight> gPointLight : register(t1);
Texture2D<float32_t4> kColorTexture : register(t2);
Texture2D<float32_t4> kNormalTexture : register(t3);
Texture2D<float32_t4> kWorldPositionTexture : register(t4);

SamplerState pointSmp : register(s1);

float32_t4 main(Output input) : SV_TARGET{
    float32_t4 color = kColorTexture.Sample(smp, input.uv);
    float32_t4 worldPosition = kWorldPositionTexture.Sample(pointSmp, input.uv);
    float32_t3 normal = kNormalTexture.Sample(pointSmp, input.uv).xyz;
    normal *= 2.0f;
    normal.xyz -= 1.0f;
    normal = normalize(normal);
    float32_t3 diffDirection = 0;

    if(kDeferredRenderingState.isDirectionLight == 1){
        float32_t3 ligDirection = kDeferredRenderingState.directionLightligDirection;
 
        // ディレクションライト拡散反射光
        float32_t directionStrength = dot(normal, ligDirection);
        directionStrength = saturate(directionStrength);

        diffDirection = kLight.ligColor * directionStrength;
    }

    float32_t4 outputColor = color * diffDirection;
    outputColor.rgb += 0.2f;
    outputColor.a = 1.0f;

    return outputColor;
}