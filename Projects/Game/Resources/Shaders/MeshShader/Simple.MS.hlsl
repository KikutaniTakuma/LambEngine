struct MSInput {
	float32_t4 position;
	float32_t3 normal;
	float32_t2 uv;
	uint32_t textureID;
	float32_t2 pad;
};

struct MSOutput {
	float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD0;
	uint32_t textureID : BLENDINDICES0;
};

struct Meshlet {
	uint32_t vertexOffset;
	uint32_t vertexCount;
	uint32_t primitiveOffset;
	uint32_t primitiveCount;
};

struct TransformParam {
	float32_t4x4 world;
	float32_t4x4 viewProjection;
};

ConstantBuffer<TransformParam> gTransform : register(b0);
StructuredBuffer<MSInput> gVertices : register(t0);
StructuredBuffer<uint32_t> gIndices : register(t1);
StructuredBuffer<uint32_t> gPrimitiveIndices : register(t2);
StructuredBuffer<Meshlet> gMeshlets : register(t3);

uint32_t3 PackedPrimitiveIndex(uint32_t packedIndex){
	return uint32_t3(
		(packedIndex >> 16) & 0xff,
		(packedIndex >> 8) & 0xff,
		(packedIndex >> 0) & 0xff
	);
}

[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main(
	uint32_t groupThreadID : SV_GroupThreadID,
	uint32_t groupID : SV_GroupID,
	out vertices MSOutput verts[64],
	out indices uint32_t3 polys[128]
){
	Meshlet meshlet = gMeshlets[groupID];

	// スレッドグループの頂点数とプリミティブ数
	// mainメソッド内で一回だけ呼び出し可能
	SetMeshOutputCounts(meshlet.vertexCount, meshlet.primitiveCount);

	if(groupThreadID < meshlet.primitiveCount){
		uint32_t primitiveIndicesIndex = meshlet.primitiveOffset + groupThreadID;
		uint32_t packedPrimitiveIndex = gPrimitiveIndices[primitiveIndicesIndex];
		polys[groupThreadID] = PackedPrimitiveIndex(packedPrimitiveIndex);
	}

	if(groupThreadID < meshlet.vertexCount) {
		uint32_t index = gIndices[meshlet.vertexOffset + groupThreadID];
		MSInput input = gVertices[index];
		MSOutput output = (MSOutput)0;

		float32_t4 localPos = input.position;
		float32_t4 worldPos = mul(localPos, gTransform.world);
		float32_t4 projPos = mul(worldPos, gTransform.viewProjection);

		output.position = projPos;
		output.worldPosition = worldPos;

		output.normal = input.normal;
		output.uv = input.uv;
		output.textureID = input.textureID;

		verts[groupThreadID] = output;
	}
}