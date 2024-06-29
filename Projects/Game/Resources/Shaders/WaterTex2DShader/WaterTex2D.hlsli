#include "../Lamb.hlsli"

struct WaterData {
    float32_t2 randomVec;
    float32_t3 normal;
    float32_t3 tangent;
    uint32_t textureID;
    float32_t density;
    uint32_t edgeDivision;
    uint32_t insideDivision;
};

StructuredBuffer<WaterData> kWaterData : register(t2);

struct WaterTex2DVertexOutPut{
    VertexShaderOutputToHull outputData;
    float32_t3x3 tangentBasis : TANGENT_BASIS;
    float32_t2 causticsUv : TEXCOORD1;
};

struct WaterTex2DHullShaderOutPut{
    HullShaderOutPut outputData;
    float32_t3x3 tangentBasis : TANGENT_BASIS;
    float32_t2 causticsUv : TEXCOORD1;
};

struct WaterTex2DDomainShaderOutPut{
    DomainShaderOutPutToGeometory outputData;
    float32_t3x3 tangentBasis : TANGENT_BASIS;
    float32_t2 causticsUv : TEXCOORD1;
};

struct WaterTex2DGeometoryShaderOutPut{
    GeometoryOutPut outputData;
    float32_t3x3 tangentBasis : TANGENT_BASIS;
    float32_t2 causticsUv : TEXCOORD1;
};