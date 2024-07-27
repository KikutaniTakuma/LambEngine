#include "Object.h"
#include "Utils/EngineInfo.h"
#ifdef _DEBUG
#include "Comp/ButtonComp.h"
#include "Comp/CameraComp.h"
#include "Comp/Camera3DComp.h"
#include "Comp/Camera2DComp.h"
#include "Comp/ChildrenObjectComp.h"
#include "Comp/CsvDataComp.h"
#include "Comp/EventComp.h"
#include "Comp/FallComp.h"
#include "Comp/InputMoveComp.h"
#include "Comp/JsonComp.h"
#include "Comp/ModelRenderComp.h"
#include "Comp/ObbComp.h"
#include "Comp/ObbPushComp.h"
#include "Comp/ModelRenderDataComp.h"
#include "Comp/SceneChangeComp.h"
#include "Comp/SpriteRenderComp.h"
#include "Comp/SpriteRenderDataComp.h"
#include "Comp/TransformComp.h"

#include "GameComp/CannonComp.h"
#include "GameComp/CustomizeComp.h"
#include "GameComp/ItemComp.h"
#include "GameComp/LoopCannonComp.h"
#include "GameComp/PlayerComp.h"
#include "GameComp/SailComp.h"
#include "GameComp/WhirlpoolsComp.h"
#include "GameComp/WindComp.h"
#include "GameComp/WindNodeComp.h"

#endif // _DEBUG

void Object::Init() {
	/*for (auto& i : components_) {
		i.second->Init();
	}*/
}

void Object::FirstUpdate()
{
	float32_t deltaTime = Lamb::DeltaTime();
	this->SetDeltaTime(deltaTime);
	for (auto& i : components_) {
		i.second->FirstUpdate();
	}
}

void Object::Move()
{
	for (auto& i : components_) {
		i.second->Move();
	}
}

void Object::Event() {
	for (auto& i : components_) {
		i.second->Event();
	}
}

void Object::Update() {
	for (auto& i : components_) {
		i.second->Update();
	}
}

void Object::LastUpdate()
{
	for (auto& i : components_) {
		i.second->LastUpdate();
	}
}

void Object::Draw([[maybe_unused]] CameraComp* cameraComp) const
{
	for (auto& i : components_) {
		i.second->Draw(cameraComp);
	}
}

void Object::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {
		if (DebugAddComp()) {
			ImGui::TreePop();
			return;
		}
		ImGui::Text("tags : ");
		for (auto& i : tags_) {
			ImGui::SameLine();
			ImGui::Text("%s, ", i.c_str());
		}
		if (ImGui::TreeNode("componets")) {
			for (auto& i : components_) {
				i.second->Debug(i.first);
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
#endif // _DEBUG
}

bool Object::DebugAddComp() {
#ifdef _DEBUG
	if (ImGui::TreeNode("Comps")) {
		DebugAdd<ButtonComp>();
		DebugAdd<Camera2DComp>();
		DebugAdd<Camera3DComp>();
		DebugAdd<ChildrenObjectComp>();
		DebugAdd<CsvDataComp>();
		DebugAdd<JsonCmop>();
		DebugAdd<EventComp>();
		DebugAdd<FallComp>();
		DebugAdd<InputMoveComp>();
		DebugAdd<ModelRenderComp>();
		DebugAdd<ModelRenderDataComp>();
		DebugAdd<ObbComp>();
		DebugAdd<ObbPushComp>();
		DebugAdd<SceneChangeComp>();
		DebugAdd<SpriteRenderComp>();
		DebugAdd<SpriteRenderDataComp>();
		DebugAdd<TransformComp>();
		DebugAdd<CannonComp>();
		DebugAdd<ItemComp>();
		DebugAdd<LoopCannonComp>();
		DebugAdd<PlayerComp>();
		DebugAdd<SailComp>();
		DebugAdd<WhirlpoolsComp>();
		DebugAdd<WindComp>();
		DebugAdd<WindNodeComp>();
		ImGui::TreePop();

		return true;
	}
#endif // _DEBUG

	return false;
}

void Object::Save(nlohmann::json& json) {
	json["type"] = "Object";
	json["Comps"] = nlohmann::json::array();
	for(auto& comp : this->components_) {
		json["Comps"].push_back(nlohmann::json::object());
		comp.second->Save(json["Comps"].back());
	}
}

