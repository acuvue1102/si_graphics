#pragma once

#include "gpu/gfx_root_signature.h"
#include "gpu/gfx_shader.h"

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
