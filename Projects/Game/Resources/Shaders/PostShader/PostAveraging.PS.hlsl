#include "Post.hlsli"

static const float32_t2 kIndex[5][5] = {
    { { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f }, { 2.0f, -2.0f } },
    { { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, { 2.0f, -1.0f } },
    { { -2.0f,  0.0f }, { -1.0f,  0.0f }, { 0.0f,  0.0f }, { 1.0f,  0.0f }, { 2.0f,  0.0f } },
    { { -2.0f,  1.0f }, { -1.0f,  1.0f }, { 0.0f,  1.0f }, { 1.0f,  1.0f }, { 2.0f,  1.0f } },
    { { -2.0f,  2.0f }, { -1.0f,  2.0f }, { 0.0f,  2.0f }, { 1.0f,  2.0f }, { 2.0f,  2.0f } },
};

static const float32_t kKernel[5][5] = {
    { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
};

float32_t4 main(Output input) : SV_TARGET{
    float32_t w, h, levels;
    tex.GetDimensions(0, w,h,levels);

    float32_t dx = rcp(w);
    float32_t dy = rcp(h);
    float32_t4 ret = float32_t4(0.0f,0.0f,0.0f,0.0f);

    for(uint32_t y = 0; y < 5; y++){
        for(uint32_t x = 0; x < 5; x++){
            ret += tex.Sample(smp, input.uv + kIndex[y][x]) * kKernel[y][x];
        }
    }

    return ret * rcp(5.0f * 5.0f);
}