#pragma once

#include "si_base/gpu/gfx_config.h"

namespace SI
{
	class BaseTexture;

	class GfxTexture
	{
	public:
		GfxTexture(BaseTexture* base = nullptr);
		~GfxTexture();
		
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;

	public:
		BaseTexture*       GetBaseTexture()      { return m_base; }
		const BaseTexture* GetBaseTexture() const{ return m_base; }

	private:
		BaseTexture* m_base;
	};

} // namespace SI
