#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/renderer/renderer_graphics_state.h"

namespace SI
{
	class RenderItem
	{
	public:
		RenderItem()
			: m_submeshIndex(kInvalidObjectIndex)
		{}
		~RenderItem(){}

		void SetGraphicsState(const GfxGraphicsState& state) { m_graphicsState = state; }
		      GfxGraphicsState& GetGraphicsState()      { return m_graphicsState; }
		const GfxGraphicsState& GetGraphicsState() const{ return m_graphicsState; }
		
		void SetGraphicsStateDesc(const RendererGraphicsStateDesc& stateDesc) { m_graphicsStateDesc = stateDesc; }
		const RendererGraphicsStateDesc& GetGraphicsStateDesc() const{ return m_graphicsStateDesc; }

	private:
		ObjectIndex                m_submeshIndex;
		GfxGraphicsState           m_graphicsState;
		RendererGraphicsStateDesc  m_graphicsStateDesc;
	};

} // namespace SI
