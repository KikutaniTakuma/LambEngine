#include "WaterTex2D.hlsli"
#include "../PerlinNoise.hlsli"

PixelShaderOutPut main(WaterTex2DVertexOutPut waterinput)
{
    VertexShaderOutput input = waterinput.outputData;

	PixelShaderOutPut output;

    uint32_t textureID = kWaterData[input.instanceID].textureID;

// お水の処理
	float32_t2 kRandomVec = kWaterData[input.instanceID].randomVec;
 	float32_t noise = CreateNoise(input.uv, kRandomVec, 1.0f);
    
    float32_t4 causticsColor = textures[textureID].Sample(smp, input.uv + frac(CreateNoiseNoDdy(input.uv * 0.1f, kRandomVec, 1.0f)));
    
    float32_t3 normal = CreateNormal(input.uv, kRandomVec, 1.0f);
    //normal = mul(normal, waterinput.tangentBasis);
    //normal = (normal.xyz + 1.0f) * 0.5f;

    float32_t3 ligDirection = kLight.ligDirection;
    ligDirection = mul(ligDirection, waterinput.tangentBasis);
    
    // ディレクションライト拡散反射光
    float32_t t = dot(normal, -ligDirection);

    //t *= -1.0f;
    //t = (t + abs(t)) * 0.5f;
    
    t = saturate(t);
    //t = pow(t, 2.0f);

    float32_t3 diffDirection = kLight.ligColor * t * 1.0f;
    
    
    float32_t3 toEye = kLight.eyePos - input.worldPosition.xyz;
    toEye = mul(toEye, waterinput.tangentBasis);
    toEye = normalize(toEye);
    
    float32_t3 refVec = -reflect(toEye, normal);
    refVec = normalize(refVec);

    t = dot(refVec, toEye);

    t = pow(saturate(t), 128.0f);
    float32_t3 specDirection = kLight.ligColor * t;
    
    float32_t3 lig = diffDirection + specDirection;
    //float32_t3 lig = specPerlin;
    
    lig.xyz += 0.3f;
    
    //lig = pow(lig, 1.0f);
    //kColor[input.instanceID].color + 
    output.color = causticsColor + kColor[input.instanceID].color;
    output.color.xyz *= lig;

    return output;
}