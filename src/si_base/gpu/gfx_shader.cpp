
#include "si_base/core/base.h"
#include "si_base/gpu/dx12/dx12_shader.h"
#include "si_base/gpu/gfx_shader.h"

namespace SI
{
	GfxVertexShader::GfxVertexShader()
	{
	}

	GfxVertexShader::~GfxVertexShader()
	{
		Release();
	}
	
	int GfxVertexShader::LoadAndCompile(
		const char* file,
		const char* entryPoint,
		const GfxShaderCompileDesc& desc)
	{
		if(m_base) return -1;

		m_base = SI_NEW(BaseVertexShader);
		return m_base->LoadAndCompile(file, entryPoint, desc);
	}

	int GfxVertexShader::Release()
	{
		if(m_base)
		{
			m_base->Release();
			SI_DELETE(m_base);
		}

		return 0;
	}
	
	///////////////////////////////////////////////////////

	GfxPixelShader::GfxPixelShader()
	{
	}

	GfxPixelShader::~GfxPixelShader()
	{
		Release();
	}
	
	int GfxPixelShader::LoadAndCompile(
		const char* file,
		const char* entryPoint,
		const GfxShaderCompileDesc& desc)
	{
		if(m_base) return -1;

		m_base = SI_NEW(BasePixelShader);
		return m_base->LoadAndCompile(file, entryPoint, desc);
	}

	int GfxPixelShader::Release()
	{
		if(m_base)
		{
			m_base->Release();
			SI_DELETE(m_base);
		}

		return 0;
	}

} // namespace SI
