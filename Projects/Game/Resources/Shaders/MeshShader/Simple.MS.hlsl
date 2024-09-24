struct MSInput {
	float32_t3 position;
	float32_t4 color;
};

struct MSOutput {
	float32_t4 position : SV_POSITION;
	float32_t4 color : COLOR;
};

struct TransformParam {
	float32_t4x4 world;
	float32_t4x4 view;
	float32_t4x4 projection;
};

struct IndexParam {
	uint32_t3 index;
};

StructuredBuffer<MSInput> gVertices : register(t0);
StructuredBuffer<IndexParam> gIndices : register(t1);
ConstantBuffer<TransformParam> gTransform : register(b0);

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

		float32_t4 localPos = float32_t4(gVertices[groupIndex].position, 1.0f);
		float32_t4 worldPos = mul(localPos, gTransform.world);
		float32_t4 viewPos = mul(worldPos, gTransform.view);
		float32_t4 projectionPos = mul(viewPos, gTransform.projection);

		output.position = projectionPos;
		output.color = gVertices[groupIndex].color;

		verts[groupIndex] = output;
	}
}