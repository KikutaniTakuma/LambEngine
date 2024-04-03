#pragma once

#include <unordered_map>

#include "Math/Vector4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Mat4x4.h"

#include "PipelineManager/Pipeline/Pipeline.h"
#include "../EngineUtils/LambPtr/LambPtr.h"
#include "../Buffer/StructuredBuffer/StructuredBuffer.h"
#include "../Buffer/ConstBuffer/ConstBuffer.h"


#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#undef max
#undef min



struct Vertex {
    // ポジション
    Vector4 pos;
    // 法線
    Vector3 normal;

    // パディング
    float pad0;

    // uv座標
    Vector2 uv;

    // テクスチャのインデックス
    uint32_t texIndex;
};

struct WVPMatrix {
    Mat4x4 worldMat;
    Mat4x4 cameraMat;
};

struct Mesh {
    D3D12_VERTEX_BUFFER_VIEW vertexView;
    Lamb::LambPtr<ID3D12Resource> vertexResource;
    uint32_t vertexNumber;

    D3D12_INDEX_BUFFER_VIEW indexView;
    Lamb::LambPtr<ID3D12Resource> indexResource;

    uint32_t indexNumber;
};

enum BlendType {
    kNone = Pipeline::Blend::None,
    
    kNormal = Pipeline::Blend::Normal,
    kAdd = Pipeline::Blend::Add,
    kSub = Pipeline::Blend::Sub,
    kMul = Pipeline::Blend::Mul,

    kUnenableDepthNone = (Pipeline::Blend::BlendTypeNum),
    kUnenableDepthNormal,
    kUnenableDepthAdd,
    kUnenableDepthSub,
    kUnenableDepthMul,

    kNum
};

struct Light {
    Vector3 ligDirection;
    float pad0;
    Vector3 ligColor;
    float pad1;
    Vector3 eyePos;
    float pad2;
    Vector3 ptPos;
    float pad3;
    Vector3 ptColor;
    float ptRange;
};

template<class T, uint32_t bufferSize>
struct ShaderData {
    ConstBuffer<Light> light;
    StructuredBuffer<WVPMatrix, bufferSize> wvpMatrix;
    StructuredBuffer<Vector4, bufferSize> color;
    StructuredBuffer<T, bufferSize> shaderStruct;
};

struct ShaderFileNames {
    std::string vsFileName;
    std::string psFileName;
    std::string gsFileName;
    std::string dsFileName;
    std::string hsFileName;

    [[nodiscard]] bool operator==(const ShaderFileNames& right) const {
        return vsFileName == right.vsFileName &&
            psFileName == right.psFileName &&
            gsFileName == right.gsFileName &&
            dsFileName == right.dsFileName &&
            hsFileName == right.hsFileName;
    }
};

struct LoadFileNames {
    std::string reourceFileName;
    ShaderFileNames shaderName;

    [[nodiscard]] bool operator==(const LoadFileNames& right) const {
        return reourceFileName == right.reourceFileName &&
            shaderName == right.shaderName;
    }
};