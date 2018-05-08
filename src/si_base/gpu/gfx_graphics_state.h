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

	struct GfxGraphicsStateDesc
	{
		const GfxInputElement*   m_inputElements     = nullptr;
		int                      m_inputElementCount = 0;
		
		GfxRootSignature*        m_rootSignature     = nullptr;
		GfxVertexShader*         m_vertexShader      = nullptr;
		GfxPixelShader*          m_pixelShader       = nullptr;

		GfxFormat                m_rtvFormats[8]     = {};
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
