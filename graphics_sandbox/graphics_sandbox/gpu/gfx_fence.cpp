#include "gfx_fence.h"

namespace SI
{

	GfxFenceEvent::GfxFenceEvent(BaseFenceEvent* base)
		: m_base(base)
	{
	}

	GfxFenceEvent::~GfxFenceEvent()
	{
	}
	
	//////////////////////////////////////////////////////////////////////////

	GfxFence::GfxFence(BaseFence* base)
		: m_base(base)
	{
	}

	GfxFence::~GfxFence()
	{
	}

} // namespace SI

