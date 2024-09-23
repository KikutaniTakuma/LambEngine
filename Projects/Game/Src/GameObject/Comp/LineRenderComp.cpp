#include "LineRenderComp.h"
#include "LineRenderDataComp.h"
#include "LineComp.h"
#include "Drawers/Line/Line.h"
#include "CameraComp.h"

void LineRenderComp::Init() {
	lineRenderDataComp_ = object_.AddComp<LineRenderDataComp>();
	lineComp_ = object_.AddComp<LineComp>();
}

void LineRenderComp::Draw(CameraComp* camera) {
	Line::Draw(
		lineComp_->start,
		lineComp_->end,
		camera->GetCameraMatrix(),
		lineRenderDataComp_->color.GetColorRGBA(),
		lineRenderDataComp_->isDepth
	);
}

void LineRenderComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
}

void LineRenderComp::Load([[maybe_unused]]nlohmann::json& json) {

}


