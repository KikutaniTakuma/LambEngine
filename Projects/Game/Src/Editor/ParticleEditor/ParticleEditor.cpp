#include "ParticleEditor.h"
#include <cassert>
#include "imgui.h"
#include "Utils/FileUtils.h"

ParticleEditor* ParticleEditor::pInstance_ = nullptr;

void ParticleEditor::Initialize() {
	assert(!pInstance_);
	pInstance_ = new ParticleEditor;
	pInstance_->camera_.pos.z = -5.0f;
	pInstance_->camera_.Update();
}

void ParticleEditor::Finalize() {
	delete pInstance_;
	pInstance_ = nullptr;
}

ParticleEditor* ParticleEditor::GetInstance() {
	return pInstance_;
}

void ParticleEditor::Editor() {
#ifdef USE_DEBUG_CODE
	if (!isOpen_) {
		inputLoadString_.resize(128);
		ImGui::Begin("ParticleEditor");
		if (ImGui::TreeNode("load")) {
			inputLoadString_.resize(128);
			ImGui::InputText("load name", inputLoadString_.data(), inputLoadString_.size());
			if (ImGui::Button("Create") && !inputLoadString_.empty()) {
				std::string createString;
				createString.clear();
				for (const auto& i : inputLoadString_) {
					if (i == '\0') {
						break;
					}
					createString.push_back(i);
				}
				currentLoadString_ = createString;
				isOpen_ = true;
			}
			ImGui::TreePop();
		}
		ImGui::End();
	}
	else {
		ImGui::Begin("ParticleEditor");
		ImGui::Checkbox("is3DCamera", &is3DCamera_);
		ImGui::Checkbox("isLocalCamera", &isLocalCamera_);

		ImGui::End();

	}

#endif // USE_DEBUG_CODE
}

void ParticleEditor::Draw([[maybe_unused]] const Camera& camera) {

}