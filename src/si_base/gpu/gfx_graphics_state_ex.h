#pragma once

#include "si_base/gpu/gfx_graphics_state.h"

namespace SI
{
	class GfxGraphicsStateEx
	{
	public:
		GfxGraphicsStateEx();
		~GfxGraphicsStateEx();

		void Initialize(const GfxGraphicsStateDesc& desc);
		void Terminate();

		GfxGraphicsState& Get(){ return m_state; }
		const GfxGraphicsState& Get() const{ return m_state; }

	private:
		GfxGraphicsState m_state;
	};

} // namespace SI
