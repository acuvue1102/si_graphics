
#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include "si_base/core/core.h"
#include "si_base/gpu/gfx_desc.h"
#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_input_layout.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/dx12/dx12_shader.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"
#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"

namespace SI
{
	BaseGraphicsState::BaseGraphicsState()
	{
	}

	BaseGraphicsState::~BaseGraphicsState()
	{
	}

	int BaseGraphicsState::Initialize(ID3D12Device& device, const GfxGraphicsStateDesc& desc)
	{
		D3D12_INPUT_ELEMENT_DESC elements[32];
		SI_ASSERT(desc.m_inputElementCount < ArraySize(elements));
		uint32_t elementCont = SI::Min((uint32_t)desc.m_inputElementCount, (uint32_t)ArraySize(elements));
		for(uint32_t e=0; e<elementCont; ++e)
		{
			D3D12_INPUT_ELEMENT_DESC& outElem = elements[e];
			const GfxInputElement&     inElem = desc.m_inputElements[e];
			
			outElem.SemanticName         = inElem.m_semanticsName;
			outElem.SemanticIndex        = inElem.m_semanticsId;
			outElem.Format               = SI::GetDx12Format(inElem.m_format);
			outElem.InputSlot            = inElem.m_inputSlot;
			outElem.AlignedByteOffset    = inElem.m_alignedByteOffset;
			outElem.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			outElem.InstanceDataStepRate = 0;
		}
		
		const BaseShader* vertexShader = desc.m_vertexShader->GetBaseShader();
		const BaseShader* pixelShader  = desc.m_pixelShader->GetBaseShader();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { elements, elementCont };
		psoDesc.pRootSignature = desc.m_rootSignature->GetBaseRootSignature()->GetComPtrRootSignature().Get();
		psoDesc.VS.pShaderBytecode = vertexShader? vertexShader->GetBinary() : nullptr;
		psoDesc.VS.BytecodeLength  = vertexShader? vertexShader->GetBinarySize() : 0;
		psoDesc.PS.pShaderBytecode = pixelShader? pixelShader->GetBinary() : nullptr;
		psoDesc.PS.BytecodeLength  = pixelShader? pixelShader->GetBinarySize() : 0;
				
        psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
        psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        psoDesc.RasterizerState.DepthClipEnable = TRUE;
        psoDesc.RasterizerState.MultisampleEnable = FALSE;
        psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
        psoDesc.RasterizerState.ForcedSampleCount = 0;
        psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
        psoDesc.BlendState.IndependentBlendEnable = FALSE;
        const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
        {
            FALSE,FALSE,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL,
        };
        for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
            psoDesc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;
		}
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		
		static_assert(ArraySize(psoDesc.RTVFormats) == 8, "array count error");
		SI_ASSERT(ArraySize(psoDesc.RTVFormats) == ArraySize(desc.m_rtvFormats));
		for(uint32_t rt=0; rt<ArraySize(desc.m_rtvFormats); ++rt)
		{
			psoDesc.RTVFormats[rt] = GetDx12Format(desc.m_rtvFormats[rt]);
		}
		psoDesc.SampleDesc.Count = 1;

		HRESULT hr = device.CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
		if(FAILED(hr))
		{
			SI_ASSERT(0);
			return -1;
		}

		return 0;
	}

} // namespace SI

#endif // SI_USE_DX12
