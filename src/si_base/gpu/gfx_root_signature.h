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
	
	struct GfxRootDescriptor
	{
		uint32_t                m_shaderRegisterIndex = 0;
		uint32_t                m_registerSpace       = 0;
		GfxRootDescriptorFlags  m_flags               = GfxRootDescriptorFlag::None;
		GfxShaderVisibility     m_visibility          = GfxShaderVisibility::All;
		GfxRootDescriptorType   m_type                = GfxRootDescriptorType::CBV;

		void InitAsSRV(
			uint32_t shaderRegisterIndex,
			uint32_t registerSpace = 0,
			GfxShaderVisibility visibility = GfxShaderVisibility::All,
			GfxRootDescriptorFlags  flags = GfxRootDescriptorFlag::None)
		{
			m_shaderRegisterIndex = shaderRegisterIndex;
			m_registerSpace       = registerSpace;
			m_flags               = flags;
			m_visibility          = visibility;
			m_type                = GfxRootDescriptorType::SRV;
		}
	};

	struct GfxRootSignatureDesc
	{
		const char*             m_name;
		GfxDescriptorHeapTable* m_tables;
		uint32_t                m_tableCount;
		GfxRootDescriptor*      m_rootDescriptors;
		uint32_t                m_rootDescriptorCount; // rootに入れるdescriptor.
		GfxRootSignatureFlags   m_flags;

		GfxRootSignatureDesc()
			: m_name(nullptr)
			, m_tables(nullptr)
			, m_tableCount(0)
			, m_rootDescriptors(nullptr)
			, m_rootDescriptorCount(0)
			, m_flags(GfxRootSignatureFlag::AllowInputAssemblerInputLayout)
		{
		}

		GfxRootSignatureDesc(
			const char*             name,
			GfxDescriptorHeapTable* tables,
			uint32_t                tableCount,
			GfxRootDescriptor*      rootDescriptors,
			uint32_t                rootDescriptorCount,
			GfxRootSignatureFlags   flags = GfxRootSignatureFlag::AllowInputAssemblerInputLayout)
			: m_name(name)
			, m_tables(tables)
			, m_tableCount(tableCount)
			, m_rootDescriptors(rootDescriptors)
			, m_rootDescriptorCount(rootDescriptorCount)
			, m_flags(flags)
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


		void* GetNative();

	public:
		BaseRootSignature*       GetBaseRootSignature()      { return m_base; }
		const BaseRootSignature* GetBaseRootSignature() const{ return m_base; }

	private:
		BaseRootSignature* m_base;
	};
} // namespace SI
