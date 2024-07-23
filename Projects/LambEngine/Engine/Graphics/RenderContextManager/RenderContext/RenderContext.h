#pragma once
#include "../../GraphicsStructs.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

#include "Engine/Graphics/Skeleton.h"

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
        mesh_(nullptr),
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

    virtual void SetMesh(const Mesh* const mesh) = 0;
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
        return mesh_->node.name;
    }

    const Mesh* const GetMesh() const {
        return mesh_;
    }

    const ModelData* const GetModelData() const {
        return modelData_;
    }

    const Node& GetNode() const {
        return mesh_->node;
    }

protected:
    const Mesh* mesh_;
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
        shaderData_.wvpMatrix.Create(bufferSize);
        shaderData_.color.Create(bufferSize);
        shaderData_.shaderStruct.Create(bufferSize);

        drawData_.resize(bufferSize);


        pipeline_ = nullptr;
        drawCount_ = 0u;

        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->BookingHeapPos(5);
        descriptorHeap->CreateView(shaderData_.light);
        descriptorHeap->CreateView(shaderData_.eyePos);
        descriptorHeap->CreateView(shaderData_.wvpMatrix);
        descriptorHeap->CreateView(shaderData_.color);
        descriptorHeap->CreateView(shaderData_.shaderStruct);

        shaderData_.wvpMatrix.OffWright();
        shaderData_.color.OffWright();
        shaderData_.shaderStruct.OffWright();
        shaderData_.light.OffWright(); 
        shaderData_.eyePos.OffWright();

        typeID_ = (typeid(RenderContext<T, bufferSize>).name());
    }
    ~RenderContext() {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->ReleaseView(shaderData_.light.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.eyePos.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.wvpMatrix.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.color.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.shaderStruct.GetHandleUINT());
    }

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
        commandlist->DrawIndexedInstanced(mesh_->indexNumber, drawCount_, 0, 0, 0);
        //commandlist->DrawInstanced(mesh_->vertexNumber, drawCount_, 0, 0);
    }
    
public:
    inline void SetMesh(const Mesh* const mesh) override {
        if (!mesh) {
            throw Lamb::Error::Code<RenderContext>("mesh is nullptr", ErrorPlace);
        }
        mesh_ = mesh;
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

        drawData_[drawCount_].wvpMatrix.worldMat = mesh_->node.loacalMatrix * matrix.worldMat;
        drawData_[drawCount_].wvpMatrix.cameraMat = matrix.cameraMat;
    }
    inline void SetColor(const Vector4& color) override {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        drawData_[drawCount_].color = color;
    }
    inline void SetLight(const DirectionLight& light) override {
        shaderData_.light.OnWright();
        *shaderData_.light = light;
        shaderData_.light.OffWright(); 
    }
    inline void SetCameraPos(const Vector3& cameraPos) {
        shaderData_.eyePos.OnWright();
        *shaderData_.eyePos = cameraPos;
        shaderData_.eyePos.OffWright();
    }
    inline void SetShaderStruct(const T& data) {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }
        drawData_[drawCount_].shaderStruct = data;
    }

    inline void ZSort() override {
        // 1以下ならソートする必要ないので早期リターン
        if (drawCount_ <= 1) {
            return;
        }
        
        
        // 正規化デバイス座標系にしてその深度値を入れる
        for (uint32_t i = 0; i < drawCount_; i++) {
            Mat4x4&& ndcMatrix = drawData_[i].wvpMatrix.worldMat * drawData_[i].wvpMatrix.cameraMat;
            drawData_[i].depth = ndcMatrix.GetTranslate().z;
        }
        // 描画をする部分だけソート
        auto endItr = drawData_.begin() + drawCount_;
        // 深度値でソート(大きい順。奥から描画していくため)
        std::sort(drawData_.begin(), endItr, [](const DrawData<T>& left, const DrawData<T>& right) {
            return left.depth > right.depth;
            }
        );

    }

    inline void DataSet() override {
        shaderData_.wvpMatrix.OnWright();
        shaderData_.color.OnWright();
        shaderData_.shaderStruct.OnWright();

        for (uint32_t i = 0; i < drawCount_; i++) {
            shaderData_.wvpMatrix[i] = std::move(drawData_[i].wvpMatrix);
            shaderData_.color[i] = std::move(drawData_[i].color);
            shaderData_.shaderStruct[i] = std::move(drawData_[i].shaderStruct);
        }

        shaderData_.wvpMatrix.OffWright();
        shaderData_.color.OffWright();
        shaderData_.shaderStruct.OffWright();
    }


private:
    ShaderData<T> shaderData_;
    std::vector<DrawData<T>> drawData_;
};

template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
class SkinRenderContext : public BaseRenderContext {
public:
    SkinRenderContext() :
        shaderData_()
    {
        shaderData_.wvpMatrix.Create(bufferSize);
        shaderData_.color.Create(bufferSize);
        shaderData_.shaderStruct.Create(bufferSize);

        drawData_.resize(bufferSize);


        pipeline_ = nullptr;
        drawCount_ = 0u;

        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->BookingHeapPos(5);
        descriptorHeap->CreateView(shaderData_.light);
        descriptorHeap->CreateView(shaderData_.eyePos);
        descriptorHeap->CreateView(shaderData_.wvpMatrix);
        descriptorHeap->CreateView(shaderData_.color);
        descriptorHeap->CreateView(shaderData_.shaderStruct);

        shaderData_.wvpMatrix.OffWright();
        shaderData_.color.OffWright();
        shaderData_.shaderStruct.OffWright();
        shaderData_.light.OffWright();
        shaderData_.eyePos.OffWright();

        typeID_ = (typeid(SkinRenderContext<T, bufferSize>).name());
    }
    ~SkinRenderContext() {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->ReleaseView(shaderData_.light.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.eyePos.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.wvpMatrix.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.color.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.shaderStruct.GetHandleUINT());
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
        commandlist->SetGraphicsRootDescriptorTable(0, shaderData_.light.GetHandleGPU());
        // ワールドとカメラマトリックス, 色, 各シェーダーの構造体
        commandlist->SetGraphicsRootDescriptorTable(1, shaderData_.wvpMatrix.GetHandleGPU());
        // スキンアニメーション用
        commandlist->SetGraphicsRootDescriptorTable(2, skinCluster_->paletteBuffer.GetHandleGPU());
        // テクスチャ
        commandlist->SetGraphicsRootDescriptorTable(3, descriptorHeap->GetGpuHeapHandle(0));

        std::array vertexBuffer = {
            mesh_->vertexView,
            skinCluster_->infliuenceBufferView
        };

        // 頂点バッファセット
        commandlist->IASetVertexBuffers(0, static_cast<uint32_t>(vertexBuffer.size()), vertexBuffer.data());
        // インデックスバッファセット
        commandlist->IASetIndexBuffer(&mesh_->indexView);
        // ドローコール
        commandlist->DrawIndexedInstanced(mesh_->indexNumber, drawCount_, 0, 0, 0);
        //commandlist->DrawInstanced(mesh_->vertexNumber, drawCount_, 0, 0);
    }

public:
    inline void SetMesh(const Mesh* const mesh) override {
        if (!mesh) {
            throw Lamb::Error::Code<SkinRenderContext>("mesh is nullptr", ErrorPlace);
        }
        mesh_ = mesh;
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

        drawData_[drawCount_].wvpMatrix.worldMat = matrix.worldMat;
        drawData_[drawCount_].wvpMatrix.cameraMat = matrix.cameraMat;
    }
    inline void SetColor(const Vector4& color) override {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<SkinRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }

        drawData_[drawCount_].color = color;
    }
    inline void SetLight(const DirectionLight& light) override {
        shaderData_.light.OnWright();
        *shaderData_.light = light;
        shaderData_.light.OffWright();
    }
    inline void SetCameraPos(const Vector3& cameraPos) {
        shaderData_.eyePos.OnWright();
        *shaderData_.eyePos = cameraPos;
        shaderData_.eyePos.OffWright();
    }
    inline void SetShaderStruct(const T& data) {
        if (bufferSize <= drawCount_) {
            throw Lamb::Error::Code<SkinRenderContext>("drawCount is over " + std::to_string(bufferSize), ErrorPlace);
        }
        drawData_[drawCount_].shaderStruct = data;
    }

    inline void ZSort() override {
        // 1以下ならソートする必要ないので早期リターン
        if (drawCount_ <= 1) {
            return;
        }


        // 正規化デバイス座標系にしてその深度値を入れる
        for (uint32_t i = 0; i < drawCount_; i++) {
            Mat4x4&& ndcMatrix = drawData_[i].wvpMatrix.worldMat * drawData_[i].wvpMatrix.cameraMat;
            drawData_[i].depth = ndcMatrix.GetTranslate().z;
        }
        // 描画をする部分だけソート
        auto endItr = drawData_.begin() + drawCount_;
        // 深度値でソート(大きい順。奥から描画していくため)
        std::sort(drawData_.begin(), endItr, [](const DrawData<T>& left, const DrawData<T>& right) {
            return left.depth > right.depth;
            }
        );
    }

    inline void DataSet() override {
        shaderData_.wvpMatrix.OnWright();
        shaderData_.color.OnWright();
        shaderData_.shaderStruct.OnWright();

        for (uint32_t i = 0; i < drawCount_; i++) {
            shaderData_.wvpMatrix[i] = std::move(drawData_[i].wvpMatrix);
            shaderData_.color[i] = std::move(drawData_[i].color);
            shaderData_.shaderStruct[i] = std::move(drawData_[i].shaderStruct);
        }

        shaderData_.wvpMatrix.OffWright();
        shaderData_.color.OffWright();
        shaderData_.shaderStruct.OffWright();
    }


private:
    ShaderData<T> shaderData_;
    std::vector<DrawData<T>> drawData_;

    SkinCluster* skinCluster_;
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

    const Mesh* const GetMesh() const {
        return renderDatas_.front()->GetMesh();
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