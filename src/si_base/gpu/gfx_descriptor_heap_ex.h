#pragma once

#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/misc/reference_counter.h"

namespace SI
{
	class GfxDescriptorHeapEx
	{
	public:
		GfxDescriptorHeapEx();
		~GfxDescriptorHeapEx();
		
		void Initialize(const GfxDescriptorHeapDesc& desc);
		void InitializeAsCbvSrvUav(uint32_t descriptorCount, GfxDescriptorHeapFlag flag = GfxDescriptorHeapFlag::ShaderVisible);
		void InitializeAsSampler(uint32_t samplerCount, GfxDescriptorHeapFlag flag = GfxDescriptorHeapFlag::ShaderVisible);
		void Terminate();

		void SetShaderResourceView(uint32_t index, GfxTexture& texture, const GfxShaderResourceViewDesc& srvDesc);
		void SetShaderResourceView(uint32_t index, GfxBuffer& buffer, const GfxShaderResourceViewDesc& srvDesc);
		void SetUnorderedAccessView(uint32_t index, GfxTexture& texture, const GfxUnorderedAccessViewDesc& uavDesc);
		void SetSampler(uint32_t index, const GfxSamplerDesc& samplerDesc);
		
		      GfxDescriptorHeap& Get()      { return m_descriptorHeap; }
		const GfxDescriptorHeap& Get() const{ return m_descriptorHeap; }

		bool IsValid() const{ return Get().IsValid(); }

	private:
		GfxDescriptorHeap m_descriptorHeap;
		ReferenceCounter  m_ref;
	};

} // namespace SI
