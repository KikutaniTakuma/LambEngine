#include "Post.hlsli"

static const float32_t PI = 3.14159265f;

struct GaussianBlurState{
    float32_t2 dir;
    float32_t sigma;
    int32_t kernelSize;
};

ConstantBuffer<GaussianBlurState> kGaussianBlurState : register(b1);


float32_t4 main(Output input) : SV_TARGET{
    float32_t w, h, levels;
    tex.GetDimensions(0, w,h,levels);
    float32_t2 texelSize = float32_t2(rcp(w), rcp(h));
    float32_t4 result = float32_t4(0.0f,0.0f,0.0f,0.0f);
    result = GaussianBlur(
        texelSize, 
        input.uv, 
        kGaussianBlurState.kernelSize, 
        kGaussianBlurState.sigma, 
        kGaussianBlurState.dir
        );
    return result;
}