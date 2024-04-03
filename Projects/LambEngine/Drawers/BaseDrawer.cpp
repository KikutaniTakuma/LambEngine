#include "BaseDrawer.h"

#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"

BaseDrawer::BaseDrawer():
	scale{Vector3::kIdentity},
	rotate{},
	translate{},
	color(std::numeric_limits<uint32_t>::max())
{}

void BaseDrawer::Draw(const Camera * camera, BlendType blend)
{
	RenderData* render = renderSet->GetRenderContext(blend);

	render->SetWVPMatrix({
		Mat4x4::MakeAffin(scale, rotate, translate),
		camera->GetViewProjection()
		}
	);

	render->SetColor(color);
}
