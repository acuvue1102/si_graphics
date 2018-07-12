
#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include "si_base/platform/windows_proxy.h"
#include "si_base/core/core.h"
#include "si_base/gpu/gfx_desc.h"
#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/gfx_compute_state.h"
#include "si_base/gpu/gfx_input_layout.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/dx12/dx12_shader.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"
#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/dx12/dx12_compute_state.h"

namespace SI
{
	BaseComputeState::BaseComputeState()
	{
	}

	BaseComputeState::~BaseComputeState()
	{
	}

	int BaseComputeState::Initialize(ID3D12Device& device, const GfxComputeStateDesc& desc)
	{
		const BaseShader* computeShader = desc.m_computeShader->GetBaseShader();

		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = desc.m_rootSignature->GetBaseRootSignature()->GetComPtrRootSignature().Get();
		psoDesc.CS.pShaderBytecode = computeShader->GetBinary();
		psoDesc.CS.BytecodeLength  = computeShader->GetBinarySize();
		
		HRESULT hr = device.CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
		if(FAILED(hr))
		{
			SI_ASSERT(0);
			return -1;
		}

		if(desc.m_name)
		{
			wchar_t wName[64];
			wName[0] = 0;
			size_t num = 0;
			errno_t ret = mbstowcs_s(&num, wName, desc.m_name, ArraySize(wName));
			if(ret == 0)
			{
				m_pipelineState->SetName(wName);
			}
		}

		return 0;
	}

} // namespace SI

#endif // SI_USE_DX12
