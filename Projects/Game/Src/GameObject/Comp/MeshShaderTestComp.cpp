#include "MeshShaderTestComp.h"
#include "TransformComp.h"
#include "CameraComp.h"

#include "ModelRenderDataComp.h"

#ifdef _DEBUG
#include "Utils/FileUtils.h"
#endif // _DEBUG

void MeshShaderTestComp::Save(nlohmann::json& json) {
	SaveCompName(json);
}

void MeshShaderTestComp::Load([[maybe_unused]]nlohmann::json& json)
{
}

void MeshShaderTestComp::Load() {
    if (renderDataComp_->fileName.empty()) {
        return;
    }
    meshDrawer_->Load(renderDataComp_->fileName);
}

void MeshShaderTestComp::Init() {

	transform_ = object_.AddComp<TransformComp>();

    renderDataComp_ = object_.AddComp<ModelRenderDataComp>();

    meshDrawer_ = std::make_unique<MeshShaderTest>();

#ifdef _DEBUG
    filePaths_ = Lamb::GetFilePathFormDir("./", ".obj");
    auto bmp = Lamb::GetFilePathFormDir("./", ".gltf");
    filePaths_.insert(filePaths_.end(), bmp.begin(), bmp.end());
#endif // _DEBUG
}

void MeshShaderTestComp::Draw(CameraComp* cameraComp)
{
    if (meshDrawer_ and renderDataComp_->isDraw) {
        meshDrawer_->Draw(
            transform_->GetWorldMatrix(),
            cameraComp->GetCameraMatrix(),
            renderDataComp_->type
        );
    }

}

void MeshShaderTestComp::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
    if (ImGui::TreeNode(guiName.c_str())) {
        ImGui::Checkbox("描画有効", &(renderDataComp_->isDraw));

        // コンボボックスを使ってenumの値を選択する
        if (ImGui::BeginCombo("BlendType", kBlendTypeStrs[static_cast<uint32_t>(renderDataComp_->type)].c_str()))
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(BlendType::kNum); ++i)
            {
                bool isSelected = (renderDataComp_->type == static_cast<BlendType>(i));
                if (ImGui::Selectable(kBlendTypeStrs[i].c_str(), isSelected))
                {
                    renderDataComp_->type = static_cast<BlendType>(i);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::ColorEdit4("color", renderDataComp_->color.data());
        if (renderDataComp_->type != BlendType::kNone) {
            ImGui::Checkbox("Lighting", &renderDataComp_->isLighting);
        }

        ImGui::Text("モデル %s", renderDataComp_->fileName.c_str());

        if (ImGui::Button("ファイルパス再読み込み")) {
            size_t size = filePaths_.size();
            filePaths_.clear();
            filePaths_.reserve(size);
            filePaths_ = Lamb::GetFilePathFormDir("./", ".obj");
            auto bmp = Lamb::GetFilePathFormDir("./", ".gltf");
            filePaths_.insert(filePaths_.end(), bmp.begin(), bmp.end());
        }

        if (ImGui::TreeNode("モデル読み込み")) {
            for (auto itr = filePaths_.begin(); itr != filePaths_.end(); itr++) {
                if (ImGui::Button(itr->string().c_str())) {
                    renderDataComp_->fileName = itr->string();
                    Load();
                }
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

#endif // _DEBUG
}
