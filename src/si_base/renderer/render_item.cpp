
#include "si_base/renderer/render_item.h"
#include "si_base/renderer/material.h"
#include "si_base/gpu/gfx_graphics_context.h"
#include "si_base/renderer/scenes.h"

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

		static thread_local std::array<GfxInputElement, 32> inputElements;

		uint32_t vertexAttributeCount = (uint32_t)m_vertexAttributes->size();
		SI_ASSERT(vertexAttributeCount<(uint32_t)inputElements.size());
		for(uint32_t v=0; v<vertexAttributeCount; ++v)
		{
			const VertexAttribute& vertexAttribute = (*m_vertexAttributes)[v];
			const Accessor& accessor = m_scenes->GetAccessor((uint32_t)vertexAttribute.m_accessorId);
			
			inputElements[v] = GfxInputElement(
				vertexAttribute.m_semantics,
				accessor.GetFormat(),
				v,
				0);
		}
		psoDesc.m_inputElements = inputElements.data();
		psoDesc.m_inputElementCount = vertexAttributeCount;

		m_graphicsState.Initialize(psoDesc);
	}

	bool RenderItem::IsValid() const
	{
		if(!m_scenes)           return false;
		if(!m_material)         return false;
		if(!m_renderMaterial)   return false;
		if(!m_subMesh)          return false;
		if(!m_indexAccessor)    return false;
		if(!m_vertexAttributes) return false;

		uint32_t vertexAttributeCount = (uint32_t)m_vertexAttributes->size();
		for(uint32_t v=0; v<vertexAttributeCount; ++v)
		{
			const VertexAttribute& vertexAttribute = (*m_vertexAttributes)[v];
			if(vertexAttribute.m_accessorId<0) return false;
		}

		if(!m_graphicsState.Get().IsValid()) return false;
		return true;
	}

} // namespace SI
