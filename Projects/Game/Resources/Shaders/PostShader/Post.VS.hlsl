#include "Post.hlsli"

Output main(float32_t4 pos : POSITION, float32_t2 uv : TEXCOORD){
    Output output;

    output.svPos = mul(pos, kWvpMatrix.worldmat);
    output.svPos = mul(output.svPos, kWvpMatrix.viewProjectionMatrix);
    output.uv = uv;

    return output;
}