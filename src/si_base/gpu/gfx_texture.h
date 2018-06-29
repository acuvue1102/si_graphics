#pragma once

#include <cstdint>
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseTexture;

	struct GfxTextureDesc
	{
		const char*        m_name           = nullptr;
		uint32_t           m_width          = 1;
		uint32_t           m_height         = 1;
		uint32_t           m_depth          = 1;
		uint32_t           m_mipLevels      = 1;
		GfxFormat          m_format         = GfxFormat::kR8G8B8A8_Unorm;
		GfxDimension       m_dimension      = GfxDimension::kTexture2D;
		GfxResourceStates  m_resourceStates = GfxResourceState::kCopyDest;
		GfxResourceFlags   m_resourceFlags  = GfxResourceFlag::kNone;
		GfxHeapType        m_heapType       = GfxHeapType::kDefault;
		float              m_clearColor[4]  = {0.0f, 0.0f, 0.0f, 0.0f};
		float              m_clearDepth     = 0.0f;
		uint8_t            m_clearStencil   = 0;
	};

	class GfxTexture
	{
	public:
		GfxTexture(BaseTexture* base = nullptr);
		~GfxTexture();
		
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;

		bool IsValid() const{ return (m_base!=nullptr); }

	public:
		BaseTexture*       GetBaseTexture()      { return m_base; }
		const BaseTexture* GetBaseTexture() const{ return m_base; }

	private:
		BaseTexture* m_base;
	};

} // namespace SI
