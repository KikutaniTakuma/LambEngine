#include "Texture2D.hlsli"

PixelShaderOutPut3 main(VertexShaderOutput input)
{
	PixelShaderOutPut3 output;

    uint32_t textureID = kTexture2DData[input.instanceID].textureID;
	
	float32_t4 textureColor = textures[textureID].Sample(smp, input.uv);
    if(textureColor.w < 0.1f){
        discard;
    }

	output.color0 = textureColor * kColor[input.instanceID].color;

    // 法線
    output.color1.xyz = input.normal;
    output.color1.w = 1.0f;

    // ポジション
    output.color2 = input.worldPosition;
    
	return output;
}