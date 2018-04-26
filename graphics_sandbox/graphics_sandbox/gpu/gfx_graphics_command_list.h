#pragma once

#include "gpu/gfx_config.h"
#include "gfx_command_list.h"

namespace SI
{
	class BaseCommandList;
	class BaseGraphicsCommandList;
	class GfxTexture;
	class GfxGraphicsState;
	class GfxResourceState;

	class GfxGraphicsCommandList : public GfxCommandList
	{
	public:
		GfxGraphicsCommandList(BaseGraphicsCommandList* base = nullptr);
		virtual ~GfxGraphicsCommandList();

		void ClearRenderTarget(GfxTexture& tex, float r, float g, float b, float a);
		
		int Reset(GfxGraphicsState& graphicsState);
		
		void ResourceBarrier(
			GfxTexture& texture,
			const GfxResourceState& before,
			const GfxResourceState& after);

		int Close();

	public:
		BaseGraphicsCommandList* GetBaseGraphicsCommandList(){ return m_base; }
		const BaseGraphicsCommandList* GetBaseGraphicsCommandList() const{ return m_base; }
		BaseCommandList* GetBaseCommandList() override;

	private:
		BaseGraphicsCommandList* m_base;
	};

} // namespace SI
