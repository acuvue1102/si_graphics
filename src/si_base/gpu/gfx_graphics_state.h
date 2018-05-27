#pragma once

#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseGraphicsState;
	class GfxShader;
	class GfxRootSignature;
	struct GfxInputElement;

	struct GfxRenderTargetBlendDesc
	{
		bool                  m_blendEnable    = false;
		bool                  m_logicOpEnable  = false;
		GfxColorWriteFlags    m_rtWriteMask    = GfxColorWriteFlag::kRGBA;
		GfxBlend              m_srcBlend       = GfxBlend::kOne;
		GfxBlend              m_destBlend      = GfxBlend::kZero;
		GfxBlendOp            m_blendOp        = GfxBlendOp::kAdd;
		GfxBlend              m_srcBlendAlpha  = GfxBlend::kOne;
		GfxBlend              m_destBlendAlpha = GfxBlend::kZero;
		GfxBlendOp            m_blendOpAlpha   = GfxBlendOp::kAdd;
		GfxLogicOp            m_logicOp        = GfxLogicOp::kNoop;
	};

	struct GfxGraphicsStateDesc
	{
		GfxRootSignature*        m_rootSignature          = nullptr;
		GfxVertexShader*         m_vertexShader           = nullptr;
		GfxPixelShader*          m_pixelShader            = nullptr;

		const GfxInputElement*   m_inputElements          = nullptr;
		int                      m_inputElementCount      = 0;

		GfxFillMode              m_fillMode               = GfxFillMode::kSolid;
		GfxCullMode              m_cullMode               = GfxCullMode::kBack;
		uint32_t                 m_renderTargetCount      = 1;
		GfxFormat                m_rtvFormats[8]          = {};
		GfxRenderTargetBlendDesc m_rtvBlend[8];
		GfxFormat                m_dsvFormat              = GfxFormat::kD32_Float;
		GfxPrimitiveTopologyType m_primitiveTopologyType  = GfxPrimitiveTopologyType::kTriangle;
		GfxDepthWriteMask        m_depthWriteMask         = GfxDepthWriteMask::kAll;
		GfxComparisonFunc        m_depthFunc              = GfxComparisonFunc::kLessEqual;
		
		int                      m_depthBias              = 0;
		float                    m_depthBiasClamp         = 0.0f;
		float                    m_slopeScaledDepthBias   = 0.0f;

		bool                     m_depthEnable            = false;
		bool                     m_stencilEnable          = false;
		bool                     m_alphaToCoverageEnable  = false;
		bool                     m_independentBlendEnable = false;
		bool                     m_frontCounterClockwise  = false;
	};

	class GfxGraphicsState
	{
	public:
		GfxGraphicsState(BaseGraphicsState* base=nullptr);
		~GfxGraphicsState();

		int Release();

	public:
		BaseGraphicsState* GetBaseGraphicsState(){ return m_base; }
		const BaseGraphicsState* GetBaseGraphicsState() const{ return m_base; }

	private:
		BaseGraphicsState* m_base;
	};

} // namespace SI
