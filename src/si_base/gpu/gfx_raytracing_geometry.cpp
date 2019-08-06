
#include "si_base/gpu/gfx_raytracing_geometry.h"

#include "si_base/gpu/dx12/dx12_raytracing_geometry.h"

namespace SI
{
	GfxBuffer GfxRaytracingScene::GetTopASBuffer()
	{
		return GfxBuffer(m_base->m_topLevelAccelerationStructure);
	}

	GfxBuffer GfxRaytracingScene::GetBottomASBuffer()
	{
		return GfxBuffer(m_base->m_bottomLevelAccelerationStructure);
	}

} // namespace SI
