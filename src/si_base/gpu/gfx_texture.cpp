
#include "si_base/gpu/dx12/dx12_texture.h"
#include "si_base/gpu/gfx_texture.h"

namespace SI
{
	GfxTexture::GfxTexture(BaseTexture* base)
		: m_base(base)
	{
	}

	GfxTexture::~GfxTexture()
	{
	}
	
	uint32_t GfxTexture::GetWidth() const
	{
		return m_base->GetWidth();
	}

	uint32_t GfxTexture::GetHeight() const
	{
		return m_base->GetHeight();
	}

	uint32_t GfxTexture::GetDepth() const
	{
		return m_base->GetDepth();
	}

	GfxFormat GfxTexture::GetFormat() const
	{
		return m_base->GetFormat();
	}

	uint32_t GfxTexture::GetArraySize() const
	{
		return m_base->GetArraySize();
	}

	uint32_t GfxTexture::GetMipLevels() const
	{
		return m_base->GetMipLevels();
	}
	
	void* GfxTexture::GetNativeResource()
	{
		return m_base->GetNativeResource();
	}
} // namespace SI
