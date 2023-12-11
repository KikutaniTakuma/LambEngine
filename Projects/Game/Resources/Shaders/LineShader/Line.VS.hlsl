#include "Line.hlsli"

StructuredBuffer<float4x4> mat: register(t0);
StructuredBuffer<float4> color: register(t1);

VertexOutput main(float4 pos : POSITION0, uint32_t instanceId : SV_InstanceID) {
    VertexOutput output;
    output.pos = mul(pos,mat[instanceId]);
    output.color = color[instanceId];

    return output;
}