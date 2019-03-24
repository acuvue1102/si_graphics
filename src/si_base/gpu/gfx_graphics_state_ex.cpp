
#include "si_base/gpu/gfx_graphics_state_ex.h"

#include "si_base/gpu/gfx_device.h"

namespace SI
{
	GfxGraphicsStateEx::GfxGraphicsStateEx()
	{
	}

	GfxGraphicsStateEx::~GfxGraphicsStateEx()
	{
		Terminate();
		SI_ASSERT(!m_state.IsValid());
	}

	void GfxGraphicsStateEx::Initialize(const GfxGraphicsStateDesc& desc)
	{
		GfxDevice& device = *GfxDevice::GetInstance();
		m_state = device.CreateGraphicsState(desc);
	}

	void GfxGraphicsStateEx::Terminate()
	{
		GfxDevice& device = *GfxDevice::GetInstance();
		device.ReleaseGraphicsState(m_state);
	}

} // namespace SI
