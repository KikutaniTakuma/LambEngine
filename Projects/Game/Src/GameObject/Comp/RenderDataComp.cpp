#include "RenderDataComp.h"

void RenderDataComp::Init()
{
	type = BlendType::kNone;
	color = Vector4::kIdentity;
	isLighting = true;
}
