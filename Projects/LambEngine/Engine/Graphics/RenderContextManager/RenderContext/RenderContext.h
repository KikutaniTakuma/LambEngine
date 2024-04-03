#pragma once
#include "../../GraphicsStructs.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

#include <typeinfo>
#include <concepts>
#include <array>
#include <memory>


class BaseRenderContext {
public:
    static constexpr uint32_t kMaxDrawInstance = 256u;


public:
    BaseRenderContext() = default;
    virtual ~BaseRenderContext() = default;

    virtual void Draw() = 0;
    virtual bool IsDraw() const = 0;

    const std::string& GetID() const {
        return typeID_;
    }

    virtual void SetMesh(const Mesh& mesh) = 0;
    virtual void SetPipeline(Pipeline* const pipeline) = 0;
    virtual void SetWVPMatrix(const WVPMatrix& matrix) = 0;
    virtual void SetColor(const Vector4& color) = 0;
    virtual void SetLight(const Light& light) = 0;

protected:
    Mesh mesh_;

    Pipeline* pipeline_;
    uint32_t drawCount_;

    std::string typeID_;
};

using RenderData = BaseRenderContext;


template<class T = int, uint32_t bufferSize = RenderData::kMaxDrawInstance>
class RenderContext : public BaseRenderContext {
public:
    RenderContext():
        shaderData_()
    {
        pipeline_ = nullptr;
        drawCount_ = 0u;

        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->BookingHeapPos(4);
        descriptorHeap->CreateView(shaderData_.light);
        descriptorHeap->CreateView(shaderData_.wvpMatrix);
        descriptorHeap->CreateView(shaderData_.color);
        descriptorHeap->CreateView(shaderData_.shaderStruct);

        typeID_ = (typeid(RenderContext<>).name());
    }
    ~RenderContext() {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->ReleaseView(shaderData_.light.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.wvpMatrix.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.color.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.shaderStruct.GetHandleUINT());
    }

    RenderContext(const RenderContext&) = delete;
    RenderContext(RenderContext&&) = delete;

    RenderContext& operator=(const RenderContext&) = delete;
    RenderContext& operator=(RenderContext&&) = delete;

public:
    [[noreturn]] void AddDrawCount() {
        drawCount_++;
    }

    [[noreturn]] void ResetDrawCount() {
        drawCount_ = 0u;
    }

    [[nodiscard]] bool IsDraw() const override {
        return drawCount_ != 0u;
    }

public:
    [[noreturn]] void Draw() override {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();
        // コマンドリスト
        ID3D12GraphicsCommandList* const commandlist = DirectXCommand::GetInstance()->GetCommandList();

        // パイプライン設定
        pipeline_->Use();

        // ライト構造体
        commandlist->SetGraphicsRootDescriptorTable(0, shaderData_.light.GetHandleGPU());
        // ワールドとカメラマトリックス
        commandlist->SetGraphicsRootDescriptorTable(1, shaderData_.wvpMatrix.GetHandleGPU());
        // 色
        commandlist->SetGraphicsRootDescriptorTable(2, shaderData_.color.GetHandleGPU());
        // 各シェーダーの構造体
        commandlist->SetGraphicsRootDescriptorTable(3, shaderData_.shaderStruct.GetHandleGPU());
        // テクスチャ
        commandlist->SetGraphicsRootDescriptorTable(4, descriptorHeap->GetGpuHeapHandle(0));

        // 頂点バッファセット
        commandlist->IASetVertexBuffers(0, 1, &mesh_.vertexView);
        // インデックスバッファセット
        commandlist->IASetIndexBuffer(&mesh_.indexView);
        // ドローコール
        commandlist->DrawIndexedInstanced(mesh_.indexNumber, drawCount_, 0, 0, 0);
    }
    
public:
    inline [[noreturn]] void SetMesh(const Mesh& mesh) override {
        mesh_ = mesh;
    }
    inline [[noreturn]] void SetPipeline(Pipeline* const pipeline) override {
        if (not pipeline_) {
            throw Lamb::Error::Code<RenderContext>("pipeline is nullptr", __func__);
        }
        pipeline_ = pipeline;
    }
    inline [[noreturn]] void SetWVPMatrix(const WVPMatrix& matrix) override {
        if (kMaxDrawInstance <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over 256", __func__);
        }

        shaderData_.wvpMatrix[drawCount_] = matrix;
    }
    inline [[noreturn]] void SetColor(const Vector4& color) override {
        if (kMaxDrawInstance <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over 256", __func__);
        }

        shaderData_.color[drawCount_] = color;
    }
    inline [[noreturn]] void SetLight(const Light& light) override {
        *shaderData_.light = light;
    }
    inline [[noreturn]] void SetSahderStruct(const T& data) {
        if (kMaxDrawInstance <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over 256", __func__);
        }
        shaderData_.shaderStruct[drawCount_] = data;
    }


private:
    ShaderData<T, bufferSize> shaderData_;
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
    inline [[noreturn]] void Set(RenderData* const renderData, BlendType blend) {
        if (not renderData) {
            throw Lamb::Error::Code<RenderSet>("renderData is nullptr", __func__);
        }

        renderDatas_[blend].reset(renderData);
    }


    inline [[noreturn]] void Draw() {
        for (auto& i : renderDatas_) {
            if (i->IsDraw()) {
                i->Draw();
            }
        }
    }

public:
    inline [[nodiscard]] RenderData* GetRenderContext(BlendType blend) {
        return renderDatas_[blend].get();
    }

    // ごり押し
    // やばい
    // マジでヤバイ
    template<IsBasedRenderContext ClassName>
    inline [[nodiscard]] ClassName* GetRenderContextDowncast(BlendType blend) {
        if (typeid(ClassName).name() != renderDatas_[blend].GetID()) {
            throw Lamb::Error::Code<RenderSet>("does not match class type", __func__);
        }

        return dynamic_cast<ClassName*>(renderDatas_[blend].get());
    }

private:
    std::array<std::unique_ptr<RenderData>, BlendType::kNum> renderDatas_;
};