struct MSInput {
	float32_t4 position;
	float32_t4 color;
};

struct MSOutput {
	float32_t4 position : SV_POSITION;
	float32_t4 color : COLOR;
};

struct TransformParam {
	float32_t4x4 world;
	float32_t4x4 viewProjection;
};

struct IndexParam {
	uint32_t3 index;
};

ConstantBuffer<TransformParam> gTransform : register(b0);
StructuredBuffer<MSInput> gVertices : register(t0);
StructuredBuffer<IndexParam> gIndices : register(t1);

[numthreads(64, 1, 1)]
[outputtopology("triangle")]
void main(
	uint32_t groupIndex : SV_GroupIndex,
	out vertices MSOutput verts[3],
	out indices uint32_t3 tris[1]
){
	// スレッドグループの頂点数とプリミティブ数
	// mainメソッド内で一回だけ呼び出し可能
	SetMeshOutputCounts(3, 1);

	if(groupIndex < 1){
		tris[groupIndex] = gIndices[groupIndex].index;
	}

	if(groupIndex < 3) {
		MSOutput output = (MSOutput)0;

		float32_t4 localPos = gVertices[groupIndex].position;
		float32_t4 worldPos = mul(localPos, gTransform.world);
		float32_t4 projectionPos = mul(worldPos, gTransform.viewProjection);

		output.position = projectionPos;
		output.color = gVertices[groupIndex].color;

		verts[groupIndex] = output;
	}
}