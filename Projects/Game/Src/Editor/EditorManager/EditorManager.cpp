#include "EditorManager.h"

#include "imgui.h"

#include "Game/StageManager/StageManager.h"
#include "Camera/Camera.h"

void EditorManager::Initialize(BlockManager* blockManager, GoalManager* goalManager, WindManager* windManager, WhirlpoolManager* whirlpoolManager) {
	stageIndex_ = StageManager::GetStage();

	saveString_.resize(128);
	saveString_ = StageManager::GetStageString(stageIndex_);

	loadString_.resize(128);
	loadString_ = StageManager::GetStageString(stageIndex_);

	blockEditor_ = std::make_unique<BlockEditor>();
	goalEditor_ = std::make_unique<GoalEditor>();
	windEditor_ = std::make_unique<WindEditor>();
	whirlpoolEditor_ = std::make_unique<WhirlpoolEditor>();

	blockEditor_->SetBlockManager(blockManager);
	blockEditor_->Initialize();

	goalEditor_->SetGoalManager(goalManager);
	goalEditor_->Initialize();

	windEditor_->SetWindManager(windManager);
	windEditor_->Initialize();

	whirlpoolEditor_->SetWhirlpoolManager(whirlpoolManager);
	whirlpoolEditor_->Initialize();
}

void EditorManager::Update() {
	ImGui::Begin("StageEditor");
	goalEditor_->Update();
	blockEditor_->Update();
	whirlpoolEditor_->Update();
	windEditor_->Update();
	if (ImGui::TreeNode("SaveFile")) {
		std::list<std::string> stageList;
		for (uint32_t i = 0; i < StageManager::kMaxStageCount; i++) {
			stageList.emplace_back(StageManager::GetStageString(i));
		}

		// std::vector に変換する
		std::vector<const char*> stageArray;
		for (const auto& stage : stageList) {
			stageArray.push_back(stage.c_str());
		}

		saveString_ = stageArray[stageIndex_];
		// Combo を使用する
		if (ImGui::Combo("Stage", &stageIndex_, stageArray.data(), static_cast<int>(stageArray.size()))) {
			saveString_ = stageArray[stageIndex_];
		}
		if (ImGui::Button("Save") && !saveString_.empty()) {
			saveString_ = stageArray[stageIndex_];
			goalEditor_->SaveFile(saveString_);
			blockEditor_->SaveFile(saveString_);
			whirlpoolEditor_->SaveFile(saveString_);
			windEditor_->SaveFile(saveString_);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("LoadFile")) {
		std::list<std::string> stageList;
		for (uint32_t i = 0; i < StageManager::kMaxStageCount; i++) {
			stageList.emplace_back(StageManager::GetStageString(i));
		}

		// std::vector に変換する
		std::vector<const char*> stageArray;
		for (const auto& stage : stageList) {
			stageArray.push_back(stage.c_str());
		}

		loadString_ = stageArray[stageIndex_];
		// Combo を使用する
		if (ImGui::Combo("Stage", &stageIndex_, stageArray.data(), static_cast<int>(stageArray.size()))) {
			loadString_ = stageArray[stageIndex_];
		}
		if (ImGui::Button("Load") && !loadString_.empty()) {
			loadString_ = stageArray[stageIndex_];
			goalEditor_->Clear();
			blockEditor_->Clear();
			whirlpoolEditor_->Clear();
			windEditor_->Clear();
			goalEditor_->LoadFile(loadString_);
			blockEditor_->LoadFile(loadString_);
			whirlpoolEditor_->LoadFile(loadString_);
			windEditor_->LoadFile(loadString_);
		}
		ImGui::TreePop();
	}
	ImGui::End();
}

void EditorManager::Draw(const Camera& camera) {
	blockEditor_->Draw(camera);
	goalEditor_->Draw(camera);
	whirlpoolEditor_->Draw(camera);
	windEditor_->Draw(camera);
}
