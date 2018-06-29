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
			: m_rangeType          (GfxDescriptorRangeType::kSrv)
			, m_descriptorCount    (0)
			, m_shaderRegisterIndex(0)
			, m_rangeFlag          (GfxDescriptorRangeFlag::kStatic)
		{
		}

		GfxDescriptorRange(
			GfxDescriptorRangeType rangeType,
			uint32_t               descriptorCount,
			uint32_t               shaderRegisterIndex = 0,
			GfxDescriptorRangeFlag rangeFlag           = GfxDescriptorRangeFlag::kStatic)
			: m_rangeType          (rangeType)
			, m_descriptorCount    (descriptorCount)
			, m_shaderRegisterIndex(shaderRegisterIndex)
			, m_rangeFlag          (rangeFlag)
		{
		}
		
		void Set(
			GfxDescriptorRangeType rangeType,
			uint32_t               descriptorCount,
			uint32_t               shaderRegisterIndex = 0,
			GfxDescriptorRangeFlag rangeFlag           = GfxDescriptorRangeFlag::kStatic)
		{
			m_rangeType           = rangeType;
			m_descriptorCount     = descriptorCount;
			m_shaderRegisterIndex = shaderRegisterIndex;
			m_rangeFlag           = rangeFlag;
		}

	public:
		GfxDescriptorRangeType m_rangeType;
		uint32_t               m_descriptorCount;
		uint32_t               m_shaderRegisterIndex;
		GfxDescriptorRangeFlag m_rangeFlag;
	};
	
	struct GfxDescriptorHeapTable
	{
		GfxDescriptorRange*   m_ranges     = nullptr;
		uint32_t              m_rangeCount = 0;
		GfxShaderVisibility   m_visibility = GfxShaderVisibility::All;
	};

	struct GfxRootSignatureDesc
	{
		GfxDescriptorHeapTable* m_tables;
		uint32_t                m_tableCount;

		GfxRootSignatureDesc()
			: m_tables(nullptr)
			, m_tableCount(0)
		{
		}

		GfxRootSignatureDesc(
			GfxDescriptorHeapTable* tables,
			uint32_t                tableCount)
			: m_tables(tables)
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
