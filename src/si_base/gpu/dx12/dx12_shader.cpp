

#include <D3Dcompiler.h>
#include "si_base/platform/windows_proxy.h"
#include "si_base/core/core.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/dx12/dx12_shader.h"
#include "si_base/file/file.h"
#include "si_base/misc/hash.h"

#if SI_USE_DX12

namespace SI
{
	int BaseShader::LoadAndCompileCommon(
		const char* path,
		const char* entryPoint,
		const GfxShaderCompileDesc& desc,
		const char* target,
		Microsoft::WRL::ComPtr<ID3DBlob>& shader,
		Hash64& hash)
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

		Hash64Generator hashGenerator;
		hashGenerator.Add(path);
		hashGenerator.Add(entryPoint);
		hash = hashGenerator.Generate();

		ID3D12ShaderReflection* reflector = nullptr;
		hr = D3DReflect(
			shader->GetBufferPointer(),
			shader->GetBufferSize(),
			IID_ID3D12ShaderReflection,
			(void**)&reflector);
		if( FAILED( hr ) || reflector==nullptr)
		{
			SI_ASSERT(0, "error D3DReflect\n");
			return -1;
		}
		//SI_SCOPE_EXIT( reflector->Release(); );
			
		// シェーダ情報取得
		D3D12_SHADER_DESC shdDesc;
		hr = reflector->GetDesc(&shdDesc);
		if( FAILED( hr ) || reflector==nullptr)
		{
			SI_ASSERT(0, "error Get D3D12_SHADER_DESC\n");
			return -1;
		}

		GfxShaderBindingResouceCount& counter = m_resourceCount;
		for(UINT i=0; i<shdDesc.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflector->GetResourceBindingDesc(i, &bindDesc);
			SI_PRINT("%d: %s\n", i, bindDesc.Name);

			switch(bindDesc.Type)
			{
			case D3D_SIT_CBUFFER:
				++counter.m_constantCount;
				break;
			case D3D_SIT_TBUFFER:
			case D3D_SIT_STRUCTURED:
			case D3D_SIT_BYTEADDRESS:
			case D3D_SIT_TEXTURE:
				++counter.m_srvBufferCount;
				break;
			case D3D_SIT_SAMPLER:
				++counter.m_samplerCount;
				break;
			case D3D_SIT_UAV_RWTYPED:
			case D3D_SIT_UAV_RWSTRUCTURED:
			case D3D_SIT_UAV_RWBYTEADDRESS:
			case D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				++counter.m_uavBufferCount;
				break;
			default:
				break;
			}
		}

		reflector->Release();

		return 0;
	}

	////////////////////////////////////////

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
			m_shader,
			m_hash);
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
			m_shader,
			m_hash);
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
			m_shader,
			m_hash);
	}

} // namespace SI

#endif // SI_USE_DX12
