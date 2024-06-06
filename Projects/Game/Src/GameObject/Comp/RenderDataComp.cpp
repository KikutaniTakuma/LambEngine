#include "RenderDataComp.h"

void RenderDataComp::Init()
{
	type = BlendType::kNormal;
	color = Vector4::kIdentity;
	isLighting = true;
}
