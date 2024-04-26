#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "Math/Vector4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Mat4x4.h"
#include "Math/Quaternion.h"

#include "PipelineManager/Pipeline/Pipeline.h"
#include "../EngineUtils/LambPtr/LambPtr.h"
#include "../Buffer/StructuredBuffer/StructuredBuffer.h"
#include "../Buffer/ConstBuffer/ConstBuffer.h"


#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#undef max
#undef min

struct Node {
    Mat4x4 loacalMatrix = Mat4x4::kIdentity;
    std::string name;
    std::vector<Node> children;
};

//struct ModelData {
//    std::vector<Vertex> vertices;
//    Node rootNode;
//};

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
    Vector3 ligDirection = -Vector3::kYIdentity;
    float pad0;
    Vector3 ligColor = Vector3::kIdentity;
    /*Vector3 eyePos;
    float pad2;
    Vector3 ptPos;
    float pad3;
    Vector3 ptColor;
    float ptRange;*/
};
//
//template<class T, uint32_t bufferSize>
//struct ShaderData {
//    ConstBuffer<Light> light;
//    StructuredBuffer<WVPMatrix, bufferSize> wvpMatrix;
//    StructuredBuffer<Vector4, bufferSize> color;
//    StructuredBuffer<T, bufferSize> shaderStruct;
//};

//struct ShaderFileNames {
//    std::string vsFileName;
//    std::string psFileName;
//    std::string gsFileName;
//    std::string dsFileName;
//    std::string hsFileName;
//
//    [[nodiscard]] bool operator==(const ShaderFileNames& right) const {
//        return vsFileName == right.vsFileName &&
//            psFileName == right.psFileName &&
//            gsFileName == right.gsFileName &&
//            dsFileName == right.dsFileName &&
//            hsFileName == right.hsFileName;
//    }
//};
//
//struct LoadFileNames {
//    std::string resourceFileName;
//    ShaderFileNames shaderName;
//
//    [[nodiscard]] bool operator==(const LoadFileNames& right) const {
//        return resourceFileName == right.resourceFileName &&
//            shaderName == right.shaderName;
//    }
//};

//namespace std {
//    template<>
//    struct hash<Vertex> {
//    public:
//        size_t operator()(const Vertex& data)const {
//            size_t result{};
//
//            result = std::hash<std::string>{}(
//                std::to_string(data.pos.m[0]) + 
//                std::to_string(data.pos.m[1]) + 
//                std::to_string(data.pos.m[2]) + 
//                std::to_string(data.pos.m[3]) + 
//                std::to_string(data.normal.x) + 
//                std::to_string(data.normal.y) + 
//                std::to_string(data.normal.z) + 
//                std::to_string(data.uv.x) + 
//                std::to_string(data.uv.y) + 
//                std::to_string(data.texIndex) 
//                );
//
//            return result;
//        }
//
//    };
//}

//namespace std {
//    template<>
//    struct hash<LoadFileNames> {
//    public:
//        size_t operator()(const LoadFileNames& data)const {
//            size_t result{};
//
//            result = std::hash<std::string>{}(
//                data.resourceFileName +
//                data.shaderName.vsFileName +
//                data.shaderName.psFileName +
//                data.shaderName.gsFileName +
//                data.shaderName.dsFileName +
//                data.shaderName.hsFileName
//                );
//
//            return result;
//        }
//
//    };
//}


template<class tValue>
struct KeyFrame{
    float time;
    tValue value;
};


using KeyFrameVector3 = KeyFrame<Vector3>;
using KeyFrameQuaternion = KeyFrame<Quaternion>;

template<class tValue>
struct AnimationCurve {
    std::vector<KeyFrame<tValue>> keyFrames;
};

struct NodeAnimation{
    AnimationCurve<Vector3> translation;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> sacle;
};

struct Animation {
    float duration;
    std::unordered_map<std::string, NodeAnimation> nodeAnimations;
};

struct Animations {
    std::vector<Animation> data;
};