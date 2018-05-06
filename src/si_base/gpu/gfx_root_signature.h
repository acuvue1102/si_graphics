#pragma once

#include <stdint.h>
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseRootSignature;

	struct GfxDescriptorRange
	{
		GfxDescriptorRangeType m_rangeType           = kGfxDescriptorRangeType_Srv;
		GfxDescriptorRangeFlag m_rangeFlag           = kGfxDescriptorRangeFlag_Static; // samplerの時は無視.
		uint32_t               m_descriptorCount     = 0;
		uint32_t               m_shaderRegisterIndex = 0;
	};
	
	struct GfxDescriptorHeapTable
	{
		GfxDescriptorRange*   m_ranges     = nullptr;
		uint32_t              m_rangeCount = 0;
	};

	struct GfxRootSignatureDesc
	{
		GfxDescriptorHeapTable* m_tables     = nullptr;
		uint32_t                m_tableCount = 0;
	};

	class GfxRootSignature
	{
	public:
		GfxRootSignature(BaseRootSignature* base=nullptr)
			: m_base(base)
		{
		}

		~GfxRootSignature()
		{
		}

	public:
		BaseRootSignature*       GetBaseRootSignature()      { return m_base; }
		const BaseRootSignature* GetBaseRootSignature() const{ return m_base; }

	private:
		BaseRootSignature* m_base;
	};
} // namespace SI
