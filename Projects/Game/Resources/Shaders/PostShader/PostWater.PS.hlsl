#include "PostWater.hlsli"
#include "Random.hlsli"

cbuffer RamdomVec : register(b2)
{
    float2 randomVec;
}

cbuffer Rotate : register(b3)
{
    float4x4 normalRotate;
}

struct Light
{
    float3 ligDirection;
    float3 ligColor;
    float3 eyePos;

    float3 ptPos;
    float3 ptColor;
    float ptRange;
};

ConstantBuffer<Light> light : register(b4);

float Perlin(float density, float2 uv)
{
    float2 uvFloor = floor(uv * density);
    float2 uvFrac = frac(uv * density);

    float2 v00 = RandomVector(uvFloor + float2(0.0f, 0.0f));
    float2 v01 = RandomVector(uvFloor + float2(0.0f, 1.0f));
    float2 v10 = RandomVector(uvFloor + float2(1.0f, 0.0f));
    float2 v11 = RandomVector(uvFloor + float2(1.0f, 1.0f));

    float c00 = dot(v00, uvFrac - float2(0.0f, 0.0f));
    float c01 = dot(v01, uvFrac - float2(0.0f, 1.0f));
    float c10 = dot(v10, uvFrac - float2(1.0f, 0.0f));
    float c11 = dot(v11, uvFrac - float2(1.0f, 1.0f));

    float2 u = uvFrac * uvFrac * (3.0f - 2.0f * uvFrac);

    float v0010 = lerp(c00, c10, u.x);
    float v0111 = lerp(c01, c11, u.x);

    float n = lerp(v0010, v0111, u.y) / 2.0f + 0.5f;
    
    return n;
}

float FractalSumNnoise(float density, float2 uv)
{
    float fn = 0.0f;
    fn += Perlin(density * 1.0f, uv) * 1.0f / 2.0f;
    fn += Perlin(density * 2.0f, uv) * 1.0f / 4.0f;
    fn += Perlin(density * 4.0f, uv) * 1.0f / 8.0f;
    fn += Perlin(density * 8.0f, uv) * 1.0f / 16.0f;

    return fn;
}

float CreateNoise(float2 uv)
{
    float pn = FractalSumNnoise(10.0f, uv + randomVec);
    float pn2 = FractalSumNnoise(5.0f, uv - randomVec);
    uv.x *= -1.0f;
    float pn3 = FractalSumNnoise(10.0f, uv + randomVec);
    uv.x *= -1.0f;
    uv.y *= -1.0f;
    float pn4 = FractalSumNnoise(5.0f, uv + randomVec);
    
    float noise = lerp((pn * 0.1f), (pn2 * 0.08f), 3.0f);
    float noise2 = lerp((pn3 * 0.1f), (pn4 * 0.08f), 3.0f);
    
    return lerp(noise,noise2, 0.5f);
}

float3 CreateNormal(float2 uv)
{
    float right = CreateNoise(float2(uv.x + 1.0f, uv.y));
    float left = CreateNoise(float2(uv.x - 1.0f, uv.y));
    float up = CreateNoise(float2(uv.x, uv.y + 1.0f));
    float bottom = CreateNoise(float2(uv.x, uv.y - 1.0f));
    
    float dfx = right - left;
    float dfy = up - bottom;
    
    float3 n = normalize(float3(-dfx, -dfy, 2.0f));
    
    float a = 0.01f;
    n = (n / a * 1.0f) * 0.5f;
   
    return n;
}

float4 main(Output input) : SV_TARGET{
    float noise = CreateNoise(input.uv);
    
    float4 texColor = tex.Sample(smp, input.uv + noise);
    
    //float4 normalTmp = float4(CreateNormal(input.uv), 1.0f);
    //normalTmp = mul(normalTmp, normalRotate);
    
    //float3 normal = normalTmp.xyz;
    float3 normal = CreateNormal(input.uv);
    //normal = normalize(normal);
    float3 ligDirection = normalize(float3(1.0f, -1.0f, 0.0f));
    
    // ディレクションライト拡散反射光
    float t = dot(normal, ligDirection);

    t *= -1.0f;
    t = (t + abs(t)) * 0.5f;

    float3 diffDirection = float3(15.0f,15.0f,15.0f) * t;
    
    
    
    float3 refVec = reflect(ligDirection, normal);
    refVec = normalize(refVec);

    float3 toEye = light.eyePos - input.worldPos.xyz;
    toEye = normalize(toEye);

    t = dot(refVec, toEye);
    t = (t + abs(t)) * 0.5f;

    t = pow(t, 5.0f);
    float3 specDirection = float3(15.0f, 15.0f, 15.0f) * t;
    
    
    
    float3 lig = diffDirection /*+ specDirection*/;
    
    lig.xyz += 0.2f;
    
    lig = pow(lig, 0.3f);
    
    //texColor *= color;
    //texColor.xyz *= lig;

    return texColor * color * float4(lig, 1.0f);
    //return float4(normal, 1.0f);
}