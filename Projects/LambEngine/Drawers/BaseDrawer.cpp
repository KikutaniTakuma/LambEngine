#include "BaseDrawer.h"
#include "Utils/SafePtr.h"

#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"

#include "Utils/EngineInfo.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


BaseDrawer::BaseDrawer() :
	renderSet(nullptr),
	isUseMeshShader_(Lamb::IsCanUseMeshShader())
{}

void BaseDrawer::Draw(const Mat4x4& worldMatrix, const Mat4x4& camera, uint32_t color, BlendType blend)
{
	Lamb::SafePtr<RenderData> render;
	if (isUseMeshShader_ and meshRenderSet) {
		render = meshRenderSet->GetRenderContext(blend);
	}
	else {
		render = renderSet->GetRenderContext(blend);
	}

	render->SetWVPMatrix({
		.worldMat = worldMatrix,
		.cameraMat = camera
		}
	);

	render->SetColor(color);

	render->AddDrawCount();
}
