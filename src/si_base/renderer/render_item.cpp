
#include "si_base/renderer/render_item.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/geometry.h"
#include "si_base/gpu/gfx_graphics_context.h"

namespace SI
{
	bool RenderItem::NeedToCreatePSO(const RendererGraphicsStateDesc& renderDesc) const
	{
		if(m_graphicsStateDesc.GetHash() != renderDesc.GetHash()) return true;

		// TODO: マテリアル側の変更もあるかチェックする.

		return false;
	}

	void RenderItem::SetupPSO(const RendererGraphicsStateDesc& renderDesc)
	{
		SI_ASSERT(renderDesc.GetHash()!=Hash64(0));
		if(!NeedToCreatePSO(renderDesc)) return;

		m_graphicsStateDesc = renderDesc;
		
		GfxGraphicsStateDesc psoDesc;
		psoDesc.m_fillMode               = renderDesc.m_fillMode;
		psoDesc.m_cullMode               = renderDesc.m_cullMode;
		psoDesc.m_renderTargetCount      = renderDesc.m_renderTargetCount;
		for(uint32_t i=0; i<psoDesc.m_renderTargetCount; ++i)
		{
			psoDesc.m_rtvFormats[i]          = renderDesc.m_rtvFormats[i];
		}

		psoDesc.m_dsvFormat              = renderDesc.m_dsvFormat;
		psoDesc.m_depthWriteMask         = renderDesc.m_depthWriteMask;
		psoDesc.m_depthFunc              = renderDesc.m_depthFunc;

		psoDesc.m_depthBias              = renderDesc.m_depthBias;
		psoDesc.m_depthBiasClamp         = renderDesc.m_depthBiasClamp;
		psoDesc.m_slopeScaledDepthBias   = renderDesc.m_slopeScaledDepthBias;

		psoDesc.m_depthEnable            = renderDesc.m_depthEnable;
		psoDesc.m_stencilEnable          = renderDesc.m_stencilEnable;
		psoDesc.m_alphaToCoverageEnable  = renderDesc.m_alphaToCoverageEnable;
		psoDesc.m_independentBlendEnable = renderDesc.m_independentBlendEnable;
		psoDesc.m_frontCounterClockwise  = renderDesc.m_frontCounterClockwise;
			
		psoDesc.m_name                   = "";
		psoDesc.m_rootSignature          = &m_renderMaterial->GetRootSignature().Get();
		psoDesc.m_vertexShader           = &m_renderMaterial->GetVertexShader();
		psoDesc.m_pixelShader            = &m_renderMaterial->GetPixelShader();

		psoDesc.m_inputElements          = m_geometry->GetInputElements();
		psoDesc.m_inputElementCount      = m_geometry->GetInputElementCount();

		m_graphicsState.Initialize(psoDesc);
	}


} // namespace SI
