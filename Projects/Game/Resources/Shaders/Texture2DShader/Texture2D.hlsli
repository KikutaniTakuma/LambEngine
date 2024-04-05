#include "../Lamb.hlsli"

struct Texture2DData {
    float32_t4x4 uvTransform;
    uint32_t textureID;
};

StructuredBuffer<Texture2DData> kTexture2DData : register(t2);