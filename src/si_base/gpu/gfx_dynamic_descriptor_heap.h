#pragma once

#include "si_base/core/assert.h"
#include "si_base/core/constant.h"
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/memory/object_pool.h"
#include "si_base/concurency/mutex.h"

namespace SI
{
	class GfxRootSignatureEx;
	struct GfxCpuDescriptor;
	class GfxCommandList;
	class GfxGraphicsContext;
	class GfxGraphicsCommandList;
	class BaseCommandList;
	
	struct GfxDescriptorTableCache
	{
		uint64_t          m_assignedBits = 0; // 使われているDescriptorのIndexのBits
		GfxCpuDescriptor* m_descriptorStart = nullptr;
		uint16_t          m_descriptorCount  = 0;
	};

	class GfxDescriptorHandleCache
	{
	public:
		GfxDescriptorHandleCache();
		~GfxDescriptorHandleCache();
		
		void UnbindAllValid();
		void StageDescriptorHandles(
			uint32_t rootIndex, uint32_t offset,
			uint32_t descriptorCount, const GfxCpuDescriptor* descriptors );
		void ParseRootSignature(GfxDescriptorHeapType type, const GfxRootSignatureEx& rootSig);
				
		void CopyAndBindTables(
			GfxGraphicsContext& context,
			GfxDescriptorHeapType type,
			GfxDescriptor destStart);

		uint32_t ComputeDescriptorCount() const;
		
		uint64_t  GetRootTableBits()      const{ return m_rootTableBits;       }
		uint64_t  GetStageRootParamBits() const{ return m_stageRootParamsBits; }
		uint16_t  GetMaxDescriptorCount() const{ return m_maxDescriptorCount;  }

		void SetIsGraphicsCache(bool isGraphics){ m_isGraphicsCache = isGraphics; }
		bool GetIsGraphicsCache()          const{ return m_isGraphicsCache;       }

	private:
		uint64_t                 m_rootTableBits;         // root signatureから得られるDescriptorTableのIndexを示すBit
		uint64_t                 m_stageRootParamsBits;   // dynamic descriptorがセットされたIndexを示すのbit
		uint16_t                 m_maxDescriptorCount;
		GfxDescriptorTableCache  m_tableCache[kMaxNumDescriptorTables];
		GfxCpuDescriptor         m_descriptorCache[kMaxNumDescriptors];
		bool                     m_isGraphicsCache;
	};

	class GfxDynamicDescriptorHeap
	{
	public:
		GfxDynamicDescriptorHeap();
		~GfxDynamicDescriptorHeap();

		void Initialize(GfxDescriptorHeapType descriptorType);
		void Terminate();

		void Reset();
		
		void ParseGraphicsRootSignature( const GfxRootSignatureEx& rootSig );
		
		void ParseComputeRootSignature( const GfxRootSignatureEx& rootSig );
		
		void SetGraphicsDescriptorHandles(
			uint32_t rootIndex, uint32_t offset,
			uint32_t descriptorCount, const GfxCpuDescriptor* descriptors);
		
		void SetComputeDescriptorHandles(
			uint32_t rootIndex, uint32_t offset,
			uint32_t descriptorCount, const GfxCpuDescriptor* descriptors);
				
		void CopyAndBindGraphicsTables(GfxGraphicsContext& context);
		void CopyAndBindComputeTables (GfxGraphicsContext& context);
		void CopyAndBindTables(GfxGraphicsContext& context, GfxDescriptorHandleCache& descriptorCache);
		
		void RetireCurrentHeap();
		void SetupNewHeap();
		GfxDescriptor Allocate( uint32_t Count, GfxDescriptorHandleCache& descriptorCache );

	private:
		GfxDescriptorHeapType    m_descriptorType;
		GfxDescriptorHandleCache m_graphicsDescriptorCache;
		GfxDescriptorHandleCache m_computeDescriptorCache;
		
		GfxDescriptorHeap*       m_currentDescriptorHeap;
		GfxDescriptor            m_firstDescriptor;
		uint32_t                 m_currentOffset;
	};

} // namespace SI
