#include "Post.hlsli"

float32_t4 main(Output input) : SV_TARGET{
    float32_t w, h, levels;
    tex.GetDimensions(0, w,h,levels);

    float32_t dx = 1.0f / w;
    float32_t dy = 1.0f / h;
    float32_t4 ret = float32_t4(0.0f,0.0f,0.0f,0.0f);

    // 最上段
    ret += tex.Sample(smp, input.uv + float32_t2(-2.0f * dx, 2.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2(-1.0f * dx, 2.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 0.0f * dx, 2.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 1.0f * dx, 2.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 2.0f * dx, 2.0f * dy));

    // 一つ上
    ret += tex.Sample(smp, input.uv + float32_t2(-2.0f * dx, 1.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2(-1.0f * dx, 1.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 0.0f * dx, 1.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 1.0f * dx, 1.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 2.0f * dx, 1.0f * dy));

    // 真ん中
    ret += tex.Sample(smp, input.uv + float32_t2(-2.0f * dx, 0.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2(-1.0f * dx, 0.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 0.0f * dx, 0.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 1.0f * dx, 0.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 2.0f * dx, 0.0f * dy));

     // 一つ下
    ret += tex.Sample(smp, input.uv + float32_t2(-2.0f * dx, -1.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2(-1.0f * dx, -1.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 0.0f * dx, -1.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 1.0f * dx, -1.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 2.0f * dx, -1.0f * dy));
    
     // 最下段
    ret += tex.Sample(smp, input.uv + float32_t2(-2.0f * dx, -2.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2(-1.0f * dx, -2.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 0.0f * dx, -2.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 1.0f * dx, -2.0f * dy));
    ret += tex.Sample(smp, input.uv + float32_t2( 2.0f * dx, -2.0f * dy));

    return ret / 25.0f;
}