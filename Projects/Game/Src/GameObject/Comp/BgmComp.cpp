#include "BgmComp.h"
#include "AudioComp.h"

void BgmComp::Init() {
	audioComp_ = object_.AddComp<AudioComp>();
}

void BgmComp::FirstUpdate() {
	if (audioComp_->GetAudio().have() and not audioComp_->GetAudio()->IsStart()) {
		audioComp_->GetAudio()->Start(audioComp_->volume, audioComp_->isLoop);
	}
}

void BgmComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
}

void BgmComp::Load([[maybe_unused]]nlohmann::json& json)
{
}
