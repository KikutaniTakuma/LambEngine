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
	uint32_t vertexCount;
	uint32_t vertexOffset;
	uint32_t primitiveCount;
	uint32_t primitiveOffset;
};

struct TransformParam {
	float32_t4x4 world;
	float32_t4x4 viewProjection;
};

ConstantBuffer<TransformParam> gTransform : register(b0);
StructuredBuffer<MSInput> gVertices : register(t0);
StructuredBuffer<uint32_t> gUniqueVertexIndices : register(t1);
StructuredBuffer<uint32_t> gPrimitiveIndices : register(t2);
StructuredBuffer<Meshlet> gMeshlets : register(t3);

uint32_t3 UnpackPrimitive(uint32_t primitive)
{
    // Unpacks a 10 bits per index triangle from a 32-bit uint.
    return uint32_t3(primitive & 0x3ff, (primitive >> 10) & 0x3ff, (primitive >> 20) & 0x3ff);
}

uint32_t3 GetPrimitive(Meshlet m, uint32_t index)
{
    return UnpackPrimitive(gPrimitiveIndices[m.primitiveOffset + index]);
}

uint32_t GetVertexIndex(Meshlet m, uint32_t localIndex)
{
    localIndex = m.vertexOffset + localIndex;

	return gUniqueVertexIndices[localIndex];
}

[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main(
	uint32_t groupThreadID : SV_GroupThreadID,
    uint32_t groupID : SV_GroupID,
    out vertices MSOutput verts[64],
	out indices uint32_t3 tris[126]
){
	Meshlet meshlet = gMeshlets[groupID];

	// スレッドグループの頂点数とプリミティブ数
	// mainメソッド内で一回だけ呼び出し可能
	SetMeshOutputCounts(meshlet.vertexCount, meshlet.primitiveCount);

	if(groupThreadID < meshlet.primitiveCount){
		tris[groupThreadID] = GetPrimitive(meshlet, groupThreadID);
	}

	if(groupThreadID < meshlet.vertexCount) {
		uint32_t index = GetVertexIndex(meshlet, groupThreadID);
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