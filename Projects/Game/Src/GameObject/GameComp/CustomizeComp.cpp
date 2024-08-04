#include "CustomizeComp.h"
#include "../Comp/CsvDataComp.h"
#include "../Comp/ChildrenObjectComp.h"

void CustomizeComp::Save(nlohmann::json& json) {
	SaveCompName(json);
}

void CustomizeComp::Load([[maybe_unused]]nlohmann::json& json) {

}

void CustomizeComp::Init() {
	items_ = object_.AddComp<ChildrenObjectComp>();
	csvData_ = object_.AddComp<CsvDataComp>();
}

void CustomizeComp::Update()
{
}
