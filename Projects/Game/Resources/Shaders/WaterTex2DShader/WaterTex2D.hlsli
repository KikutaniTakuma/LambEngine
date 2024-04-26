#include "../Lamb.hlsli"

struct WaterData {
    float32_t2 randomVec;
    float32_t3 pad; // <- huh?
    uint32_t textureID;
};

StructuredBuffer<WaterData> kWaterData : register(t2);

struct WaterTex2DVertexOutPut{
    VertexShaderOutput outputData;
    
}