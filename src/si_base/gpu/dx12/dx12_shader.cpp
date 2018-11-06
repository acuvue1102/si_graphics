

#include <D3Dcompiler.h>
#include "si_base/platform/windows_proxy.h"
#include "si_base/core/core.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/dx12/dx12_shader.h"
#include "si_base/file/file.h"

#if SI_USE_DX12
namespace SI
{
	namespace
	{
		int LoadAndCompileCommon(
			const char* path,
			const char* entryPoint,
			const GfxShaderCompileDesc& desc,
			const char* target,
			Microsoft::WRL::ComPtr<ID3DBlob>& shader)
		{
	#if defined(_DEBUG)
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#else
			UINT compileFlags = 0;
	#endif

			Microsoft::WRL::ComPtr<ID3DBlob> errCode;
	#if 0
			WCHAR fileW[260];
			size_t len = 0;
			errno_t err = mbstowcs_s(&len, fileW, path, _TRUNCATE);
			if(err) return -1;

			HRESULT hr = D3DCompileFromFile(
				fileW,
				nullptr,
				nullptr,
				entryPoint,
				target,
				compileFlags,
				0,
				&m_shader,
				&errCode);
	#else
			File file;
			if(file.Open(path) < 0) return -1;
			int64_t fileSize = file.GetFileSize();
			if(fileSize<=0) return -1;
			std::vector<char> buffer(fileSize);
			file.Read(&buffer[0], fileSize);
			file.Close();

			HRESULT hr = D3DCompile(
				&buffer[0],
				fileSize,
				path,
				nullptr,
				nullptr,
				entryPoint,
				target,
				compileFlags,
				0,
				&shader,
				&errCode);
	#endif
			if(FAILED(hr))
			{
				const char* errString = (const char*)errCode->GetBufferPointer();
				SI_ASSERT(0, "error D3DCompileFromFile\n%s", errString);
				return -1;
			}

			return 0;
		}
	}

	int BaseVertexShader::LoadAndCompile(
		const char* path,
		const char* entryPoint,
		const GfxShaderCompileDesc& desc)
	{
		return LoadAndCompileCommon(
			path,
			entryPoint,
			desc,
			"vs_5_0",
			m_shader);
	}

	//////////////////////////////////////////
	
	int BasePixelShader::LoadAndCompile(
		const char* path,
		const char* entryPoint,
		const GfxShaderCompileDesc& desc)
	{
		return LoadAndCompileCommon(
			path,
			entryPoint,
			desc,
			"ps_5_0",
			m_shader);
	}
	
	//////////////////////////////////////////
	
	int BaseComputeShader::LoadAndCompile(
		const char* path,
		const char* entryPoint,
		const GfxShaderCompileDesc& desc)
	{
		return LoadAndCompileCommon(
			path,
			entryPoint,
			desc,
			"cs_5_0",
			m_shader);
	}

} // namespace SI

#endif // SI_USE_DX12
