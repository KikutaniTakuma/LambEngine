#include "../Lamb.hlsli"

struct WaterData {
    float32_t2 randomVec;
    float32_t3 normal;
    float32_t3 tangent;
    uint32_t textureID;
    float32_t density;
};

StructuredBuffer<WaterData> kWaterData : register(t2);

struct WaterTex2DVertexOutPut{
    VertexShaderOutput outputData;
    float32_t3x3 tangentBasis : TANGENT_BASIS;
    float32_t2 causticsUv : TEXCOORD1;
};