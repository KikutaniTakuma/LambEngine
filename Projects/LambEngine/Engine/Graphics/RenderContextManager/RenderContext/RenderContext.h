#pragma once
#include "../../GraphicsStructs.h"
#include "../../Meshlet/Meshlet.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

#include "Engine/Graphics/Skeleton.h"

#include "Utils/EngineInfo.h"

#include <typeinfo>
#include <concepts>
#include <array>
#include <memory>
#include <algorithm>


class BaseRenderContext {
public:
    static constexpr uint32_t kMaxDrawInstance = 256u;


public:
    BaseRenderContext():
        vertexIndexData_(nullptr),
        modelData_(nullptr),
        pipeline_(nullptr),
        drawCount_(0),
        typeID_()
    {}
    virtual ~BaseRenderContext() = default;

    virtual void Draw() const = 0;

    const std::string& GetID() const {
        return typeID_;
    }

    virtual void SetVertexIndexData(const VertexIndexData* const) = 0;
    virtual void SetModelData(const ModelData* const modelData) = 0;
    virtual void SetPipeline(Pipeline* const pipeline) = 0;
    virtual void SetWVPMatrix(const WVPMatrix& matrix) = 0;
    virtual void SetColor(const Vector4& color) = 0;
    virtual void SetLight(const DirectionLight& light) = 0;
    virtual void SetCameraPos(const Vector3& cameraPos) = 0;
    virtual void ZSort() = 0;
    virtual void DataSet() = 0;



public:
    void AddDrawCount() {
        drawCount_++;
    }

    void ResetDrawCount() {
        drawCount_ = 0u;
    }

    bool IsDraw() const {
        return drawCount_ != 0u;
    }

    const std::string& GetRootName() const {
        return vertexIndexData_->node.name;
    }

    const VertexIndexData* const GetVertexIndexData() const {
        return vertexIndexData_;
    }

    const ModelData* const GetModelData() const {
        return modelData_;
    }

    const Node& GetNode() const {
        return vertexIndexData_->node;
    }

protected:
    const VertexIndexData* vertexIndexData_;
    const ModelData* modelData_;


    Pipeline* pipeline_;
    uint32_t drawCount_;

    std::string typeID_;
};

using RenderData = BaseRenderContext;


template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
class RenderContext : public BaseRenderContext {
public:
    RenderContext():
        shaderData_()
    {
        for (uint32_t i = 0; i < DirectXSwapChain::kBackBufferNumber; ++i) {
            shaderData_[i].wvpMatrix.Create(bufferSize);
            shaderData_[i].color.Create(bufferSize);
            shaderData_[i].shaderStruct.Create(bufferSize);
        }

        drawData_.color.resize(bufferSize);
        drawData_.depth.resize(bufferSize);
        drawData_.shaderStruct.resize(bufferSize);
        drawData_.wvpMatrix.resize(bufferSize);


        pipeline_ = nullptr;
        drawCount_ = 0u;

        for (uint32_t i = 0; i < DirectXSwapChain::kBackBufferNumber; ++i) {
            shaderData_[i].wvpMatrix.OffWright();
            shaderData_[i].color.OffWright();
            shaderData_[i].shaderStruct.OffWright();
            shaderData_[i].light.OffWright();
            shaderData_[i].eyePos.OffWright();
        }
       

        typeID_ = (typeid(RenderContext<T, bufferSize>).name());
    }
    ~RenderContext() = default;

    RenderContext(const RenderContext&) = delete;
    RenderContext(RenderContext&&) = delete;

    RenderContext& operator=(const RenderContext&) = delete;
    RenderContext& operator=(RenderContext&&) = delete;

public:
    void Draw() const override {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();
        // コマンドリスト
        ID3D12GraphicsCommandList* const commandlist = DirectXCommand::GetMainCommandlist()->GetCommandList();

        // パイプライン設定
        pipeline_->Use();

        // ライト構造体
        commandlist->SetGraphicsRootConstantBufferView(0, shaderData_[Lamb::GetBufferIndex()].light.GetGPUVtlAdrs());
        // カメラポジション
        commandlist->SetGraphicsRootConstantBufferView(1, shaderData_[Lamb::GetBufferIndex()].eyePos.GetGPUVtlAdrs());
        // ワールドとカメラマトリックス
        commandlist->SetGraphicsRootShaderResourceView(2, shaderData_[Lamb::GetBufferIndex()].wvpMatrix.GetGPUVtlAdrs());
        // 色
        commandlist->SetGraphicsRootShaderResourceView(3, shaderData_[Lamb::GetBufferIndex()].color.GetGPUVtlAdrs());
        // shaderの固有構造体
        commandlist->SetGraphicsRootShaderResourceView(4, shaderData_[Lamb::GetBufferIndex()].shaderStruct.GetGPUVtlAdrs());
        // テクスチャ
        commandlist->SetGraphicsRootDescriptorTable(5, descriptorHeap->GetGpuHeapHandle(0));

        // 頂点バッファセット
        commandlist->IASetVertexBuffers(0, 1, &(vertexIndexData_->vertexView));
        // インデックスバッファセット
        commandlist->IASetIndexBuffer(&(vertexIndexData_->indexView));
        // ドローコール
        commandlist->DrawIndexedInstanced(vertexIndexData_->indexNumber, drawCount_, 0, 0, 0);
    }
    
public:
    inline void SetVertexIndexData(const VertexIndexData* const vertexIndexData) override {
        if (!vertexIndexData) {
            throw Lamb::Error::Code<RenderContext>("vertexIndexData is nullptr", ErrorPlace);
        }
        vertexIndexData_ = vertexIndexData;
    }
    inline void SetModelData(const ModelData* const modelData) override {
        if (!modelData) {
            throw Lamb::Error::Code<RenderContext>("modelData is nullptr", ErrorPlace);
        }
        modelData_ = modelData;
    }
    inline void SetPipeline(Pipeline* const pipeline) override {
        if (!pipeline) {
            throw Lamb::Error::Code<RenderContext>("pipeline is nullptr", ErrorPlace);
        }
        pipeline_ = pipeline;
    }
    inline void SetWVPMatrix(const WVPMatrix& matrix) override {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        drawData_.wvpMatrix[drawCount_].worldMat = vertexIndexData_->node.loacalMatrix * matrix.worldMat;
        drawData_.wvpMatrix[drawCount_].cameraMat = matrix.cameraMat;
    }
    inline void SetColor(const Vector4& color) override {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        drawData_.color[drawCount_] = color;
    }
    inline void SetLight(const DirectionLight& light) override {
        shaderData_[Lamb::GetBufferIndex()].light.OnWright();
        *shaderData_[Lamb::GetBufferIndex()].light = light;
        shaderData_[Lamb::GetBufferIndex()].light.OffWright();
    }
    inline void SetCameraPos(const Vector3& cameraPos) {
        shaderData_[Lamb::GetBufferIndex()].eyePos.OnWright();
        *shaderData_[Lamb::GetBufferIndex()].eyePos = cameraPos;
        shaderData_[Lamb::GetBufferIndex()].eyePos.OffWright();
    }
    inline void SetShaderStruct(const T& data) {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }
        drawData_.shaderStruct[drawCount_] = data;
    }

    inline void ZSort() override {
        // 1以下ならソートする必要ないので早期リターン
        if (drawCount_ <= 1) {
            return;
        }


        // 正規化デバイス座標系にしてその深度値を入れる
        for (uint32_t i = 0; i < drawCount_; i++) {
            Mat4x4&& ndcMatrix = drawData_.wvpMatrix[i].worldMat * drawData_.wvpMatrix[i].cameraMat;
            // 深度値
            drawData_.depth[i] = ndcMatrix.back().at(3);
        }
        // 描画をする部分だけソート
        auto endItr = drawData_.depth.begin() + drawCount_;
        // 深度値でソート(大きい順。奥から描画していくため)
        std::sort(drawData_.depth.begin(), endItr, [](float left, float right) {
            return right < left;
            }
        );

    }

    inline void DataSet() override {
        shaderData_[Lamb::GetBufferIndex()].wvpMatrix.MemCpy(
            drawData_.wvpMatrix.data(),
            sizeof(WVPMatrix) * drawCount_
        );

        shaderData_[Lamb::GetBufferIndex()].color.MemCpy(
            drawData_.color.data(),
            sizeof(Vector4) * drawCount_
        );

        shaderData_[Lamb::GetBufferIndex()].shaderStruct.MemCpy(
            drawData_.shaderStruct.data(),
            sizeof(T) * drawCount_
        );
    }


private:
    std::array<ShaderData<T>, DirectXSwapChain::kBackBufferNumber> shaderData_;
    DrawData<T> drawData_;
};

template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
class SkinRenderContext : public BaseRenderContext {
public:
    SkinRenderContext() :
        shaderData_()
    {
        shaderData_[Lamb::GetBufferIndex()].wvpMatrix.Create(bufferSize);
        shaderData_[Lamb::GetBufferIndex()].color.Create(bufferSize);
        shaderData_[Lamb::GetBufferIndex()].shaderStruct.Create(bufferSize);

        drawData_.color.resize(bufferSize);
        drawData_.depth.resize(bufferSize);
        drawData_.shaderStruct.resize(bufferSize);
        drawData_.wvpMatrix.resize(bufferSize);

        for (uint32_t i = 0; i < DirectXSwapChain::kBackBufferNumber; ++i) {
            shaderData_[i].wvpMatrix.OffWright();
            shaderData_[i].color.OffWright();
            shaderData_[i].shaderStruct.OffWright();
            shaderData_[i].light.OffWright();
            shaderData_[i].eyePos.OffWright();
        }

        pipeline_ = nullptr;
        drawCount_ = 0u;

        typeID_ = (typeid(SkinRenderContext<T, bufferSize>).name());
    }
    ~SkinRenderContext() {

    }

    SkinRenderContext(const SkinRenderContext&) = delete;
    SkinRenderContext(SkinRenderContext&&) = delete;

    SkinRenderContext& operator=(const SkinRenderContext&) = delete;
    SkinRenderContext& operator=(SkinRenderContext&&) = delete;

public:
    void Draw() const override {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();
        // コマンドリスト
        ID3D12GraphicsCommandList* const commandlist = DirectXCommand::GetMainCommandlist()->GetCommandList();

        // パイプライン設定
        pipeline_->Use();

        // ライト構造体
        commandlist->SetGraphicsRootConstantBufferView(0, shaderData_[Lamb::GetBufferIndex()].light.GetGPUVtlAdrs());
        // カメラポジション
        commandlist->SetGraphicsRootConstantBufferView(1, shaderData_[Lamb::GetBufferIndex()].eyePos.GetGPUVtlAdrs());
        // ワールドとカメラマトリックス
        commandlist->SetGraphicsRootShaderResourceView(2, shaderData_[Lamb::GetBufferIndex()].wvpMatrix.GetGPUVtlAdrs());
        // 色
        commandlist->SetGraphicsRootShaderResourceView(3, shaderData_[Lamb::GetBufferIndex()].color.GetGPUVtlAdrs());
        // shaderの固有構造体
        commandlist->SetGraphicsRootShaderResourceView(4, shaderData_[Lamb::GetBufferIndex()].shaderStruct.GetGPUVtlAdrs());
        // スキンアニメーション用
        commandlist->SetGraphicsRootShaderResourceView(5, skinCluster_->paletteBuffer[Lamb::GetBufferIndex()].GetGPUVtlAdrs());
        // テクスチャ
        commandlist->SetGraphicsRootDescriptorTable(6, descriptorHeap->GetGpuHeapHandle(0));

        std::array vertexBuffer = {
            vertexIndexData_->vertexView,
            skinCluster_->infliuenceBufferView
        };

        // 頂点バッファセット
        commandlist->IASetVertexBuffers(0, static_cast<uint32_t>(vertexBuffer.size()), vertexBuffer.data());
        // インデックスバッファセット
        commandlist->IASetIndexBuffer(&vertexIndexData_->indexView);
        // ドローコール
        commandlist->DrawIndexedInstanced(vertexIndexData_->indexNumber, drawCount_, 0, 0, 0);
        //commandlist->DrawInstanced(mesh_->vertexNumber, drawCount_, 0, 0);
    }

public:
    inline void SetVertexIndexData(const VertexIndexData* const vertexIndexData) override {
        if (!vertexIndexData) {
            throw Lamb::Error::Code<SkinRenderContext>("vertexIndexData is nullptr", ErrorPlace);
        }
        vertexIndexData_ = vertexIndexData;
    }
    inline void SetModelData(const ModelData* const modelData) override {
        if (!modelData) {
            throw Lamb::Error::Code<SkinRenderContext>("modelData is nullptr", ErrorPlace);
        }
        modelData_ = modelData;
    }
    inline void SetPipeline(Pipeline* const pipeline) override {
        if (!pipeline) {
            throw Lamb::Error::Code<SkinRenderContext>("pipeline is nullptr", ErrorPlace);
        }
        pipeline_ = pipeline;
    }
    inline void SetSkinCluster(SkinCluster* skinCluster) {
        if (not skinCluster) {
            throw Lamb::Error::Code<SkinRenderContext>("skinCluster is nullptr", ErrorPlace);
        }
        skinCluster_ = skinCluster;
    }
    inline void SetWVPMatrix(const WVPMatrix& matrix) override {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<SkinRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        drawData_.wvpMatrix[drawCount_].worldMat = matrix.worldMat;
        drawData_.wvpMatrix[drawCount_].cameraMat = matrix.cameraMat;
    }
    inline void SetColor(const Vector4& color) override {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<SkinRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        drawData_.color[drawCount_] = color;
    }
    inline void SetLight(const DirectionLight& light) override {
        shaderData_[Lamb::GetBufferIndex()].light.OnWright();
        *shaderData_[Lamb::GetBufferIndex()].light = light;
        shaderData_[Lamb::GetBufferIndex()].light.OffWright();
    }
    inline void SetCameraPos(const Vector3& cameraPos) {
        shaderData_[Lamb::GetBufferIndex()].eyePos.OnWright();
        *shaderData_[Lamb::GetBufferIndex()].eyePos = cameraPos;
        shaderData_[Lamb::GetBufferIndex()].eyePos.OffWright();
    }
    inline void SetShaderStruct(const T& data) {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<SkinRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }
        drawData_.shaderStruct[drawCount_] = data;
    }

    inline void ZSort() override {
        // 1以下ならソートする必要ないので早期リターン
        if (drawCount_ <= 1) {
            return;
        }


        // 正規化デバイス座標系にしてその深度値を入れる
        for (uint32_t i = 0; i < drawCount_; i++) {
            Mat4x4&& ndcMatrix = drawData_.wvpMatrix[i].worldMat * drawData_.wvpMatrix[i].cameraMat;
            // 深度値
            drawData_.depth[i] = ndcMatrix.back().at(3);
        }
        // 描画をする部分だけソート
        auto endItr = drawData_.depth.begin() + drawCount_;
        // 深度値でソート(大きい順。奥から描画していくため)
        std::sort(drawData_.depth.begin(), endItr, [](float left, float right) {
            return right < left;
            }
        );
    }

    inline void DataSet() override {
        shaderData_[Lamb::GetBufferIndex()].wvpMatrix.MemCpy(
            drawData_.wvpMatrix.data(),
            sizeof(WVPMatrix) * drawCount_
        );

        shaderData_[Lamb::GetBufferIndex()].color.MemCpy(
            drawData_.color.data(),
            sizeof(Vector4) * drawCount_
        );

        shaderData_[Lamb::GetBufferIndex()].shaderStruct.MemCpy(
            drawData_.shaderStruct.data(),
            sizeof(T) * drawCount_
        );
    }


private:
    std::array<ShaderData<T>, DirectXSwapChain::kBackBufferNumber> shaderData_;
    DrawData<T> drawData_;

    Lamb::SafePtr<SkinCluster> skinCluster_;
};

// MeshShaderテスト用
template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
class MeshRenderContext : public BaseRenderContext {
public:
    MeshRenderContext() :
        shaderData_()
    {
        constexpr uint32_t kBufferSize = bufferSize;
        drawData_.color.resize(kBufferSize);
        drawData_.depth.resize(kBufferSize);
        drawData_.shaderStruct.resize(kBufferSize);
        drawData_.wvpMatrix.resize(kBufferSize);
       
        std::for_each(
            shaderStruct_.begin(),
            shaderStruct_.end(),
            [kBufferSize](auto& n) {
                n.Create(kBufferSize);
            }
        );
        std::for_each(
            colors_.begin(),
            colors_.end(),
            [kBufferSize](auto& n) {
                n.Create(kBufferSize);
            }
        );

        typeID_ = (typeid(MeshRenderContext).name());
    }
    ~MeshRenderContext() {
        
    }

    MeshRenderContext(const MeshRenderContext&) = delete;
    MeshRenderContext(MeshRenderContext&&) = delete;

    MeshRenderContext& operator=(const MeshRenderContext&) = delete;
    MeshRenderContext& operator=(MeshRenderContext&&) = delete;

public:
    void Draw() const override {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();
        
        // コマンドリスト
        Lamb::SafePtr commandlist = DirectXCommand::GetMainCommandlist()->GetCommandList();

        // パイプライン設定
        pipeline_->Use();
        // Light
        commandlist->SetGraphicsRootConstantBufferView(0, light_[Lamb::GetBufferIndex()].GetGPUVtlAdrs());
        // cameraPos
        commandlist->SetGraphicsRootConstantBufferView(1, eyePos_[Lamb::GetBufferIndex()].GetGPUVtlAdrs());
        // インスタンスカウント
        commandlist->SetGraphicsRootConstantBufferView(2, instanceCount_[Lamb::GetBufferIndex()].GetGPUVtlAdrs());


        // Transform
        commandlist->SetGraphicsRootShaderResourceView(3, shaderData_->gTransform[Lamb::GetBufferIndex()].GetGPUVtlAdrs());
        // gVertices
        commandlist->SetGraphicsRootShaderResourceView(4, shaderData_->gVertices.GetGPUVtlAdrs());
        // gUniqueVertexIndices
        commandlist->SetGraphicsRootShaderResourceView(5, shaderData_->gUniqueVertexIndices.GetGPUVtlAdrs());
        // gPrimitiveIndices
        commandlist->SetGraphicsRootShaderResourceView(6, shaderData_->gPrimitiveIndices.GetGPUVtlAdrs());
        // gMeshlets
        commandlist->SetGraphicsRootShaderResourceView(7, shaderData_->gMeshlets.GetGPUVtlAdrs());
        // 色
        commandlist->SetGraphicsRootShaderResourceView(8, colors_[Lamb::GetBufferIndex()].GetGPUVtlAdrs());
        // 各shader固有の構造体
        commandlist->SetGraphicsRootShaderResourceView(9, shaderStruct_[Lamb::GetBufferIndex()].GetGPUVtlAdrs());
        // Textures
        commandlist->SetGraphicsRootDescriptorTable(10, descriptorHeap->GetGpuHeapHandle(0));
        // ドローコール
        commandlist->DispatchMesh(shaderData_->meshletCount, 1, 1);
    }

public:
    inline void SetVertexIndexData(const VertexIndexData* const vertexIndexData) override {
        if (!vertexIndexData) {
            throw Lamb::Error::Code<MeshRenderContext>("vertexIndexData is nullptr", ErrorPlace);
        }
        vertexIndexData_ = vertexIndexData;
    }
    inline void SetModelData([[maybe_unused]]const ModelData* const modelData) override {
        
    }
    inline void SetPipeline(Pipeline* const pipeline) override {
        if (!pipeline) {
            throw Lamb::Error::Code<MeshRenderContext>("pipeline is nullptr", ErrorPlace);
        }
        pipeline_ = pipeline;
    }
    inline void SetWVPMatrix(const WVPMatrix& matrix) override {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<MeshRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        drawData_.wvpMatrix[drawCount_].worldMat = vertexIndexData_->node.loacalMatrix *  matrix.worldMat;
        drawData_.wvpMatrix[drawCount_].cameraMat = matrix.cameraMat;
    }
    inline void SetColor(const Vector4& color) override {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<MeshRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        drawData_.color[drawCount_] = color;
    }
    inline void SetLight(const DirectionLight& light) override {
        light_[Lamb::GetBufferIndex()].OnWright();
        *light_[Lamb::GetBufferIndex()] = light;
        light_[Lamb::GetBufferIndex()].OffWright();
    }
    inline void SetCameraPos(const Vector3& cameraPos) {
        eyePos_[Lamb::GetBufferIndex()].OnWright();
        *eyePos_[Lamb::GetBufferIndex()] = cameraPos;
        eyePos_[Lamb::GetBufferIndex()].OffWright();
    }
    inline void SetShaderStruct(const T& data) {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<MeshRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }
        drawData_.shaderStruct[drawCount_] = data;
    }

    inline void ZSort() override {
        // 1以下ならソートする必要ないので早期リターン
        if (drawCount_ <= 1) {
            return;
        }


        // 正規化デバイス座標系にしてその深度値を入れる
        for (uint32_t i = 0; i < drawCount_; i++) {
            Mat4x4&& ndcMatrix = drawData_.wvpMatrix[i].worldMat * drawData_.wvpMatrix[i].cameraMat;
            // 深度値
            drawData_.depth[i] = ndcMatrix.back().at(3);
        }
        // 描画をする部分だけソート
        auto endItr = drawData_.depth.begin() + drawCount_;
        // 深度値でソート(大きい順。奥から描画していくため)
        std::sort(drawData_.depth.begin(), endItr, [](float left, float right) {
            return right < left;
            }
        );
    }

    inline void DataSet() override {

        shaderData_->gTransform[Lamb::GetBufferIndex()].MemCpy(
            drawData_.wvpMatrix.data(), sizeof(WVPMatrix) * drawCount_
        );
        shaderStruct_[Lamb::GetBufferIndex()].MemCpy(
            drawData_.shaderStruct.data(), sizeof(T) * drawCount_
        );
        colors_[Lamb::GetBufferIndex()].MemCpy(
            drawData_.color.data(), sizeof(Vector4) * drawCount_
        );

        instanceCount_[Lamb::GetBufferIndex()].OnWright();
        *instanceCount_[Lamb::GetBufferIndex()] = drawCount_;
        instanceCount_[Lamb::GetBufferIndex()].OffWright();
    }

    inline void SetMeshShaderData(Lamb::SafePtr<MeshShaderData> shaderData) {
        shaderData_ = shaderData;
    }

private:
    Lamb::SafePtr<MeshShaderData> shaderData_;

    std::array<ConstantBuffer<DirectionLight>, DirectXSwapChain::kBackBufferNumber> light_;
    std::array<ConstantBuffer<Vector3>, DirectXSwapChain::kBackBufferNumber> eyePos_;
    std::array<ConstantBuffer<uint32_t>, DirectXSwapChain::kBackBufferNumber> instanceCount_;
    std::array<StructuredBuffer<T>, DirectXSwapChain::kBackBufferNumber> shaderStruct_;
    std::array<StructuredBuffer<Vector4>, DirectXSwapChain::kBackBufferNumber> colors_;

   DrawData<T> drawData_;
};


template<class T>
concept IsBasedRenderContext =  std::is_base_of_v<BaseRenderContext, T>;

class RenderSet {
public:
    RenderSet() = default;
    ~RenderSet() = default;
    RenderSet(const RenderSet&) = delete;
    RenderSet(RenderSet&&) = delete;

    RenderSet& operator=(const RenderSet&) = delete;
    RenderSet& operator=(RenderSet&&) = delete;


public:
    inline void Set(RenderData* const renderData, BlendType blend) {
        if (not renderData) {
            throw Lamb::Error::Code<RenderSet>("renderData is nullptr", ErrorPlace);
        }

        renderDatas_[blend].reset(renderData);
    }

    RenderData* const GetRenderData(BlendType blend) const {
        return renderDatas_[blend].get();
    }
    bool IsDraw(BlendType blend) const {
        return renderDatas_[blend]->IsDraw();
    }

    inline void Draw() {
        for (auto& i : renderDatas_) {
            if (i->IsDraw()) {
                i->Draw();
            }
        }
    }

    inline void ResetDrawCount() {
        for (auto& i : renderDatas_) {
            i->ResetDrawCount();
        }
    }

    inline void DrawAndResetDrawCount() {
        Draw();
        ResetDrawCount();
    }

    inline void DataSet() {
        for (auto& i : renderDatas_) {
            i->DataSet();
        }
    }

public:
    inline RenderData* GetRenderContext(BlendType blend) {
        return renderDatas_[blend].get();
    }

    // ごり押し
    // やばい
    // マジでヤバイ
    // 使うときは気を付けて(一応型が違ったらエラーは出る)
    template<IsBasedRenderContext ClassName>
    inline ClassName* GetRenderContextDowncast(BlendType blend) {
        if (typeid(ClassName).name() != renderDatas_[blend]->GetID()) {
            throw Lamb::Error::Code<RenderSet>("does not match class type", ErrorPlace);
        }

        return dynamic_cast<ClassName*>(renderDatas_[blend].get());
    }

    const std::string& GetRootNodeName() const {
        return renderDatas_.front()->GetRootName();
    }

    const Node& GetNode() const {
        return renderDatas_.front()->GetNode();
    }

    const VertexIndexData* const GetMesh() const {
        return renderDatas_.front()->GetVertexIndexData();
    }
    const ModelData* const GetModelData() const {
        return renderDatas_.front()->GetModelData();
    }

public:
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::iterator begin() {
        return renderDatas_.begin();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::iterator end() {
        return renderDatas_.end();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::const_iterator cbegin() const {
        return renderDatas_.cbegin();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::const_iterator cend() const {
        return renderDatas_.cend();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::reverse_iterator rbegin() {
        return renderDatas_.rbegin();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::reverse_iterator rend() {
        return renderDatas_.rend();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::const_reverse_iterator crbegin() const {
        return renderDatas_.crbegin();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::const_reverse_iterator crend() const {
        return renderDatas_.crend();
    }

    std::unique_ptr<RenderData>* const data() {
        return renderDatas_.data();
    }
    const std::unique_ptr<RenderData>* const data() const {
        return renderDatas_.data();
    }

private:
    std::array<std::unique_ptr<RenderData>, BlendType::kNum> renderDatas_;
};