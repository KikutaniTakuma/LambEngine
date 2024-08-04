#include "Object.h"
#include "Utils/EngineInfo.h"
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
#include "Comp/ModelRenderDataComp.h"
#include "Comp/ObbComp.h"
#include "Comp/ObbPushComp.h"
#include "Comp/QuadComp.h"
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
#include "GameComp/UIDrawComp.h"
#include "GameComp/WhirlpoolsComp.h"
#include "GameComp/WindComp.h"
#include "GameComp/WindNodeComp.h"
void Object::Init() {
	/*for (auto& i : components_) {
		i.second->Init();
	}*/
}

void Object::FirstUpdate()
{
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
		DebugAdd<JsonComp>();
		DebugAdd<EventComp>();
		DebugAdd<FallComp>();
		DebugAdd<InputMoveComp>();
		DebugAdd<ModelRenderComp>();
		DebugAdd<ModelRenderDataComp>();
		DebugAdd<ObbComp>();
		DebugAdd<ObbPushComp>();
		DebugAdd<QuadComp>();
		DebugAdd<SceneChangeComp>();
		DebugAdd<SpriteRenderComp>();
		DebugAdd<SpriteRenderDataComp>();
		DebugAdd<TransformComp>();
		DebugAdd<CannonComp>();
		DebugAdd<ItemComp>();
		DebugAdd<LoopCannonComp>();
		DebugAdd<PlayerComp>();
		DebugAdd<SailComp>();
		DebugAdd<UIDrawComp>();
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

void Object::Load(nlohmann::json& compData) {
	// コンポーネントを追加
	for (auto& comp : compData) {
		AddComps(comp);
	}

	for (auto& i : components_) {
		i.second->Load();
	}
}

void Object::AddComps(nlohmann::json& compData)
{
	std::string compName = compData["CompName"].get<std::string>();

	AddAndLoadComp<ButtonComp>(compName, compData);
	AddAndLoadComp<Camera2DComp>(compName, compData);
	AddAndLoadComp<Camera3DComp>(compName, compData);
	AddAndLoadComp<ChildrenObjectComp>(compName, compData);
	AddAndLoadComp<CsvDataComp>(compName, compData);
	AddAndLoadComp<JsonComp>(compName, compData);
	AddAndLoadComp<EventComp>(compName, compData);
	AddAndLoadComp<FallComp>(compName, compData);
	AddAndLoadComp<InputMoveComp>(compName, compData);
	AddAndLoadComp<ModelRenderComp>(compName, compData);
	AddAndLoadComp<ModelRenderDataComp>(compName, compData);
	AddAndLoadComp<ObbComp>(compName, compData);
	AddAndLoadComp<ObbPushComp>(compName, compData);
	AddAndLoadComp<QuadComp>(compName, compData);
	AddAndLoadComp<SceneChangeComp>(compName, compData);
	AddAndLoadComp<SpriteRenderComp>(compName, compData);
	AddAndLoadComp<SpriteRenderDataComp>(compName, compData);
	AddAndLoadComp<TransformComp>(compName, compData);
	AddAndLoadComp<CannonComp>(compName, compData);
	AddAndLoadComp<ItemComp>(compName, compData);
	AddAndLoadComp<LoopCannonComp>(compName, compData);
	AddAndLoadComp<PlayerComp>(compName, compData);
	AddAndLoadComp<SailComp>(compName, compData);
	AddAndLoadComp<UIDrawComp>(compName, compData);
	AddAndLoadComp<WhirlpoolsComp>(compName, compData);
	AddAndLoadComp<WindComp>(compName, compData);
	AddAndLoadComp<WindNodeComp>(compName, compData);
}

