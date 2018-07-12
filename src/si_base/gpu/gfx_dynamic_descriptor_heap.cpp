
#include "si_base/gpu/gfx_dynamic_descriptor_heap.h"

#include "si_base/misc/bitwise.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/gpu/gfx_graphics_command_list.h"
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/gfx_core.h"
#include "si_base/gpu/gfx_graphics_context.h"

namespace SI
{
	GfxDescriptorHandleCache::GfxDescriptorHandleCache()
		: m_rootTableBits(0)
		, m_maxDescriptorCount(0)
	{
	}

	GfxDescriptorHandleCache::~GfxDescriptorHandleCache()
	{
	}

	void GfxDescriptorHandleCache::Reset()
	{
		m_rootTableBits = 0;
		m_stageRootParamsBits = 0;
		m_maxDescriptorCount = 0;
	}
	
	void GfxDescriptorHandleCache::UnbindAllValid()
	{
		m_stageRootParamsBits = 0;

		uint64_t tableBits = m_rootTableBits;
		while (tableBits != 0)
		{
			int rootIndex = Bitwise::LSB64(tableBits);
			SI_ASSERT(0<=rootIndex);

			tableBits ^= (1ULL << (uint64_t)rootIndex);

			if (m_tableCache[rootIndex].m_assignedBits != 0)
			{
				m_stageRootParamsBits |= (1ULL << (uint64_t)rootIndex);
			}
		}
	}

	void GfxDescriptorHandleCache::StageDescriptorHandles(
		uint32_t rootIndex, uint32_t offset,
		uint32_t descriptorCount, const GfxCpuDescriptor* descriptors)
	{
		SI_ASSERT((1ULL<<(uint64_t)rootIndex) & m_rootTableBits);
		SI_ASSERT(offset + descriptorCount <= m_tableCache[rootIndex].m_descriptorCount);
		SI_ASSERT(offset + descriptorCount <= 64, "assinedBitsのbit数を超えている");

		GfxDescriptorTableCache& tableCache = m_tableCache[rootIndex];
		for(uint32_t i=0; i<descriptorCount; ++i)
		{
			tableCache.m_descriptorStart[i+offset] = descriptors[i];
		}

		tableCache.m_assignedBits |= ((1ULL<<(uint64_t)descriptorCount)-1ULL) << (uint64_t)offset;
		m_stageRootParamsBits     |= (1ULL<<(uint64_t)rootIndex);
	}
	
	void GfxDescriptorHandleCache::ParseRootSignature(
		GfxDescriptorHeapType type, const GfxRootSignatureEx& rootSig)
	{
		m_rootTableBits = (type==GfxDescriptorHeapType::Sampler)?
			rootSig.GetSamplerTableBits() : rootSig.GetViewsTableBits();
		
		uint16_t currentOffset = 0;
		uint64_t tableBits = m_rootTableBits;
		while(tableBits != 0)
		{
			int rootIndex = Bitwise::LSB64(tableBits);
			SI_ASSERT(0 <= rootIndex);
			
			tableBits ^= (1ULL<<(uint64_t)rootIndex);
			
			uint16_t descCount = rootSig.GetTableDesciptorCount(rootIndex);
			SI_ASSERT(0 < descCount);
			SI_ASSERT(descCount <= 64, "TableCacheのassinedBitsの拡張が必要");
			
			GfxDescriptorTableCache& tableCache = m_tableCache[rootIndex];
			tableCache.m_assignedBits = 0;
			tableCache.m_descriptorStart = &m_descriptorCache[currentOffset];
			tableCache.m_descriptorCount  = descCount;

			currentOffset += descCount;
		}

		m_maxDescriptorCount = currentOffset;
	}
	
	void GfxDescriptorHandleCache::CopyAndBindTables(
		GfxGraphicsContext& context,
		GfxDescriptorHeapType type,
		GfxDescriptor destStart,
		bool isGraphicsMode)
	{
		uint32_t descriptorSize = (uint32_t)SI_BASE_DEVICE().GetDescriptorSize(type);

		uint32_t paramCount = 0;
		uint32_t tableSize[kMaxNumDescriptorTables];
		uint32_t rootIndices[kMaxNumDescriptorTables];

		uint64_t paramBits = m_stageRootParamsBits;
		while(paramBits != 0)
		{
			int rootIndex = Bitwise::LSB64(paramBits);
			SI_ASSERT(0 <= rootIndex);
			SI_ASSERT(rootIndex < kMaxNumDescriptorTables);

			rootIndices[paramCount] = (uint32_t)rootIndex;
			paramBits ^= (1ULL << (uint64_t)rootIndex);

			SI_ASSERT(m_tableCache[rootIndex].m_assignedBits != 0);

			uint32_t maxSetHandle = Bitwise::MSB64(m_tableCache[rootIndex].m_assignedBits);

			tableSize[paramCount] = maxSetHandle + 1;

			++paramCount;
		}

		m_stageRootParamsBits = 0;

		uint32_t         dstDescriptorCount = 0;
		GfxCpuDescriptor dstDescriptorStarts[kMaxNumDescriptors];
		uint32_t         dstDescriptorSizes [kMaxNumDescriptors];

		uint32_t         srcDescriptorCount = 0;
		GfxCpuDescriptor srcDescriptorStarts[kMaxNumDescriptors];
		uint32_t         srcDescriptorSizes [kMaxNumDescriptors];

		for (uint32_t i=0; i<paramCount; ++i)
		{
			uint32_t rootIndex = rootIndices[i];

			GfxDescriptorTableCache& rootDescTable = m_tableCache[rootIndex];

			GfxCpuDescriptor* srcDescStart   = rootDescTable.m_descriptorStart;			
			uint64_t          srcAssignedBit = rootDescTable.m_assignedBits;

			GfxCpuDescriptor curDesc = destStart.GetCpuDescriptor();

			while (srcAssignedBit != 0)
			{
				int skipCount = Bitwise::LSB64(srcAssignedBit);
				srcAssignedBit >>= (uint32_t)skipCount;

				srcDescStart += (uint32_t)skipCount;
				curDesc.m_ptr += (uint32_t)skipCount * descriptorSize;

				int descriptorCount = Bitwise::LSB64(~srcAssignedBit);
				SI_ASSERT(0<=descriptorCount);
				srcAssignedBit >>= descriptorCount;

				dstDescriptorStarts[dstDescriptorCount] = curDesc;
				dstDescriptorSizes [dstDescriptorCount] = descriptorCount;
				++dstDescriptorCount;

				for (int j=0; j<descriptorCount; ++j)
				{
					srcDescriptorStarts[srcDescriptorCount] = srcDescStart[j];
					srcDescriptorSizes [srcDescriptorCount] = 1;
					++srcDescriptorCount;
				}
				srcDescStart  += descriptorCount;
				curDesc.m_ptr += descriptorCount * descriptorSize;
			}
			
			if(isGraphicsMode)
			{
				context.GetBaseGraphicsCommandList()->SetGraphicsDescriptorTable(
					rootIndex, destStart.GetGpuDescriptor());
			}
			else
			{
				context.GetBaseGraphicsCommandList()->SetComputeDescriptorTable(
					rootIndex, destStart.GetGpuDescriptor());
			}
			
			destStart += tableSize[i] * descriptorSize;
		}

		SI_BASE_DEVICE().CopyDescriptors(
			dstDescriptorCount, dstDescriptorStarts, dstDescriptorSizes,
			srcDescriptorCount, srcDescriptorStarts, srcDescriptorSizes,
			type);
	}
	
	uint32_t GfxDescriptorHandleCache::ComputeDescriptorCount() const
	{
		uint32_t neededSpace = 0;
		uint64_t paramsBits = m_stageRootParamsBits;
		while(paramsBits != 0)
		{
			int rootIndex = Bitwise::LSB64(paramsBits);
			SI_ASSERT(0 <= rootIndex);
			paramsBits ^= (1ULL << (uint64_t)rootIndex);

			uint64_t assinedBits = m_tableCache[rootIndex].m_assignedBits;
			SI_ASSERT(assinedBits != 0);
			int maxSetHandle = Bitwise::MSB64(assinedBits);
			SI_ASSERT(0<=maxSetHandle);

			neededSpace += (uint32_t)maxSetHandle + 1;
		}

		return neededSpace;
	}

	/////////////////////////////////////////////////////////

	GfxDynamicDescriptorHeap::GfxDynamicDescriptorHeap()
		: m_descriptorType(GfxDescriptorHeapType::Max)
		, m_currentDescriptorHeap(nullptr)
		, m_currentOffset(0)
		, m_isGraphicsMode(true)
	{
	}

	GfxDynamicDescriptorHeap::~GfxDynamicDescriptorHeap()
	{
	}
	
	void GfxDynamicDescriptorHeap::Initialize(GfxDescriptorHeapType descriptorType)
	{
		SI_ASSERT(descriptorType==GfxDescriptorHeapType::CbvSrvUav || descriptorType==GfxDescriptorHeapType::Sampler);
		m_descriptorType = descriptorType;
	}

	void GfxDynamicDescriptorHeap::Terminate()
	{
		RetireCurrentHeap();
		m_descriptorType = GfxDescriptorHeapType::Max;
	}
	
	void GfxDynamicDescriptorHeap::Reset()
	{
		m_descriptorCache.Reset();
		
		RetireCurrentHeap();
	}

	void GfxDynamicDescriptorHeap::ParseGraphicsRootSignature(const GfxRootSignatureEx& rootSig)
	{
		SI_ASSERT(m_descriptorType != GfxDescriptorHeapType::Max);
		m_descriptorCache.ParseRootSignature(m_descriptorType, rootSig);
		
		m_isGraphicsMode = true;
	}

	void GfxDynamicDescriptorHeap::ParseComputeRootSignature(const GfxRootSignatureEx& rootSig)
	{
		SI_ASSERT(m_descriptorType != GfxDescriptorHeapType::Max);
		m_descriptorCache.ParseRootSignature(m_descriptorType, rootSig);
		
		m_isGraphicsMode = false;
	}

	void GfxDynamicDescriptorHeap::SetDescriptorHandles(
		uint32_t rootIndex, uint32_t offset,
		uint32_t descriptorCount, const GfxCpuDescriptor* descriptors)
	{
		m_descriptorCache.StageDescriptorHandles(rootIndex, offset, descriptorCount, descriptors);
	}
	
	void GfxDynamicDescriptorHeap::CopyAndBindTables(
		GfxGraphicsContext& context)
	{
		if (m_descriptorCache.GetStageRootParamBits() == 0) return;

		uint32_t descriptorCount = m_descriptorCache.ComputeDescriptorCount();
		GfxDescriptor newDescriptor = Allocate(descriptorCount, m_descriptorCache);
		
		if(m_descriptorType == GfxDescriptorHeapType::Sampler)
		{
			context.SetSamplerDescriptorHeap(m_currentDescriptorHeap);
		}
		else
		{
			context.SetCbvSrvUavDescriptorHeap(m_currentDescriptorHeap);
		}

		m_descriptorCache.CopyAndBindTables(context, m_descriptorType, newDescriptor, m_isGraphicsMode);
	}
	
	void GfxDynamicDescriptorHeap::RetireCurrentHeap()
	{
		if (m_currentDescriptorHeap == nullptr) return;
		
		if(m_descriptorType == GfxDescriptorHeapType::Sampler)
		{
			SI_SAMPLER_DESCRIPTOR_HEAP_POOL().Deallocate(m_currentDescriptorHeap);
		}
		else
		{
			SI_VIEW_DESCRIPTOR_HEAP_POOL().Deallocate(m_currentDescriptorHeap);
		}
		
		m_currentDescriptorHeap = nullptr;
		m_firstDescriptor = GfxDescriptor();
		m_currentOffset = 0;
	}
	
	void GfxDynamicDescriptorHeap::SetupNewHeap()
	{
		RetireCurrentHeap();

		if(m_descriptorType == GfxDescriptorHeapType::Sampler)
		{
			m_currentDescriptorHeap = SI_SAMPLER_DESCRIPTOR_HEAP_POOL().Allocate();
		}
		else
		{
			m_currentDescriptorHeap = SI_VIEW_DESCRIPTOR_HEAP_POOL().Allocate();
		}

		m_firstDescriptor = GfxDescriptor(
			m_currentDescriptorHeap->GetCpuDescriptor(0),
			m_currentDescriptorHeap->GetGpuDescriptor(0));
			
		m_currentOffset = 0;
	}

	GfxDescriptor GfxDynamicDescriptorHeap::Allocate(
		uint32_t count,
		GfxDescriptorHandleCache& descriptorCache )
	{
		uint32_t descriptorSize = (uint32_t)SI_BASE_DEVICE().GetDescriptorSize(m_descriptorType);
		if( m_currentDescriptorHeap == nullptr ||
			kMaxNumDescriptors < (m_currentOffset + count))
		{
			SetupNewHeap();
			descriptorCache.UnbindAllValid();
		}
		
		GfxDescriptor ret = m_firstDescriptor + m_currentOffset * descriptorSize;
		m_currentOffset += count;
		return ret;
	}

} // namespace SI
