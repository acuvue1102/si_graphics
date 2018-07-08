#pragma once

#include <cstdint>
#include "si_base/gpu/gfx_enum.h"
#include "si_base/core/assert.h"

namespace SI
{
	class BaseRootSignature;

	class GfxDescriptorRange
	{
	public:
		GfxDescriptorRange()
			: m_rangeType          (GfxDescriptorRangeType::Srv)
			, m_descriptorCount    (0)
			, m_shaderRegisterIndex(0)
			, m_rangeFlags         (GfxDescriptorRangeFlag::None)
		{
		}

		GfxDescriptorRange(
			GfxDescriptorRangeType  rangeType,
			uint32_t                descriptorCount,
			uint32_t                shaderRegisterIndex = 0,
			GfxDescriptorRangeFlags rangeFlags           = GfxDescriptorRangeFlag::None)
			: m_rangeType          (rangeType)
			, m_descriptorCount    (descriptorCount)
			, m_shaderRegisterIndex(shaderRegisterIndex)
			, m_rangeFlags         (rangeFlags)
		{
		}
		
		void Set(
			GfxDescriptorRangeType  rangeType,
			uint32_t                descriptorCount,
			uint32_t                shaderRegisterIndex = 0,
			GfxDescriptorRangeFlags rangeFlags          = GfxDescriptorRangeFlag::None)
		{
			m_rangeType           = rangeType;
			m_descriptorCount     = descriptorCount;
			m_shaderRegisterIndex = shaderRegisterIndex;
			m_rangeFlags          = rangeFlags;
		}

	public:
		GfxDescriptorRangeType   m_rangeType;
		uint32_t                 m_descriptorCount;
		uint32_t                 m_shaderRegisterIndex;
		GfxDescriptorRangeFlags  m_rangeFlags;
	};
	
	struct GfxDescriptorHeapTable
	{
		GfxDescriptorRange*   m_ranges     = nullptr;
		uint32_t              m_rangeCount = 0;
		GfxShaderVisibility   m_visibility = GfxShaderVisibility::All;
	};

	struct GfxRootSignatureDesc
	{
		const char*             m_name;
		GfxDescriptorHeapTable* m_tables;
		uint32_t                m_tableCount;

		GfxRootSignatureDesc()
			: m_name(nullptr)
			, m_tables(nullptr)
			, m_tableCount(0)
		{
		}

		GfxRootSignatureDesc(
			const char*             name,
			GfxDescriptorHeapTable* tables,
			uint32_t                tableCount)
			: m_name(name)
			, m_tables(tables)
			, m_tableCount(tableCount)
		{
		}
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

		bool IsValid() const{ return (m_base!=nullptr); }

	public:
		BaseRootSignature*       GetBaseRootSignature()      { return m_base; }
		const BaseRootSignature* GetBaseRootSignature() const{ return m_base; }

	private:
		BaseRootSignature* m_base;
	};
} // namespace SI
