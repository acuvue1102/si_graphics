#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>

struct ID3D12Device;

namespace SI
{
	class BaseDevice;
	struct GfxComputeStateDesc;

	class BaseComputeState
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseComputeState();
		~BaseComputeState();

		int Initialize(ID3D12Device& device, const GfxComputeStateDesc& desc);
		
	public:
		ComPtr<ID3D12PipelineState>& GetComPtrComputeState()
		{
			return m_pipelineState;
		}

	private:
		ComPtr<ID3D12PipelineState> m_pipelineState;
	};

} // namespace SI

#endif // SI_USE_DX12
