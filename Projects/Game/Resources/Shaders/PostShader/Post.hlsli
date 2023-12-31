Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

cbuffer Matrix : register(b0){
    float4x4 worldmat;
    float4x4 viewProjectionMatrix;
}

cbuffer Color : register(b1){
    float4 color;
}

struct Output{
    float4 svPos : SV_POSITION;
    float2 uv : TEXCOORD;
};