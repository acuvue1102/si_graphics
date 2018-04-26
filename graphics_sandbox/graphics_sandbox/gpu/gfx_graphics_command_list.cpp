
#include "gpu/dx12/dx12_graphics_command_list.h"
#include "gpu/gfx_texture.h"
#include "gpu/gfx_graphics_state.h"
#include "gpu/gfx_graphics_command_list.h"

namespace SI
{
	GfxGraphicsCommandList::GfxGraphicsCommandList(BaseGraphicsCommandList* base)
		: m_base(base)
	{
	}

	GfxGraphicsCommandList::~GfxGraphicsCommandList()
	{
	}

	void GfxGraphicsCommandList::ClearRenderTarget(GfxTexture& tex, float r, float g, float b, float a)
	{
		m_base->ClearRenderTarget(*tex.GetBaseTexture(), r, g, b, a);
	}

	int GfxGraphicsCommandList::Reset(GfxGraphicsState& graphicsState)
	{
		return m_base->Reset(*graphicsState.GetBaseGraphicsState());
	}
	
	void GfxGraphicsCommandList::ResourceBarrier(
		GfxTexture& texture,
		const GfxResourceState& before,
		const GfxResourceState& after)
	{
		m_base->ResourceBarrier(
			*texture.GetBaseTexture(),
			before,
			after);
	}
	
	int GfxGraphicsCommandList::Close()
	{
		return m_base->Close();
	}
	
	BaseCommandList* GfxGraphicsCommandList::GetBaseCommandList()
	{
		return m_base;
	}

} // namespace SI
