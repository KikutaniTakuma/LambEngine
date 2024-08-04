#include "CustomizeComp.h"
#include "../Comp/CsvDataComp.h"
#include "../Comp/JsonComp.h"
#include "../Comp/ChildrenObjectComp.h"
#include "../Comp/ModelRenderComp.h"
#include "../Comp/ModelRenderDataComp.h"

void CustomizeComp::Save(nlohmann::json& json) {
	SaveCompName(json);
}

void CustomizeComp::Load([[maybe_unused]]nlohmann::json& json) {

}

void CustomizeComp::Load()
{
	csvData_->Load();
	jsonData_->Load();
	auto& jsonData = jsonData_->GetJsonData();

	maxCannonNum_ = jsonData["Cannon"].get<int32_t>();
	maxLoopCannonNum_ = jsonData["LoopCannon"].get<int32_t>();
	maxSailNum_ = jsonData["Sail"].get<int32_t>();

	cannonComps_.reserve(maxCannonNum_);
	loopCannonComps_.reserve(maxLoopCannonNum_);
	sailComps_.reserve(maxSailNum_);

	items_->ClearObjects();

	/*for (auto& y : csvData_->GetCsvData()) {
		for (auto& x : y) {
			if (x == 1) {
				Lamb::SafePtr addObject = Lamb::MakeSafePtr<Object>();
				Lamb::SafePtr box = addObject->AddComp<ModelRenderComp>();
				Lamb::SafePtr dataComp = addObject->GetComp<ModelRenderDataComp>();
				dataComp->fileName = "./"
				
				items_->AddObject();
			}
		}
	}*/
}

void CustomizeComp::Init() {
	items_ = object_.AddComp<ChildrenObjectComp>();
	csvData_ = object_.AddComp<CsvDataComp>();
	jsonData_ = object_.AddComp<JsonComp>();
}

void CustomizeComp::Update() {

}
