#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_device_std_allocator.h"
#include "si_base/gpu/dx12/dx12_declare.h"

namespace SI
{
	struct GfxRaytracingShaderTablesDesc;

	struct BaseRaytracingShaderTable
	{
		Microsoft::WRL::ComPtr<ID3D12Resource>   m_buffer;
		D3D12_GPU_VIRTUAL_ADDRESS                m_gpuAddress = 0;
		size_t                                   m_size = 0;
		size_t                                   m_stride = 0;
	};

	class BaseRaytracingShaderTables
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		BaseRaytracingShaderTables();
		~BaseRaytracingShaderTables();

		int Initialize(ID3D12Device& device, const GfxRaytracingShaderTablesDesc& desc);

		const BaseRaytracingShaderTable& GetRayGenShaderTable()   const{ return m_rayGenShaderTable; }
		const BaseRaytracingShaderTable& GetMissShaderTable()     const{ return m_missShaderTable; }
		const BaseRaytracingShaderTable& GetHitGroupShaderTable() const{ return m_hitGroupShaderTable; }


	private:
		BaseRaytracingShaderTable m_rayGenShaderTable;
		BaseRaytracingShaderTable m_missShaderTable;
		BaseRaytracingShaderTable m_hitGroupShaderTable;
	};

} // namespace SI

#endif // SI_USE_DX12