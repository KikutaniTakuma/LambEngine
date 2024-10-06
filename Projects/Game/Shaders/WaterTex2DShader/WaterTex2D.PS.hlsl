#include "WaterTex2D.hlsli"
#include "../PerlinNoise.hlsli"
#include "../Normal.hlsli"

PixelShaderOutPut4 main(GeometoryOutPut input)
{
	PixelShaderOutPut4 output;

    // お水の処理
	const float32_t2 kRandomVec = kWaterData[input.instanceID].randomVec;
    const float32_t kDensity = kWaterData[input.instanceID].density;
    
    float32_t3 perlinNormal = CreateNormal(input.uv, kRandomVec, kDensity);
    float32_t3 normal = input.normal;
    float32_t3 tangent = NormalToTangent(perlinNormal);
    float32_t3 binormal = CalcBinormal(perlinNormal, tangent);
    float32_t3 blendNormal = BlendNormal(perlinNormal, tangent, binormal, normal);

    float32_t3 ligDirection = kLight.ligDirection;
    float32_t3 ligColor = kLight.ligColor;
    float32_t shinness = kLight.shinness;
 
    // ディレクションライト拡散反射光
    float32_t t = dot(blendNormal, ligDirection);
    t = saturate(t);

    float32_t3 diffDirection = ligColor * t;

    // 鏡面反射光
    float32_t3 toEye = kCameraPos.pos - input.worldPosition.xyz;
    toEye = normalize(toEye);
    
    float32_t3 refVec = reflect(ligDirection, blendNormal);
    refVec = normalize(refVec);

    t = dot(refVec, toEye);

    t = pow(saturate(t), shinness);
    float32_t3 specDirection = ligColor * t;
    
    float32_t3 lig = diffDirection + specDirection;
    lig += 0.2f;
    output.color0.rgb = clamp(kColor[input.instanceID].color.rgb /** lig*/, float32_t3(0.0f,0.0f,0.0f), float32_t3(1.0f, 1.0f, 1.0f));
    
    // 色
    output.color0.w = kColor[input.instanceID].color.w;

    // 法線
    output.color1.xyz = blendNormal;
    output.color1.w = 1.0f;

    // ポジション
    output.color2 = input.worldPosition;

    output.color3 = 0;
    output.color3.x = CreateNoiseNoDdy(input.uv + float32_t2(1.0f, 0.0f), kRandomVec, kDensity) * 0.5f;
    output.color3.x -= CreateNoiseNoDdy(input.uv - float32_t2(1.0f, 0.0f), kRandomVec, kDensity) * 0.5f;

    return output;
}