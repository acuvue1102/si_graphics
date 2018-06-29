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
	
	struct GfxDescriptorTableCache
	{
		uint64_t          m_assignedBits = 0;
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
			GfxDescriptorHeapType type,
			GfxDescriptor destStart, GfxCommandList* commandList, bool isGraphics);

		uint32_t ComputeDescriptorCount() const;

	private:
		uint64_t                 m_rootTableBits;
		uint64_t                 m_stageRootParamsBits;
		uint16_t                 m_maxDescriptorCount;
		GfxDescriptorTableCache  m_tableCache[kMaxNumDescriptorTables];
		GfxCpuDescriptor         m_descriptorCache[kMaxNumDescriptors];
	};

	class GfxDynamicDescriptorHeap
	{
	public:
		GfxDynamicDescriptorHeap();
		~GfxDynamicDescriptorHeap();

		void Initialize(GfxDescriptorHeapType descriptorType);
		void Terminate();
		
		void ParseGraphicsRootSignature( const GfxRootSignatureEx& rootSig );
		
		void SetGraphicsDescriptorHandles(
			uint32_t rootIndex, uint32_t offset,
			uint32_t descriptorCount, const GfxCpuDescriptor* descriptors);
		
		void CopyAndBindStaleTables(
			GfxGraphicsContext& context,
			GfxDescriptor destStart, GfxGraphicsCommandList* commandList);
		
		void RetireCurrentHeap();
		void SetupNewHeap();
		GfxDescriptor Allocate( uint32_t Count );

	private:
		GfxDescriptorHeapType    m_descriptorType;
		GfxDescriptorHandleCache m_graphicsDescriptorCache;
		
		GfxDescriptorHeap*       m_currentDescriptorHeap;
		GfxDescriptor            m_firstDescriptor;
		uint32_t                 m_currentOffset;
	};

} // namespace SI
