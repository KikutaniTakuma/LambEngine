#include "Post.hlsli"
#include "Random.hlsli"

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
    
    float3 n = float3(-dfx, -dfy, 2.0f);
    
    n = normalize(n);
    
    n = (n / 0.02f * 1.0f) * 0.5f;
    
    return n;
}

float4 main(Output input) : SV_TARGET{
    float noise = CreateNoise(input.uv);
    
    float4 texColor = tex.Sample(smp, input.uv + noise);
    
    float3 normal = CreateNormal(input.uv);
    
    float3 ligDirection = normalize(float3(1.0f, -1.0f, 0.0f));
    
    // ディレクションライト拡散反射光
    float t = dot(normal, ligDirection);

    t *= -1.0f;
    t = (t + abs(t)) * 0.5f;

    float3 lig = float3(1.50f,15.0f,15.0f) * t;
    
    lig.xyz += 0.2f;
    
    lig = pow(lig, 0.3f);

    return texColor * color * float4(lig, 1.0f);
}