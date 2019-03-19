#pragma once

#include "si_base/gpu/gfx_graphics_state.h"

namespace SI
{	
	struct RendererGraphicsStateDescCore
	{
		GfxFillMode              m_fillMode               = GfxFillMode::Solid;
		uint32_t                 m_renderTargetCount      = 1;
		GfxFormat                m_rtvFormats[8]          = {};
		GfxRenderTargetBlendDesc m_rtvBlend[8];
		GfxFormat                m_dsvFormat              = GfxFormat::D32_Float;
		GfxDepthWriteMask        m_depthWriteMask         = GfxDepthWriteMask::All;
		GfxComparisonFunc        m_depthFunc              = GfxComparisonFunc::LessEqual;

		int                      m_depthBias              = 0;
		float                    m_depthBiasClamp         = 0.0f;
		float                    m_slopeScaledDepthBias   = 0.0f;

		bool                     m_depthEnable            = false;
		bool                     m_stencilEnable          = false;
		bool                     m_alphaToCoverageEnable  = false;
		bool                     m_independentBlendEnable = false;
	};

	struct RendererGraphicsStateDesc : public RendererGraphicsStateDescCore
	{
		void UpdateHash();
		Hash64 GetHash();

	private:
		Hash64 m_hash = 0;
	};

} // namespace SI
