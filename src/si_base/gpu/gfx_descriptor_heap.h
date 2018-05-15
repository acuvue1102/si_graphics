﻿#pragma once

#include "si_base/core/constant.h"
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseDescriptorHeap;
	class GfxBuffer;

	struct GfxDescriptorHeapDesc
	{
		GfxDescriptorHeapType m_type            = GfxDescriptorHeapType::kCbvSrvUav;
		GfxDescriptorHeapFlag m_flag            = GfxDescriptorHeapFlag::kNone;
		uint32_t              m_descriptorCount = 0;
	};

	struct GfxRenderTargetViewDesc
	{
	};

	struct GfxDepthStencilViewDesc
	{
	};

	struct GfxShaderResourceViewDesc
	{
		GfxDimension    m_srvDimension = GfxDimension::kTexture2D;
		GfxFormat       m_format       = GfxFormat::kR8G8B8A8_Unorm;
		uint32_t        m_miplevels    = 1;
		uint32_t        m_arraySize    = 1;
	};

	struct GfxSamplerDesc
	{
		GfxFilter           m_filter         = GfxFilter::kMinMagMipPoint;
		GfxTextureAddress   m_addressU       = GfxTextureAddress::kWrap;
		GfxTextureAddress   m_addressV       = GfxTextureAddress::kWrap;
		GfxTextureAddress   m_addressW       = GfxTextureAddress::kWrap;
		float               m_mipLODBias     = 0.0f;
		uint32_t            m_maxAnisotropy  = 1;
		GfxComparisonFunc   m_comparisonFunc = GfxComparisonFunc::kLess;
		float               m_borderColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		float               m_minLOD         = 0.0f;
		float               m_maxLOD         = kFltMax;
	};

	struct GfxConstantBufferViewDesc
	{
		GfxBuffer*          m_buffer = nullptr;
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
		
		GfxCpuDescriptor GetCpuDescriptor(uint32_t descriptorIndex) const;
		GfxGpuDescriptor GetGpuDescriptor(uint32_t descriptorIndex) const;

	public:
		      BaseDescriptorHeap* GetBaseDescriptorHeap()      { return m_base; }
		const BaseDescriptorHeap* GetBaseDescriptorHeap() const{ return m_base; }

	private:
		BaseDescriptorHeap* m_base;
	};

} // namespace SI
