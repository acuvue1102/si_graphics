
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_input_layout.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/misc/hash.h"

namespace SI
{
	uint64_t GfxGraphicsStateDescCore::GenerateHash() const
	{
		Hash64Generator hashGenerator;
		if(m_name)          hashGenerator.Add(m_name);
		// m_rootSignature 何かやる.
		if(m_vertexShader)  hashGenerator.Add(m_vertexShader->GetHash());
		if(m_pixelShader)   hashGenerator.Add(m_pixelShader->GetHash());
		
		for(int i=0; i<m_inputElementCount; ++i)
		{
			const GfxInputElement& e = m_inputElements[i];
			if(e.m_semanticsName) hashGenerator.Add(e.m_semanticsName);
			hashGenerator.Add(e.m_semanticsId);
			hashGenerator.Add(e.m_format);
			hashGenerator.Add(e.m_inputSlot);
			hashGenerator.Add(e.m_alignedByteOffset);
		}
		
		hashGenerator.Add(m_fillMode);
		hashGenerator.Add(m_cullMode);
		for(uint32_t i=0; i<m_renderTargetCount; ++i)
		{
			hashGenerator.Add(m_rtvFormats[i]);
			hashGenerator.Add(m_rtvBlend[i]);
		}
		
		hashGenerator.Add(m_dsvFormat);
		hashGenerator.Add(m_primitiveTopologyType);
		hashGenerator.Add(m_depthWriteMask);
		hashGenerator.Add(m_depthFunc);
		
		hashGenerator.Add(m_depthBias);
		hashGenerator.Add(m_depthBiasClamp);
		hashGenerator.Add(m_slopeScaledDepthBias);
		
		hashGenerator.Add(m_depthEnable);
		hashGenerator.Add(m_stencilEnable);
		hashGenerator.Add(m_alphaToCoverageEnable);
		hashGenerator.Add(m_independentBlendEnable);
		hashGenerator.Add(m_frontCounterClockwise);

		return hashGenerator.Generate();
	}
	
	bool GfxGraphicsStateDescCore::operator==(const GfxGraphicsStateDescCore& desc) const
	{
		if(m_name)
		{
			if(!desc.m_name) return false;
			if(strcmp(m_name, desc.m_name)!=0) return false;
		}
		else
		{
			if(desc.m_name) return false;
		}
		
		// m_rootSignature 何かやる.
		
		if(m_vertexShader)
		{
			if(!desc.m_vertexShader) return false;
			if(m_vertexShader->GetHash() != desc.m_vertexShader->GetHash()) return false;
		}
		else
		{
			if(desc.m_vertexShader) return false;
		}

		if(m_pixelShader)
		{
			if(!desc.m_pixelShader) return false;
			if(m_pixelShader->GetHash() != desc.m_pixelShader->GetHash()) return false;
		}
		else
		{
			if(desc.m_pixelShader) return false;
		}

		if(m_inputElementCount != desc.m_inputElementCount) return false;
		for(int i=0; i<m_inputElementCount; ++i)
		{
			if(m_inputElements[i] != desc.m_inputElements[i]) return false;
		}
		
		if(m_fillMode != desc.m_fillMode) return false;
		if(m_cullMode != desc.m_cullMode) return false;
		
		if(m_renderTargetCount != desc.m_renderTargetCount) return false;
		for(uint32_t i=0; i<m_renderTargetCount; ++i)
		{
			if(m_rtvFormats[i] != desc.m_rtvFormats[i]) return false;
			if(m_rtvBlend[i]   != desc.m_rtvBlend[i])   return false;
		}
		
		if(m_dsvFormat             != desc.m_dsvFormat)             return false;
		if(m_primitiveTopologyType != desc.m_primitiveTopologyType) return false;
		if(m_depthWriteMask        != desc.m_depthWriteMask)        return false;
		if(m_depthFunc             != desc.m_depthFunc)             return false;

		if(m_depthBias            != desc.m_depthBias)            return false;
		if(m_depthBiasClamp       != desc.m_depthBiasClamp)       return false;
		if(m_slopeScaledDepthBias != desc.m_slopeScaledDepthBias) return false;
		
		if(m_depthEnable            != desc.m_depthEnable)            return false;
		if(m_stencilEnable          != desc.m_stencilEnable)          return false;
		if(m_alphaToCoverageEnable  != desc.m_alphaToCoverageEnable)  return false;
		if(m_independentBlendEnable != desc.m_independentBlendEnable) return false;
		if(m_frontCounterClockwise  != desc.m_frontCounterClockwise)  return false;

		return true;
	}
	
	GfxGraphicsStateDesc::GfxGraphicsStateDesc(const GfxGraphicsStateDescCore& desc)
		: GfxGraphicsStateDescCore(desc)
	{
	}
	
	void GfxGraphicsStateDesc::UpdateHash()
	{
		m_hash = GfxGraphicsStateDescCore::GenerateHash();
	}

	Hash64 GfxGraphicsStateDesc::GetHash()
	{
		if(m_hash==0) UpdateHash();

		return m_hash;
	}

	GfxGraphicsState::GfxGraphicsState(BaseGraphicsState* base)
		: m_base(base)
	{
	}

	GfxGraphicsState::~GfxGraphicsState()
	{
	}

} // namespace SI
