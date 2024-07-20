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
    float32_t environmentCoefficient;
};

struct PointLight {
    float32_t3 ptPos;
    float32_t3 ptColor;
    float32_t ptRange;
};

ConstantBuffer<DeferredRenderingData> gDeferredRenderingState : register(b0);
StructuredBuffer<PointLight> gPointLight : register(t0);
Texture2D<float32_t4> gColorTexture : register(t1);
Texture2D<float32_t4> gNormalTexture : register(t2);
Texture2D<float32_t4> gWorldPositionTexture : register(t3);
TextureCube<float32_t4> gCubeTex : register(t4);

// リニアサンプラー
SamplerState gLinearSmp : register(s0);
// ポイントサンプラー
SamplerState gPointSmp : register(s01);

PixelShaderOutPut2 main(Output input) {
    float32_t4 color = gColorTexture.Sample(gLinearSmp, input.uv);
    float32_t4 worldPosition = gWorldPositionTexture.Sample(gPointSmp, input.uv);
    float32_t3 normal = gNormalTexture.Sample(gPointSmp, input.uv).xyz;
    float32_t len = length(normal);
    normal = normalize(normal);
    PixelShaderOutPut2 outputColor;

    float32_t3 eyePos = gDeferredRenderingState.eyePos;

    if(len != 0.0f){
        float32_t3 cameraToPosition = normalize(worldPosition.xyz - eyePos);
        float32_t3 reflectedVector = reflect(cameraToPosition, normal);
        float32_t4 environment = gCubeTex.Sample(gLinearSmp, reflectedVector);

        color.rgb += environment.rgb * gDeferredRenderingState.environmentCoefficient;
    }

    if(gDeferredRenderingState.isDirectionLight == 1 && len != 0.0f){
        float32_t3 ligDirection = gDeferredRenderingState.directionLight.ligDirection;
        float32_t3 ligColor = gDeferredRenderingState.directionLight.ligColor;
        float32_t shinness = gDeferredRenderingState.directionLight.shinness;
 
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