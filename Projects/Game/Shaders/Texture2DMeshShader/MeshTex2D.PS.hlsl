#include "../LambMesh.hlsli"

struct Texture2DData {
    float32_t4x4 uvTransform;
    float32_t3 pad; // <- huh?
    uint32_t textureID;
};

StructuredBuffer<Texture2DData> kTexture2DData : register(t6);

PixelShaderOutPut3 main(MSOutput input)
{
	PixelShaderOutPut3 output;

    uint32_t textureID = kTexture2DData[input.instanceID].textureID;
	
	float32_t4 textureColor = textures[textureID].Sample(smp, input.uv);
    if(textureColor.w < 0.1f){
        discard;
    }

	output.color0 = textureColor * gColor[input.instanceID].color;

    // 法線
    output.color1.xyz = input.normal;
    output.color1.w = 1.0f;

    // ポジション
    output.color2 = input.worldPosition;
    
	return output;
}