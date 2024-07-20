#include "Model.hlsli"

PixelShaderOutPut3 main(VertexShaderOutput input)
{
	PixelShaderOutPut3 output;
	
	float32_t4 textureColor = textures[input.textureID].Sample(smp, input.uv);


	output.color0 = textureColor;

	if(kIsLighting[input.instanceID].isLighting == 1) {
	// ディレクションライト拡散反射光
		float t = dot(input.normal, kLight.ligDirection);

		t *= -1.0f;
		t = (t + abs(t)) * 0.5f;

		float3 diffDirection = kLight.ligColor * t;

		float3 lig = diffDirection;
		lig.x += 0.2f;
		lig.y += 0.2f;
		lig.z += 0.2f;

		output.color0.xyz *= lig;
	}
	output.color0 *= kColor[input.instanceID].color;

    // 法線
    output.color1.xyz = input.normal;
	output.color1.w = 1.0f;

    // ポジション
    output.color2 = input.worldPosition;
	return output;
}