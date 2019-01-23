#pragma once

#include "si_base/container/array.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/gpu/gfx_graphics_state.h"

namespace SI
{
	class RenderItem
	{
	public:
		RenderItem();
		~RenderItem();
		
		void SetRootSignature(const GfxRootSignature& sig) { m_rootSignature = sig; }
		GfxRootSignature&         GetRootSignature(){ return m_rootSignature; }
		GfxGraphicsState&         GetGraphicsState(){ return m_graphicsState; }

	private:
		GfxRootSignature         m_rootSignature;
		GfxGraphicsState         m_graphicsState;
	};

} // namespace SI
