#pragma once

#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseTexture;
	class BaseDescriptorHeap;

	struct GfxDescriptorHeapDesc
	{
		GfxDescriptorHeapType m_type            = kGfxDescriptorHeapType_CbvSrvUav;
		GfxDescriptorHeapFlag m_flag            = kGfxDescriptorHeapFlag_None;
		uint32_t              m_descriptorCount = 0;
	};

	struct GfxRenderTargetViewDesc
	{
	};

	struct GfxShaderResourceViewDesc
	{
		GfxDimension    m_srvDimension = kGfxDimension_Texture2D;
		GfxFormat       m_format       = kGfxFormat_R8G8B8A8_Unorm;
		uint32_t        m_miplevels    = 1;
		uint32_t        m_arraySize    = 1;
	};
	
	struct GfxCpuDescriptor
	{
		size_t m_ptr = 0;
	};
	struct GfxGpuDescriptor
	{
		size_t m_ptr = 0;
	};

	class GfxDescriptorHeap
	{
	public:
		GfxDescriptorHeap(BaseDescriptorHeap* base = nullptr)
			: m_base(base)
		{
		}

		~GfxDescriptorHeap()
		{
		}
		
		GfxCpuDescriptor GetCpuDescriptor(GfxDescriptorHeapType type, uint32_t descriptorIndex) const;
		GfxGpuDescriptor GetGpuDescriptor(GfxDescriptorHeapType type, uint32_t descriptorIndex) const;

	public:
		      BaseDescriptorHeap* GetBaseDescriptorHeap()      { return m_base; }
		const BaseDescriptorHeap* GetBaseDescriptorHeap() const{ return m_base; }

	private:
		BaseDescriptorHeap* m_base;
	};

} // namespace SI
