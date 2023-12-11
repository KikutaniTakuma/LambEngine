#include "Line.hlsli"

StructuredBuffer<float4x4> mat: register(t0);

VertexOutput main(float4 pos : POSITION0, float4 color : COLOR0, uint matIndex : BLENDINDICES) {
    VertexOutput output;
    output.pos = mul(pos,mat[matIndex]);
    output.color = color;

    return output;
}