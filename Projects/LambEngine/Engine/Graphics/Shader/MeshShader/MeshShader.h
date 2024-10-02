#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#undef max
#undef min
#include "Engine/Buffer/ConstantBuffer.h" 
#include "Engine/Buffer/StructuredBuffer.h"
#include "Math/Vector4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Mat4x4.h"

#include "../../GraphicsStructs.h"

#include "DirectXMesh/DirectXMesh.h"


// MeshShaderのパイプラインを作るためのクラス
template<typename ValueType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE kObjectType>
class alignas(void*) StateParam {
public:
	StateParam() :
		type_(kObjectType),
		value_(ValueType()) 
	{}

	StateParam(const ValueType& value) :
		type_(kObjectType),
		value_(value)
	{}

	~StateParam() = default;

	StateParam& operator=(const ValueType& value) {
		type_ = kObjectType;
		value_ = value;

		return *this;
	}


private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_;
	ValueType                           value_;
};

// MeshShaderのパイプラインを作るためのクラス(パディングあり)
template<typename ValueType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE kObjectType>
class alignas(void*) StateParamHasPad {
public:
	StateParamHasPad() :
		type_(kObjectType),
		value_(ValueType()),
		pad({})
	{}

	StateParamHasPad(const ValueType& value) :
		type_(kObjectType),
		value_(value),
		pad({})
	{}

	~StateParamHasPad() = default;

	StateParamHasPad& operator=(const ValueType& value) {
		type_ = kObjectType;
		value_ = value;

		return *this;
	}


private:
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_;
	ValueType                           value_;
	int8_t                              pad[4]; // 4バイトパディング
};




using SP_ROOT_SIGNATURE = StateParam<       ID3D12RootSignature*,        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE       >;
using SP_AS             = StateParam<       D3D12_SHADER_BYTECODE,       D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS                   >;
using SP_MS             = StateParam<       D3D12_SHADER_BYTECODE,       D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS                   >;
using SP_PS             = StateParam<       D3D12_SHADER_BYTECODE,       D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS                   >;
using SP_BLEND          = StateParamHasPad< D3D12_BLEND_DESC,            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND                >;
using SP_RASTERIZER     = StateParam<       D3D12_RASTERIZER_DESC,       D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER           >;
using SP_DEPTH_STENCIL  = StateParam<       D3D12_DEPTH_STENCIL_DESC,    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL        >;
using SP_SAMPLE_MASK    = StateParam<       UINT,                        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK          >;
using SP_SAMPLE_DESC    = StateParamHasPad< DXGI_SAMPLE_DESC,            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC          >;
using SP_RT_FORMAT      = StateParam<       D3D12_RT_FORMAT_ARRAY,       D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS>;
using SP_DS_FORMAT      = StateParam<       DXGI_FORMAT,                 D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT >;
using SP_FLAGS          = StateParam<       D3D12_PIPELINE_STATE_FLAGS,  D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS                >;


// MeshShader用のパイプラインステート
struct MeshShaderPipelineStateDesc {
	SP_ROOT_SIGNATURE rootSignature;
	SP_AS             AS;
	SP_MS             MS;
	SP_PS             PS;
	SP_BLEND          blend;
	SP_RASTERIZER     rasterizer;
	SP_DEPTH_STENCIL  depthStencil;
	SP_SAMPLE_MASK    sampleMask;
	SP_SAMPLE_DESC    sampleDesc;
	SP_RT_FORMAT      RTFormat;
	SP_DS_FORMAT      DSFormt;
	SP_FLAGS          flags;
};

// Meshletを保存した構造体
struct ResMesh {
	std::vector<Vertex>           vertices; // 頂点データ
	std::vector<uint32_t>         indices;  // インデックスデータ
	
	std::vector<DirectX::Meshlet> meshlets;            // メッシュレット
	std::vector<uint32_t>         uniqueVertexIndices; // インデックスデータ
	std::vector<uint32_t>         primitiveIndices;    // プリミティブインデックス
};

// MeshShaderで使うStructuredBufferをまとめた構造体
struct MeshShaderData {
	StructuredBuffer<Vertex>           gVertices;            // 頂点データ
	StructuredBuffer<uint32_t>         gUniqueVertexIndices; // インデックスデータ
	StructuredBuffer<uint32_t>         gPrimitiveIndices;    // プリミティブインデックス
	StructuredBuffer<DirectX::Meshlet> gMeshlets;            // メッシュレット
	StructuredBuffer<WVPMatrix>        gTransform;           // トランスフォーム

	uint32_t meshletCount;                                   // メッシュレットの数
};