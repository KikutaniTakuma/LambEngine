#include "RenderDataComp.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void RenderDataComp::Init()
{
	type = BlendType::kNone;
	color = Vector4::kIdentity;
	isLighting = true;
}

void RenderDataComp::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {
        // コンボボックスを使ってenumの値を選択する
        if (ImGui::BeginCombo("Enum", kBlendTypeStrs[static_cast<uint32_t>(type)].c_str()))
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

		ImGui::ColorEdit4("color", color.data());
        if (type != BlendType::kNone) {
            ImGui::Checkbox("Lighting", &isLighting);
        }
		ImGui::TreePop();
	}

#endif // _DEBUG
}
