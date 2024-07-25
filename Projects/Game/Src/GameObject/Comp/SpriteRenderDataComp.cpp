#include "SpriteRenderDataComp.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

#ifdef _DEBUG
#include "imgui.h"
#include "Utils/FileUtils.h"
#endif // _DEBUG

#ifdef _DEBUG
const std::array<std::string, uint32_t(SpriteRenderDataComp::Offset::kNum)> SpriteRenderDataComp::kComboEnum_ = {
    "kMiddle",
    "kLeftUpper",
    "kRightUpper",
    "kLeftUnder",
    "kRightUnder"
};
#endif // _DEBUG

const std::array<Mat4x4, uint32_t(SpriteRenderDataComp::Offset::kNum)> SpriteRenderDataComp::kOffsetMatrix = {
    Mat4x4::kIdentity,
    Mat4x4::MakeTranslate(Vector3(-1.0f, 1.0f, 0.0f) * 0.5f),
    Mat4x4::MakeTranslate(Vector3(1.0f, 1.0f, 0.0f) * 0.5f),
    Mat4x4::MakeTranslate(Vector3(-1.0f, -1.0f, 0.0f) * 0.5f),
    Mat4x4::MakeTranslate(Vector3(1.0f, -1.0f, 0.0f) * 0.5f)
};

void SpriteRenderDataComp::Init() {
    type = BlendType::kNone;
    color = Vector4::kIdentity;
    fileName = TextureManager::kWhiteTexturePath;

    filePaths_ = Lamb::GetFilePathFormDir("./", ".png");
    auto bmp = Lamb::GetFilePathFormDir("./", ".bmp");
    filePaths_.insert(filePaths_.end(), bmp.begin(), bmp.end());

    euler_ = uvTransform.rotate.ToEuler();

    texHandle = 0;
}

#ifdef _DEBUG
void SpriteRenderDataComp::FirstUpdate() {
    uvTransform.rotate = Quaternion::EulerToQuaternion(euler_);
}
#endif // _DEBUG

void SpriteRenderDataComp::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {
        // コンボボックスを使ってenumの値を選択する
        if (ImGui::BeginCombo("BlendType", kBlendTypeStrs[static_cast<uint32_t>(type)].c_str()))
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(BlendType::kNum); ++i)
            {
                bool isSelected = (type == static_cast<BlendType>(i));
                if (ImGui::Selectable(kBlendTypeStrs[i].c_str(), isSelected))
                {
                    type = static_cast<BlendType>(i);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // コンボボックスを使ってenumの値を選択する
        if (ImGui::BeginCombo("Offset", kComboEnum_[static_cast<uint32_t>(offsetType)].c_str()))
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(Offset::kNum); ++i)
            {
                bool isSelected = (offsetType == static_cast<Offset>(i));
                if (ImGui::Selectable(kComboEnum_[i].c_str(), isSelected))
                {
                    offsetType = static_cast<Offset>(i);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        offsetTransform_ = kOffsetMatrix[static_cast<uint32_t>(offsetType)];

        ImGui::ColorEdit4("color", color.data());
        ImGui::NewLine();
        ImGui::Text("uv");
        ImGui::DragFloat3("scale", uvTransform.scale.data());
        ImGui::DragFloat3("rotate", euler_.data());
        ImGui::DragFloat3("translate", uvTransform.translate.data());

        if (euler_ == Vector3::kZero) {
            euler_ = uvTransform.rotate.ToEuler();
        }
        uvTransform.rotate = Quaternion::EulerToQuaternion(euler_);

        if (ImGui::Button("再読み込み")) {
            size_t size = filePaths_.size();
            filePaths_.clear();
            filePaths_.reserve(size);
            filePaths_ = Lamb::GetFilePathFormDir("./", ".png");
            auto bmp = Lamb::GetFilePathFormDir("./", ".bmp");
            filePaths_.insert(filePaths_.end(), bmp.begin(), bmp.end());
        }

        for (auto itr = filePaths_.begin(); itr != filePaths_.end(); itr++) {
            bool isButton = ImGui::Button("Load");
            if (itr != filePaths_.rbegin().base()) {
                ImGui::SameLine();
            }
            ImGui::Text("%s", itr->string().c_str());

            if (isButton) {
                fileName = itr->string();
                Lamb::SafePtr textureManager = TextureManager::GetInstance();
                textureManager->LoadTexture(fileName);
                texHandle = textureManager->GetHandle(fileName);
            }
        }


		ImGui::TreePop();
	}
#endif // _DEBUG

}
