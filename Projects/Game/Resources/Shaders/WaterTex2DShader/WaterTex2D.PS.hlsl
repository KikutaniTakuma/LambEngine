#include "WaterTex2D.hlsli"
#include "../PerlinNoise.hlsli"

PixelShaderOutPut2 main(WaterTex2DVertexOutPut waterinput)
{
    VertexShaderOutput input = waterinput.outputData;

	PixelShaderOutPut2 output;

    uint32_t textureID = kWaterData[input.instanceID].textureID;

// お水の処理
	const float32_t2 kRandomVec = kWaterData[input.instanceID].randomVec;
    const float32_t kDensity = kWaterData[input.instanceID].density;
 	float32_t noise = CreateNoise(input.uv, kRandomVec, kDensity);
    
    float32_t4 causticsColor = textures[textureID].Sample(smp, waterinput.causticsUv + frac(CreateNoiseNoDdy(waterinput.causticsUv * 0.1f, kRandomVec, kDensity)));
    
    float32_t3 normal = CreateNormal(input.uv, kRandomVec, kDensity);

    float32_t3 ligDirection = kLight.ligDirection;
    ligDirection = mul(ligDirection, waterinput.tangentBasis);
    
    // ディレクションライト拡散反射光
    float32_t t = dot(normal, -ligDirection);
    t = saturate(t);

    float32_t3 diffDirection = kLight.ligColor * t;
    
    
    float32_t3 toEye = kLight.eyePos - input.worldPosition.xyz;
    toEye = mul(toEye, waterinput.tangentBasis);
    toEye = normalize(toEye);
    
    float32_t3 refVec = -reflect(toEye, normal);
    refVec = normalize(refVec);

    t = dot(refVec, toEye);

    t = pow(saturate(t), 128.0f);
    float32_t3 specDirection = kLight.ligColor * t;
    
    float32_t3 lig = diffDirection + specDirection;
    
    lig.xyz += 0.2f;
    
    //output.color0 = causticsColor + kColor[input.instanceID].color;
    output.color0 = kColor[input.instanceID].color;
    output.color0.xyz *= lig;
    output.color1 = output.color0;

    return output;
}