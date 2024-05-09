#include "Model.hlsli"

struct Well{
	float32_t4x4 skeletonSpaceMatrix;
	float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

StructuredBuffer<Well> kMatrixPalette : register(t4);

struct AnimationVertexInput{
	float32_t4 position : POSITION0;
	float32_t3 normal : NORMAL0;
	float32_t2 uv : TEXCOORD;
	uint32_t textureID : BLENDINDICES;
	float32_t4 weight : WEIGHT0;
	int32_t4 index : INDEX0;
};

struct Skinned {
	float32_t4 position;
	float32_t3 normal;
};

Skinned Skinning(AnimationVertexInput input){
	Skinned result;
	result.position = mul(input.position, kMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
	result.position += mul(input.position, kMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
	result.position += mul(input.position, kMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
	result.position += mul(input.position, kMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
	result.position.w = 1.0f;

	result.normal = mul(input.normal, (float32_t3x3)kMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
	result.normal += mul(input.normal, (float32_t3x3)kMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
	result.normal += mul(input.normal, (float32_t3x3)kMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
	result.normal += mul(input.normal, (float32_t3x3)kMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
	result.normal = normalize(result.normal);

	return result;
}


VertexShaderOutput main(AnimationVertexInput input,uint32_t instanceID : SV_InstanceID)
{
	VertexShaderOutput output;
	Skinned skinned = Skinning(input);

	skinned.position = mul(skinned.position, kWvpMat[instanceID].worldMat);
	output.worldPosition = skinned.position;
	output.position = mul(skinned.position, kWvpMat[instanceID].cameraMat);
	output.normal = mul(skinned.normal, (float32_t3x3)kWvpMat[instanceID].worldMat);
	output.uv = input.uv;
	output.instanceID = instanceID;
	output.textureID = input.textureID;

	return output;
}