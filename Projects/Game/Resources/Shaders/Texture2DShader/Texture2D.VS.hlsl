#include "Texture2D.hlsli"

struct Matrix{
    float4x4 worldMat;
    float4x4 uvMat;
};

StructuredBuffer<Matrix> mat : register(t0);

Output main(float4 pos : POSITION, float2 uv : TEXCOORD, uint32_t instanceId : SV_InstanceID){
    Output output;

    output.svPos = mul(pos, mat[instanceId].worldMat);
    float4 uvTmp = float4(uv,0.0f,1.0f);
    output.uv = mul(uvTmp, mat[instanceId].uvMat).xy;
    output.instanceId = instanceId;

    return output;
}