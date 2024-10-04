Texture2D<float32_t4> tex : register(t0);
Texture2D<float32_t4> caustics : register(t1);
SamplerState smp : register(s0);

cbuffer Matrix : register(b0){
    float32_t4x4 worldmat;
    float32_t4x4 viewProjectionMatrix;
}

cbuffer Color : register(b1){
    float32_t4 color;
}

cbuffer NormalVector : register(b2)
{
    float32_t3 kNormal;
    float32_t3 kTangent;
}

cbuffer DensityScale :register(b5) {
    float32_t kDensityScale;
}

struct Output{
    float32_t4 svPos : SV_POSITION;
    float32_t2 uv : TEXCOORD0;
    float32_t2 causticsUv : TEXCOORD1;
    float32_t4 worldPos : POSITION;
    float32_t3x3 tangentBasis : TANGENT_BASIS;
};