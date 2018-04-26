﻿#include "gpu/dx12/dx12_swap_chain.h"
#include "gpu/gfx_texture.h"
#include "gpu/gfx_command_queue.h"
#include "gpu/gfx_swap_chain.h"

namespace SI
{
	GfxSwapChain::GfxSwapChain(BaseSwapChain* base)
		: m_base(base)
	{
	}

	GfxSwapChain::~GfxSwapChain()
	{
	}

	int GfxSwapChain::Present(uint32_t syncInterval)
	{
		return m_base->Present(syncInterval);
	}
	
	int GfxSwapChain::Flip()
	{
		return m_base->Flip();
	}
	
	GfxTexture GfxSwapChain::GetSwapChainTexture()
	{
		GfxTexture tex(&m_base->GetSwapChainTexture());
		return tex;
	}

} // namespace SI
