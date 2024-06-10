#include "Post.hlsli"

static const float32_t PI = 3.14159265f;


float32_t gauss(float32_t x, float32_t y, float32_t sigma){
    float32_t exponent = -(x * x + y* y) * rcp(2.0f * sigma * sigma);
    float32_t denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

static const float32_t2 kIndex[3][3] = {
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f,  0.0f }, { 0.0f,  0.0f }, { 1.0f,  0.0f } },
    { { -1.0f,  1.0f }, { 0.0f,  1.0f }, { 1.0f,  1.0f } },
};

float32_t4 main(Output input) : SV_TARGET{
    float32_t weight = 0.0f;
    float32_t kernel[3][3];


    for(uint32_t y = 0; y < 3; y++){
        for(uint32_t x = 0; x < 3; x++){
            kernel[y][x] gauss(kIndex[y][x].x, kIndex[y][x].y, 2.0f);
        }
    }

    float32_t4 result = float32_t4(0.0f,0.0f,0.0f,0.0f);

    for(uint32_t y = 0; y < 3; y++){
        for(uint32_t x = 0; x < 3; x++){
            result += tex.Sample(smp, input.uv + kIndex[y][x]) * kernel[y][x];
        }
    }

    return result * rcp(5.0f * 5.0f);
}