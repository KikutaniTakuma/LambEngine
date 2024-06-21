#pragma once
#include "../../GraphicsStructs.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

#include "Engine/Graphics/Skeleton.h"

#include <typeinfo>
#include <concepts>
#include <array>
#include <memory>


class BaseRenderContext {
public:
    static constexpr uint32_t kMaxDrawInstance = 256u;


public:
    BaseRenderContext():
        pipeline_(nullptr),
        drawCount_(0),
        typeID_()
    {}
    virtual ~BaseRenderContext() = default;

    void UsePipeline() {
        pipeline_->Use();
    }

    const std::string& GetID() const {
        return typeID_;
    }

    virtual void SetPipeline(Pipeline* const pipeline) {
        pipeline_ = pipeline;
    }
    virtual void SetWVPMatrix(const WVPMatrix& matrix) = 0;
    virtual void SetColor(const Vector4& color) = 0;

public:
    void AddStartInstanceLocation(uint32_t& startInstanceLocation) const {
        startInstanceLocation += drawCount_;
    }

    void AddDrawCount() {
        drawCount_++;
        (*allDrawCount_)++;
    }

    void ResetDrawCount() {
        drawCount_ = 0u;
    }

    uint32_t GetDrawCount() const {
        return drawCount_;
    }

    bool IsDraw() const {
        return drawCount_ != 0u;
    }

    void SetAllDrawCount(uint32_t* allDrawCount) {
        allDrawCount_ = allDrawCount;
    }

    void SetMesh(const Mesh* mesh) {
        mesh_ = mesh;
    }


protected:
    Pipeline* pipeline_;
    uint32_t drawCount_;

    std::string typeID_;

    Lamb::SafePtr<uint32_t> allDrawCount_ = nullptr;
    Lamb::SafePtr<const Mesh> mesh_;
};

using RenderData = BaseRenderContext;


template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
class RenderContext : public BaseRenderContext {
public:
    using type = T;
    static constexpr const size_t size = bufferSize;

public:
    RenderContext():
        shaderData_()
    {
        pipeline_ = nullptr;
        drawCount_ = 0u;

        typeID_ = (typeid(RenderContext<T, bufferSize>).name());

        shaderData_.wvpMatrix.resize(bufferSize);
        shaderData_.color.resize(bufferSize);
        shaderData_.shaderStruct.resize(bufferSize);
    }
    ~RenderContext() = default;

    RenderContext(const RenderContext&) = delete;
    RenderContext(RenderContext&&) = delete;

    RenderContext& operator=(const RenderContext&) = delete;
    RenderContext& operator=(RenderContext&&) = delete;
    
public:
    inline void SetWVPMatrix(const WVPMatrix& matrix) override {
        if (bufferSize <= *allDrawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        shaderData_.wvpMatrix[drawCount_].worldMat = mesh_->node.loacalMatrix * matrix.worldMat;
        shaderData_.wvpMatrix[drawCount_].cameraMat = matrix.cameraMat;
    }
    inline void SetColor(const Vector4& color) override {
        if (bufferSize <= *allDrawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        shaderData_.color[drawCount_] = color;
    }
    inline void SetShaderStruct(const T& data) {
        if (bufferSize <= *allDrawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }
        shaderData_.shaderStruct[drawCount_] = data;
    }

public:
    const ShaderData<T>& GetShaderData() const {
        return shaderData_;
    }


private:
    ShaderData<T> shaderData_;
};

//template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
//class SkinRenderContext : public BaseRenderContext {
//public:
//    SkinRenderContext() :
//        shaderData_()
//    {
//        shaderData_.wvpMatrix.Create(bufferSize);
//        shaderData_.color.Create(bufferSize);
//        shaderData_.shaderStruct.Create(bufferSize);
//
//
//        pipeline_ = nullptr;
//        drawCount_ = 0u;
//
//        // ディスクリプタヒープ
//        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();
//
//        descriptorHeap->BookingHeapPos(4);
//        descriptorHeap->CreateView(shaderData_.light);
//        descriptorHeap->CreateView(shaderData_.wvpMatrix);
//        descriptorHeap->CreateView(shaderData_.color);
//        descriptorHeap->CreateView(shaderData_.shaderStruct);
//
//        typeID_ = (typeid(SkinRenderContext<T, bufferSize>).name());
//    }
//    ~SkinRenderContext() {
//        // ディスクリプタヒープ
//        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();
//
//        descriptorHeap->ReleaseView(shaderData_.light.GetHandleUINT());
//        descriptorHeap->ReleaseView(shaderData_.wvpMatrix.GetHandleUINT());
//        descriptorHeap->ReleaseView(shaderData_.color.GetHandleUINT());
//        descriptorHeap->ReleaseView(shaderData_.shaderStruct.GetHandleUINT());
//    }
//
//    SkinRenderContext(const SkinRenderContext&) = delete;
//    SkinRenderContext(SkinRenderContext&&) = delete;
//
//    SkinRenderContext& operator=(const SkinRenderContext&) = delete;
//    SkinRenderContext& operator=(SkinRenderContext&&) = delete;
//
//public:
//    void Draw() override {
//        // ディスクリプタヒープ
//        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();
//        // コマンドリスト
//        ID3D12GraphicsCommandList* const commandlist = DirectXCommand::GetMainCommandlist()->GetCommandList();
//
//        // パイプライン設定
//        pipeline_->Use();
//
//        // ライト構造体
//        commandlist->SetGraphicsRootDescriptorTable(0, shaderData_.light.GetHandleGPU());
//        // ワールドとカメラマトリックス, 色, 各シェーダーの構造体
//        commandlist->SetGraphicsRootDescriptorTable(1, shaderData_.wvpMatrix.GetHandleGPU());
//        // スキンアニメーション用
//        commandlist->SetGraphicsRootDescriptorTable(2, skinCluster_->paletteBuffer.GetHandleGPU());
//        // テクスチャ
//        commandlist->SetGraphicsRootDescriptorTable(3, descriptorHeap->GetGpuHeapHandle(0));
//
//        std::array vertexBuffer = {
//            mesh_->vertexView,
//            skinCluster_->infliuenceBufferView
//        };
//
//        // 頂点バッファセット
//        commandlist->IASetVertexBuffers(0, static_cast<uint32_t>(vertexBuffer.size()), vertexBuffer.data());
//        // インデックスバッファセット
//        commandlist->IASetIndexBuffer(&mesh_->indexView);
//        // ドローコール
//        commandlist->DrawIndexedInstanced(mesh_->indexNumber, drawCount_, 0, 0, 0);
//        //commandlist->DrawInstanced(mesh_->vertexNumber, drawCount_, 0, 0);
//    }
//
//public:
//    inline void SetMesh(const Mesh* const mesh) override {
//        if (!mesh) {
//            throw Lamb::Error::Code<SkinRenderContext>("mesh is nullptr", ErrorPlace);
//        }
//        mesh_ = mesh;
//    }
//    inline void SetModelData(const ModelData* const modelData) override {
//        if (!modelData) {
//            throw Lamb::Error::Code<SkinRenderContext>("modelData is nullptr", ErrorPlace);
//        }
//        modelData_ = modelData;
//    }
//    inline void SetPipeline(Pipeline* const pipeline) override {
//        if (!pipeline) {
//            throw Lamb::Error::Code<SkinRenderContext>("pipeline is nullptr", ErrorPlace);
//        }
//        pipeline_ = pipeline;
//    }
//    inline void SetSkinCluster(SkinCluster* skinCluster) {
//        if (not skinCluster) {
//            throw Lamb::Error::Code<SkinRenderContext>("skinCluster is nullptr", ErrorPlace);
//        }
//        skinCluster_ = skinCluster;
//    }
//    inline void SetWVPMatrix(const WVPMatrix& matrix) override {
//        if (bufferSize <= drawCount_) {
//            throw Lamb::Error::Code<SkinRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
//        }
//
//        shaderData_.wvpMatrix[drawCount_].worldMat = matrix.worldMat;
//        shaderData_.wvpMatrix[drawCount_].cameraMat = matrix.cameraMat;
//    }
//    inline void SetColor(const Vector4& color) override {
//        if (bufferSize <= drawCount_) {
//            throw Lamb::Error::Code<SkinRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
//        }
//
//        shaderData_.color[drawCount_] = color;
//    }
//    inline void SetLight(const Light& light) override {
//        *shaderData_.light = light;
//    }
//    inline void SetShaderStruct(const T& data) {
//        if (bufferSize <= drawCount_) {
//            throw Lamb::Error::Code<SkinRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
//        }
//        shaderData_.shaderStruct[drawCount_] = data;
//    }
//
//
//private:
//    ShaderDataBuffers<T> shaderData_;
//
//    SkinCluster* skinCluster_;
//};

template<class T>
concept IsBasedRenderContext =  std::is_base_of_v<BaseRenderContext, T>;

class BaseRenderSet {
public:
    BaseRenderSet() = default;
    virtual ~BaseRenderSet() = default;
    BaseRenderSet(const BaseRenderSet&) = delete;
    BaseRenderSet(BaseRenderSet&&) = delete;

    BaseRenderSet& operator=(const BaseRenderSet&) = delete;
    BaseRenderSet& operator=(BaseRenderSet&&) = delete;

public:
    virtual void Draw(BlendType blendType) = 0;
    virtual void OnceDraw(BlendType blendType) = 0;
    virtual void UploadShaderData() = 0;
    virtual void SetLight(const Light& light) = 0;

public:
    inline void Set(RenderData* const renderData, BlendType blend) {
        if (not renderData) {
            throw Lamb::Error::Code<BaseRenderSet>("renderData is nullptr", ErrorPlace);
        }

        renderDatas_[blend].reset(renderData);
        renderDatas_[blend]->SetAllDrawCount(&allDrawCount_);
        renderDatas_[blend]->SetMesh(mesh_);
    }



    inline void ResetDrawCount() {
        for (auto& i : renderDatas_) {
            i->ResetDrawCount();
        }
        startInstanceLocation_ = 0;
        allDrawCount_ = 0;
    }

public:
    inline RenderData* GetRenderContext(BlendType blend) {
        return renderDatas_[blend].get();
    }

    // ごり押し
    // やばい
    // マジでヤバイ
    // 使うときは気を付けて(一応型が違ったらエラーは出る)
    template<IsBasedRenderContext RenderContextType>
    inline RenderContextType* GetRenderContextDowncast(BlendType blend) {
        if (typeid(RenderContextType).name() != renderDatas_[blend]->GetID()) {
            throw Lamb::Error::Code<BaseRenderSet>("does not match class type", ErrorPlace);
        }

        return dynamic_cast<RenderContextType*>(renderDatas_[blend].get());
    }


public:
    void SetMesh(const Mesh* const mesh) {
        mesh_ = mesh;
    }
    void SetModelData(const ModelData* const modelData) {
        modelData_ = modelData;
    }


    const std::string& GetRootNodeName() const {
        return mesh_->node.name;
    }

    const Node& GetNode() const {
        return mesh_->node;
    }

    const Mesh* const GetMesh() const {
        return mesh_;
    }
    const ModelData* const GetModelData() const {
        return modelData_;
    }

protected:
    std::array<std::unique_ptr<RenderData>, BlendType::kNum> renderDatas_;

    const Mesh* mesh_ = nullptr;
    const ModelData* modelData_ = nullptr;

    uint32_t startInstanceLocation_ = 0;

    uint32_t allDrawCount_ = 0;
};

template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
class RenderSet : public BaseRenderSet {
public:
    RenderSet():
        BaseRenderSet(),
        shaderData_()
    {
        shaderData_.wvpMatrix.Create(bufferSize);
        shaderData_.color.Create(bufferSize);
        shaderData_.shaderStruct.Create(bufferSize);

        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->BookingHeapPos(4);
        descriptorHeap->CreateView(shaderData_.light);
        descriptorHeap->CreateView(shaderData_.wvpMatrix);
        descriptorHeap->CreateView(shaderData_.color);
        descriptorHeap->CreateView(shaderData_.shaderStruct);
    }
    ~RenderSet() {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->ReleaseView(shaderData_.light.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.wvpMatrix.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.color.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.shaderStruct.GetHandleUINT());
    }
    RenderSet(const RenderSet&) = delete;
    RenderSet(RenderSet&&) = delete;

    RenderSet& operator=(const RenderSet&) = delete;
    RenderSet& operator=(RenderSet&&) = delete;

public:
    void Draw(BlendType blendType) override {
        Lamb::SafePtr<RenderData> renderContext = this->GetRenderContext(blendType);

        if (not renderContext->IsDraw()) {
            return;
        }

        // ディスクリプタヒープ
        const Lamb::SafePtr descriptorHeap = CbvSrvUavHeap::GetInstance();
        // コマンドリスト
        const Lamb::SafePtr commandlist = DirectXCommand::GetMainCommandlist()->GetCommandList();


        // パイプラインセット
        renderContext->UsePipeline();

        // ライト構造体
        commandlist->SetGraphicsRootDescriptorTable(0, shaderData_.light.GetHandleGPU());
        // ワールドとカメラマトリックス, 色, 各シェーダーの構造体
        commandlist->SetGraphicsRootDescriptorTable(1, shaderData_.wvpMatrix.GetHandleGPU());
        // テクスチャ
        commandlist->SetGraphicsRootDescriptorTable(2, descriptorHeap->GetGpuHeapHandle(0));

        // 頂点バッファセット
        commandlist->IASetVertexBuffers(0, 1, &mesh_->vertexView);
        // インデックスバッファセット
        commandlist->IASetIndexBuffer(&mesh_->indexView);
        // ドローコール
        commandlist->DrawIndexedInstanced(mesh_->indexNumber, renderContext->GetDrawCount(), 0, 0, startInstanceLocation_);

        renderContext->AddStartInstanceLocation(startInstanceLocation_);
    }

    void OnceDraw(BlendType blendType) override {
        Lamb::SafePtr<RenderContext<T, bufferSize>> renderContext = this->GetRenderContextDowncast<RenderContext<T, bufferSize>>(blendType);

        const ShaderData<T>& shaderData = renderContext->GetShaderData();
        uint32_t dataIndex = renderContext->GetDrawCount() - 1;

        shaderData_.wvpMatrix[startInstanceLocation_] = shaderData.wvpMatrix[dataIndex];
        shaderData_.color[startInstanceLocation_] = shaderData.color[dataIndex];
        shaderData_.shaderStruct[startInstanceLocation_] = shaderData.shaderStruct[dataIndex];

        // ディスクリプタヒープ
        const Lamb::SafePtr descriptorHeap = CbvSrvUavHeap::GetInstance();
        // コマンドリスト
        const Lamb::SafePtr commandlist = DirectXCommand::GetMainCommandlist()->GetCommandList();


        // パイプラインセット
        renderContext->UsePipeline();

        // ライト構造体
        commandlist->SetGraphicsRootDescriptorTable(0, shaderData_.light.GetHandleGPU());
        // ワールドとカメラマトリックス, 色, 各シェーダーの構造体
        commandlist->SetGraphicsRootDescriptorTable(1, shaderData_.wvpMatrix.GetHandleGPU());
        // テクスチャ
        commandlist->SetGraphicsRootDescriptorTable(2, descriptorHeap->GetGpuHeapHandle(0));

        // 頂点バッファセット
        commandlist->IASetVertexBuffers(0, 1, &mesh_->vertexView);
        // インデックスバッファセット
        commandlist->IASetIndexBuffer(&mesh_->indexView);
        // ドローコール
        commandlist->DrawIndexedInstanced(mesh_->indexNumber, 1, 0, 0, startInstanceLocation_);

        startInstanceLocation_ += 1;
    }


    void UploadShaderData() override {
        uint32_t startInstanceLocation = 0;

        for (uint32_t i = 0; i < BlendType::kNum; i++) {
            RenderContext<T, bufferSize>* renderContext = this->GetRenderContextDowncast<RenderContext<T, bufferSize>>(BlendType(i));
            const ShaderData<T>& shaderData = renderContext->GetShaderData();

            for (uint32_t indexCount = 0; indexCount < renderContext->GetDrawCount(); indexCount++) {
                shaderData_.wvpMatrix[startInstanceLocation + indexCount] = shaderData.wvpMatrix[indexCount];
                shaderData_.color[startInstanceLocation + indexCount] = shaderData.color[indexCount];
                shaderData_.shaderStruct[startInstanceLocation + indexCount] = shaderData.shaderStruct[indexCount];
            }

            startInstanceLocation += renderContext->GetDrawCount();
        }
    }

    inline void SetLight(const Light& light) override {
        *shaderData_.light = light;
    }

private:
    ShaderDataBuffers<T> shaderData_;
};