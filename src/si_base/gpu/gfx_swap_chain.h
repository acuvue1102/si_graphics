#pragma once

#include "gfx_descriptor_heap.h"

namespace SI
{
	class BaseSwapChain;
	class GfxCommandQueue;

	class GfxSwapChain
	{
	public:
		GfxSwapChain(BaseSwapChain* base=nullptr);
		~GfxSwapChain();

		int Present(uint32_t syncInterval);
		int Flip();

	public:
		GfxTexture GetSwapChainTexture();
		GfxCpuDescriptor GetSwapChainCpuDescriptor();
		

	public:
		BaseSwapChain* GetBaseSwapChain(){ return m_base; }
		const BaseSwapChain* GetBaseSwapChain() const{ return m_base; }

	private:
		BaseSwapChain* m_base;
	};

} // namespace SI