#pragma once

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
		

	public:
		BaseSwapChain* GetBaseSwapChain(){ return m_base; }
		const BaseSwapChain* GetBaseSwapChain() const{ return m_base; }

	private:
		BaseSwapChain* m_base;
	};

} // namespace SI