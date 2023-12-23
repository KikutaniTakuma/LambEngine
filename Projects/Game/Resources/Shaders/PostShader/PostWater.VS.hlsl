#include "PostWater.hlsli"

Output main(float4 pos : POSITION, float2 uv : TEXCOORD){
    Output output;

    output.svPos = mul(pos, wvpmat);
    output.uv = uv;
    output.worldPos = pos;

    return output;
}