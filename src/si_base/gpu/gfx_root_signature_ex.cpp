
#include "si_base/core/core.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/gpu/gfx_device.h"
#include "si_base/memory/pool_allocator.h"

namespace SI
{
	void GfxDescriptorHeapTableEx::Terminate()
	{
		if(m_table.m_ranges)
		{
			SI_DEVICE_TEMP_ALLOCATOR().DeleteArrayRaw(m_table.m_ranges, m_table.m_rangeCount);
			m_table.m_ranges     = nullptr;
			m_table.m_rangeCount = 0;
			m_table.m_visibility = GfxShaderVisibility::All;
		}
	}

	void GfxDescriptorHeapTableEx::ReserveRanges(uint32_t rangeCount, GfxShaderVisibility visibility)
	{
		m_table.m_rangeCount = rangeCount;
		m_table.m_ranges = SI_DEVICE_TEMP_ALLOCATOR().NewArrayRaw<GfxDescriptorRange>((size_t)rangeCount);
		m_table.m_visibility = visibility;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	
	GfxRootSignatureDescEx::GfxRootSignatureDescEx(uint32_t tableCount)
		: m_tables(SI_DEVICE_TEMP_ALLOCATOR().NewArrayRaw<GfxDescriptorHeapTableEx>(tableCount))
		, m_tableCount(tableCount)
	{
		SI_ASSERT(m_tables);
		SI_ASSERT(0 < tableCount);
	}

	void GfxRootSignatureDescEx::Terminate()
	{
		if(m_tables)
		{
			auto& pool = SI_DEVICE_TEMP_ALLOCATOR();
			pool.DeleteArrayRaw(m_tables, m_tableCount);
			m_tables = nullptr;
			m_tableCount = 0;
		}
	}

	void GfxRootSignatureDescEx::ReserveTables(uint32_t tableCount)
	{
		Terminate();

		SI_ASSERT(0 < tableCount);
			
		auto& pool = SI_DEVICE_TEMP_ALLOCATOR();
		m_tables = pool.NewArrayRaw<GfxDescriptorHeapTableEx>(tableCount);
		m_tableCount = tableCount;
			
		SI_ASSERT(m_tables);
	}

	
	void GfxRootSignatureEx::Initialize(const GfxRootSignatureDesc& desc)
	{
		SI_ASSERT(!m_sig.IsValid());

		for(uint32_t i=0; i<desc.m_tableCount; ++i)
		{
			if(desc.m_tables[i].m_ranges==nullptr) continue;

			uint32_t rangeCount = desc.m_tables[i].m_rangeCount;
			if(rangeCount<=0) continue;

			const GfxDescriptorRange* ranges = (desc.m_tables[i].m_ranges);
			if(ranges[0].m_rangeType == GfxDescriptorRangeType::kSampler)
			{
                m_samplerTableBits |= ((uint64_t)1 << (uint64_t)i);
			}
			else
			{
                m_viewsTableBits |= ((uint64_t)1 << (uint64_t)i);
			}
			
			m_tableDescriptorCount[i] = 0;
			for(uint32_t j=0; j<rangeCount; ++j)
			{
				m_tableDescriptorCount[i] += ranges[j].m_descriptorCount;
			}
		}

		// TODO: 同じROOT SIGNATUREだったら再利用した方がよさげ.

		m_sig = SI_DEVICE().CreateRootSignature(desc);
		m_ref.Create();
	}

	void GfxRootSignatureEx::Terminate()
	{
		if(m_sig.IsValid())
		{
			if(m_ref.ReleaseRef() == 0)
			{
				// refCountが0の時のみ開放する.
				SI_DEVICE().ReleaseRootSignature(m_sig);
			}
			else
			{
				m_sig = GfxRootSignature();
			}
		}
	}

} // namespace SI
