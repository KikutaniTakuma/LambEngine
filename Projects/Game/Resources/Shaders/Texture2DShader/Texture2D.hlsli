SamplerState smp : register(s0);

StructuredBuffer<float4> color : register(t1);
StructuredBuffer<uint32_t> textureIndex : register(t2);
Texture2D<float4> tex[] : register(t3);

struct Output{
    float4 svPos : SV_POSITION;
    float2 uv : TEXCOORD;
    uint32_t instanceId : BLENDINDICES;
};