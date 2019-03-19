#pragma once

#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/misc/hash_declare.h"

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
		GfxColorWriteFlags    m_rtWriteMask    = GfxColorWriteFlag::RGBA;
		GfxBlend              m_srcBlend       = GfxBlend::One;
		GfxBlend              m_destBlend      = GfxBlend::Zero;
		GfxBlendOp            m_blendOp        = GfxBlendOp::Add;
		GfxBlend              m_srcBlendAlpha  = GfxBlend::One;
		GfxBlend              m_destBlendAlpha = GfxBlend::Zero;
		GfxBlendOp            m_blendOpAlpha   = GfxBlendOp::Add;
		GfxLogicOp            m_logicOp        = GfxLogicOp::Noop;
		
		bool operator==(const GfxRenderTargetBlendDesc& desc) const
		{
			if(m_blendEnable    != desc.m_blendEnable)    return false;
			if(m_logicOpEnable  != desc.m_logicOpEnable)  return false;
			if(m_rtWriteMask    != desc.m_rtWriteMask)    return false;
			if(m_srcBlend       != desc.m_srcBlend)       return false;
			if(m_destBlend      != desc.m_destBlend)      return false;
			if(m_blendOp        != desc.m_blendOp)        return false;
			if(m_srcBlendAlpha  != desc.m_srcBlendAlpha)  return false;
			if(m_destBlendAlpha != desc.m_destBlendAlpha) return false;
			if(m_blendOpAlpha   != desc.m_blendOpAlpha)   return false;
			if(m_logicOp        != desc.m_logicOp)        return false;

			return true;
		}

		bool operator!=(const GfxRenderTargetBlendDesc& desc) const
		{
			return !(*this==desc);
		}
	};

	struct GfxGraphicsStateDescCore
	{
		const char*              m_name                   = nullptr;
		GfxRootSignature*        m_rootSignature          = nullptr;
		const GfxVertexShader*   m_vertexShader           = nullptr;
		const GfxPixelShader*    m_pixelShader            = nullptr;

		const GfxInputElement*   m_inputElements          = nullptr;
		int                      m_inputElementCount      = 0;

		GfxFillMode              m_fillMode               = GfxFillMode::Solid;
		GfxCullMode              m_cullMode               = GfxCullMode::Back;
		uint32_t                 m_renderTargetCount      = 1;
		GfxFormat                m_rtvFormats[8]          = {};
		GfxRenderTargetBlendDesc m_rtvBlend[8];
		GfxFormat                m_dsvFormat              = GfxFormat::D32_Float;
		GfxPrimitiveTopologyType m_primitiveTopologyType  = GfxPrimitiveTopologyType::Triangle;
		GfxDepthWriteMask        m_depthWriteMask         = GfxDepthWriteMask::All;
		GfxComparisonFunc        m_depthFunc              = GfxComparisonFunc::LessEqual;
		
		int                      m_depthBias              = 0;
		float                    m_depthBiasClamp         = 0.0f;
		float                    m_slopeScaledDepthBias   = 0.0f;

		bool                     m_depthEnable            = false;
		bool                     m_stencilEnable          = false;
		bool                     m_alphaToCoverageEnable  = false;
		bool                     m_independentBlendEnable = false;
		bool                     m_frontCounterClockwise  = false;

		Hash64 GenerateHash() const;
		
		bool operator==(const GfxGraphicsStateDescCore& desc) const;
		bool operator!=(const GfxGraphicsStateDescCore& desc) const{ return !(*this==desc); }
	};

	struct GfxGraphicsStateDesc : public GfxGraphicsStateDescCore
	{
		GfxGraphicsStateDesc(){}
		GfxGraphicsStateDesc(const GfxGraphicsStateDescCore& desc);

		void UpdateHash();
		Hash64 GetHash();

	private:
		Hash64 m_hash = 0;
	};

	class GfxGraphicsState
	{
	public:
		GfxGraphicsState(BaseGraphicsState* base=nullptr);
		~GfxGraphicsState();

	public:
		BaseGraphicsState* GetBaseGraphicsState(){ return m_base; }
		const BaseGraphicsState* GetBaseGraphicsState() const{ return m_base; }

	private:
		BaseGraphicsState* m_base;
	};

} // namespace SI
