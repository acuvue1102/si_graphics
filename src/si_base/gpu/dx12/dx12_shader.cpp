

#include <D3Dcompiler.h>
#include "si_base/platform/windows_proxy.h"
#include "si_base/core/core.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/dx12/dx12_shader.h"

#if SI_USE_DX12
namespace SI
{	
	int BaseVertexShader::LoadAndCompile(
		const char* file,
		const char* entryPoint,
		const GfxShaderCompileDesc& desc)
	{
		WCHAR fileW[260];
		size_t len = 0;
		errno_t err = mbstowcs_s(&len, fileW, file, _TRUNCATE);
		if(err) return -1;
		
#if defined(_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		ComPtr<ID3DBlob> errCode;
		HRESULT hr = D3DCompileFromFile(
			fileW,
			nullptr,
			nullptr,
			entryPoint,
			"vs_5_0",
			compileFlags,
			0,
			&m_shader,
			&errCode);
		if(FAILED(hr))
		{
			const char* errString = (const char*)errCode->GetBufferPointer();
			SI_ASSERT(0, "error D3DCompileFromFile\n%s", errString);
			return -1;
		}

		return 0;
	}

	//////////////////////////////////////////
	
	int BasePixelShader::LoadAndCompile(
		const char* file,
		const char* entryPoint,
		const GfxShaderCompileDesc& desc)
	{
		WCHAR fileW[260];
		size_t len = 0;
		errno_t err = mbstowcs_s(&len, fileW, file, _TRUNCATE);
		if(err) return -1;
		
#if defined(_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif
		
		ComPtr<ID3DBlob> errCode;
		HRESULT hr = D3DCompileFromFile(
			fileW,
			nullptr,
			nullptr,
			entryPoint,
			"ps_5_0",
			compileFlags,
			0,
			&m_shader,
			&errCode);
		if(FAILED(hr))
		{
			const char* errString = (const char*)errCode->GetBufferPointer();
			SI_ASSERT(0, "error D3DCompileFromFile\n%s", errString);
			return -1;
		}

		return 0;
	}

} // namespace SI

#endif // SI_USE_DX12
