struct WVPMatrix {
	float32_t4x4 worldMat;
	float32_t4x4 cameraMat;
};

struct DirectionLight {
	float32_t3 ligDirection;
	float32_t3 ligColor;
	float32_t3 eyePos;
	float32_t3 ptPos;
	float32_t3 ptColor;
	float32_t ptRange;
};

struct Color {
	float32_t4 color;
};

ConstantBuffer<DirectionLight> kLight : register(b0);
StructuredBuffer<WVPMatrix> kWvpMat : register(t0);
StructuredBuffer<Color> kColor : register(t1);
Texture2D<float32_t4> textures[] : register(t3);
SamplerState smp : register(s0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t3 normal : NORMAL0;
	float32_t2 uv : TEXCOORD;
	uint32_t textureID : BLENDINDICES;
};

struct VertexShaderOutput{
    float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD0;
	uint32_t textureID : BLENDINDICES0;
	uint32_t instanceID : BLENDINDICES1;
};

struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};
struct PixelShaderOutPut2 {
	float32_t4 color0 : SV_TARGET0;
	float32_t4 color1 : SV_TARGET1;
};
struct PixelShaderOutPut3 {
	float32_t4 color0 : SV_TARGET0;
	float32_t4 color1 : SV_TARGET1;
	float32_t4 color2 : SV_TARGET2;
};
struct PixelShaderOutPut4 {
	float32_t4 color0 : SV_TARGET0;
	float32_t4 color1 : SV_TARGET1;
	float32_t4 color2 : SV_TARGET2;
	float32_t4 color3 : SV_TARGET3;
};
struct PixelShaderOutPut5 {
	float32_t4 color0 : SV_TARGET0;
	float32_t4 color1 : SV_TARGET1;
	float32_t4 color2 : SV_TARGET2;
	float32_t4 color3 : SV_TARGET3;
	float32_t4 color4 : SV_TARGET4;
};
struct PixelShaderOutPut6 {
	float32_t4 color0 : SV_TARGET0;
	float32_t4 color1 : SV_TARGET1;
	float32_t4 color2 : SV_TARGET2;
	float32_t4 color3 : SV_TARGET3;
	float32_t4 color4 : SV_TARGET4;
	float32_t4 color5 : SV_TARGET5;
};
struct PixelShaderOutPut7 {
	float32_t4 color0 : SV_TARGET0;
	float32_t4 color1 : SV_TARGET1;
	float32_t4 color2 : SV_TARGET2;
	float32_t4 color3 : SV_TARGET3;
	float32_t4 color4 : SV_TARGET4;
	float32_t4 color5 : SV_TARGET5;
	float32_t4 color6 : SV_TARGET6;
};
struct PixelShaderOutPut8 {
	float32_t4 color0 : SV_TARGET0;
	float32_t4 color1 : SV_TARGET1;
	float32_t4 color2 : SV_TARGET2;
	float32_t4 color3 : SV_TARGET3;
	float32_t4 color4 : SV_TARGET4;
	float32_t4 color5 : SV_TARGET5;
	float32_t4 color6 : SV_TARGET6;
	float32_t4 color7 : SV_TARGET7;
};


/// Please define the following in each Shader ///

/// Please define the structure used in each Shader
/// example : 
///	struct IsLighting {
///		uint32_t isLighting;
///	};

/// After defining the structure used in each shader, 
/// define StructuredBuffer and specify register(t2).
/// example : 
/// StructuredBuffer<IsLighting> kIsLighting : register(t2);