
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
				
		psoDesc.RasterizerState.FillMode              = GetDx12FillMode(desc.m_fillMode);
		psoDesc.RasterizerState.CullMode              = GetDx12CullMode(desc.m_cullMode);
		psoDesc.RasterizerState.FrontCounterClockwise = desc.m_frontCounterClockwise? TRUE : FALSE;
		psoDesc.RasterizerState.DepthBias             = desc.m_depthBias;
		psoDesc.RasterizerState.DepthBiasClamp        = desc.m_depthBiasClamp;
		psoDesc.RasterizerState.SlopeScaledDepthBias  = desc.m_slopeScaledDepthBias;
		psoDesc.RasterizerState.DepthClipEnable       = TRUE;
		psoDesc.RasterizerState.MultisampleEnable     = FALSE;
		psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		psoDesc.RasterizerState.ForcedSampleCount     = 0;
		psoDesc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		psoDesc.SampleMask                            = UINT_MAX;
		psoDesc.PrimitiveTopologyType                 = GetDx12PrimitiveTopologyType(desc.m_primitiveTopologyType);		
		psoDesc.SampleDesc.Count                      = 1;
		psoDesc.NumRenderTargets                      = desc.m_renderTargetCount;
		psoDesc.BlendState.AlphaToCoverageEnable      = FALSE;
		psoDesc.BlendState.IndependentBlendEnable     = FALSE;
		psoDesc.DepthStencilState.DepthEnable         = desc.m_depthEnable? TRUE : FALSE;
		psoDesc.DepthStencilState.StencilEnable       = desc.m_stencilEnable? TRUE : FALSE;
		if(psoDesc.DepthStencilState.DepthEnable)
		{
			psoDesc.DepthStencilState.DepthWriteMask  = GetDx12DepthWriteMask(desc.m_depthWriteMask);
			psoDesc.DepthStencilState.DepthFunc       = GetDx12ComparisonFunc(desc.m_depthFunc);
		}
		
		SI_ASSERT(desc.m_renderTargetCount <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
		UINT blendRtCount = psoDesc.BlendState.IndependentBlendEnable? psoDesc.NumRenderTargets : 1;

		for (UINT rt = 0; rt<blendRtCount; ++rt)
		{
			D3D12_RENDER_TARGET_BLEND_DESC& outBlend = psoDesc.BlendState.RenderTarget[rt];
			const GfxRenderTargetBlendDesc&  inBlend = desc.m_rtvBlend[rt];

			outBlend.BlendEnable            = inBlend.m_blendEnable?   TRUE : FALSE;
			outBlend.LogicOpEnable          = inBlend.m_logicOpEnable? TRUE : FALSE;
			outBlend.SrcBlend               = GetDx12Blend(inBlend.m_srcBlend);
			outBlend.DestBlend              = GetDx12Blend(inBlend.m_destBlend);
			outBlend.BlendOp                = GetDx12BlendOp(inBlend.m_blendOp);
			outBlend.SrcBlendAlpha          = GetDx12Blend(inBlend.m_srcBlendAlpha);
			outBlend.DestBlendAlpha         = GetDx12Blend(inBlend.m_destBlendAlpha);
			outBlend.BlendOpAlpha           = GetDx12BlendOp(inBlend.m_blendOpAlpha);
			outBlend.LogicOp                = GetDx12LogicOp(inBlend.m_logicOp);
			outBlend.RenderTargetWriteMask  = GetDx12RenderTargetWriteMask(inBlend.m_rtWriteMask);
		}

		for (UINT rt = 0; rt<psoDesc.NumRenderTargets; ++rt)
		{			
			psoDesc.RTVFormats[rt] = GetDx12Format(desc.m_rtvFormats[rt]);
		}
		psoDesc.DSVFormat = GetDx12Format(desc.m_dsvFormat);

		HRESULT hr = device.CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
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
