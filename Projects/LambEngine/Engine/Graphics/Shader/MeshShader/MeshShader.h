#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#undef max
#undef min


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




#ifndef PSST
#define PSST(x)  D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_##x
#endif // !PSST

using SP_ROOT_SIGNATURE = StateParam<ID3D12RootSignature*,       PSST(ROOT_SIGNATURE)>;
using SP_AS             = StateParam<D3D12_SHADER_BYTECODE,      PSST(AS)>;
using SP_MS             = StateParam<D3D12_SHADER_BYTECODE,      PSST(MS)>;
using SP_PS             = StateParam<D3D12_SHADER_BYTECODE,      PSST(PS)>;
using SP_BLEND          = StateParam<D3D12_BLEND_DESC,           PSST(BLEND)>;
using SP_RASTERIZER     = StateParam<D3D12_RASTERIZER_DESC,      PSST(RASTERIZER)>;
using SP_DEPTH_STENCIL  = StateParam<D3D12_DEPTH_STENCIL_DESC,   PSST(DEPTH_STENCIL)>;
using SP_SAMPLE_MASK    = StateParam<UINT,                       PSST(SAMPLE_MASK)>;
using SP_SAMPLE_DESC    = StateParam<DXGI_SAMPLE_DESC,           PSST(SAMPLE_DESC)>;
using SP_RT_FORMAT      = StateParam<D3D12_RT_FORMAT_ARRAY,      PSST(RENDER_TARGET_FORMATS)>;
using SP_DS_FORMAT      = StateParam<DXGI_FORMAT,                PSST(DEPTH_STENCIL_FORMAT)>;
using SP_FLAGS          = StateParam<D3D12_PIPELINE_STATE_FLAGS, PSST(FLAGS)>;

#ifdef PSST
#undef PSST
#endif // PSST



struct MeshShaderPipelineStateDesc {
	SP_ROOT_SIGNATURE RootSignature;
	SP_AS             AS;
	SP_MS             MS;
	SP_PS             PS;
	SP_BLEND          Blend;
	SP_RASTERIZER     Rasterizer;
	SP_DEPTH_STENCIL  DepthStencil;
	SP_SAMPLE_MASK    SampleMask;
	SP_SAMPLE_DESC    SampleDesc;
	SP_RT_FORMAT      RTFormat;
	SP_DS_FORMAT      DSFormt;
	SP_FLAGS          Flags;
};