#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/gfx_descriptor_heap.h"

namespace SI
{
	struct GfxDescriptorHeapDesc;

	// DescriptorHeap
	class BaseDescriptorHeap
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		BaseDescriptorHeap();
		~BaseDescriptorHeap();

		int Initialize(			
			ID3D12Device& device,
			const GfxDescriptorHeapDesc& desc);

		int Terminate();

		void CreateRenderTargetView(
			ID3D12Device& device,
			uint32_t descriptorIndex,
			BaseTexture& texture,
			const GfxRenderTargetViewDesc& desc);

		void CreateDepthStencilView(
			ID3D12Device& device,
			uint32_t descriptorIndex,
			BaseTexture& texture,
			const GfxDepthStencilViewDesc& desc);

		void CreateShaderResourceView(
			ID3D12Device& device,
			uint32_t descriptorIndex,
			BaseTexture& texture,
			const GfxShaderResourceViewDesc& desc);

		void CreateSampler(
			ID3D12Device& device,
			uint32_t descriptorIndex,
			const GfxSamplerDesc& desc);

		void CreateConstantBufferView(
			ID3D12Device& device,
			uint32_t descriptorIndex,
			const GfxConstantBufferViewDesc& desc);

	public:
		GfxCpuDescriptor GetCpuDescriptor(uint32_t descriptorIndex) const;
		GfxGpuDescriptor GetGpuDescriptor(uint32_t descriptorIndex) const;

		ID3D12DescriptorHeap* GetDx12DescriptorHeap()
		{
			return m_descriptorHeap.Get();
		}

	private:	
		D3D12_CPU_DESCRIPTOR_HANDLE GetDx12CpuDescriptor(uint32_t descriptorId, size_t descriptorSize) const;

	private:
		ComPtr<ID3D12DescriptorHeap>      m_descriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE       m_cpuDescriptor;
		D3D12_GPU_DESCRIPTOR_HANDLE       m_gpuDescriptor;
		GfxDescriptorHeapType             m_type;
	};

} // namespace SI

#endif // SI_USE_DX12
