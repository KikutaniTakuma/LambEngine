#include "DeferredRendering.hlsli"
#include "../OutputStructs.hlsli"

static const float32_t PI = 3.14159265f;

struct DirectionLight{
    float32_t3 ligDirection;
    float32_t shinness;
    float32_t3 ligColor;
    float32_t pad;
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

ConstantBuffer<DeferredRenderingData> kDeferredRenderingState : register(b0);
StructuredBuffer<PointLight> gPointLight : register(t0);
Texture2D<float32_t4> kColorTexture : register(t1);
Texture2D<float32_t4> kNormalTexture : register(t2);
Texture2D<float32_t4> kWorldPositionTexture : register(t3);

// ポイントサンプラー
SamplerState pointSmp : register(s0);

PixelShaderOutPut2 main(Output input) {
    float32_t4 color = kColorTexture.Sample(pointSmp, input.uv);
    float32_t4 worldPosition = kWorldPositionTexture.Sample(pointSmp, input.uv);
    float32_t3 normal = kNormalTexture.Sample(pointSmp, input.uv).xyz;
    normal = normalize(normal);
    PixelShaderOutPut2 outputColor;

    if(kDeferredRenderingState.isDirectionLight == 1){
        float32_t3 eyePos = kDeferredRenderingState.eyePos;
        float32_t3 ligDirection = kDeferredRenderingState.directionLight.ligDirection;
        float32_t3 ligColor = kDeferredRenderingState.directionLight.ligColor;
        float32_t shinness = kDeferredRenderingState.directionLight.shinness;
 
        // ディレクションライト拡散反射光
        float32_t t = dot(normal, ligDirection);
        t = saturate(t);

        float32_t3 diffDirection = ligColor * t;

        // 鏡面反射光
        float32_t3 toEye = eyePos - worldPosition.xyz;
        toEye = normalize(toEye);
    
        float32_t3 refVec = reflect(ligDirection, normal);
        refVec = normalize(refVec);

        t = dot(refVec, toEye);

        t = pow(saturate(t), shinness);
        float32_t3 specDirection = ligColor * t;
    
        float32_t3 lig = diffDirection + specDirection;
        lig += 0.2f;
        outputColor.color0.rgb = color.rgb * lig;
    }else{
        outputColor.color0.rgb = color.rgb;
    }

    outputColor.color0.a = 1.0f;
    outputColor.color1 = outputColor.color0;

    return outputColor;
}