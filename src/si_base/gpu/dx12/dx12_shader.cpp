

#include <D3Dcompiler.h>
#include <vector>
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
		HRESULT hr;
#if defined(_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		Microsoft::WRL::ComPtr<ID3DBlob> errCode;
		File file;
		if(file.Open(path) < 0) return -1;
		int64_t fileSize = file.GetFileSize();
		if(fileSize<=0) return -1;
		std::vector<char> buffer(fileSize+1);
		file.Read(&buffer[0], fileSize);
		file.Close();
		buffer[fileSize] = 0;

		const char* code = &buffer[0];

		Hash64Generator hashGenerator;
		hashGenerator.Add(code);
		hashGenerator.Add(entryPoint);
		{
			const GfxShaderCompileMacro* macros = desc.m_macros;
			while(macros && macros->m_name && macros->m_definition)
			{
				hashGenerator.Add(macros->m_name);
				hashGenerator.Add(macros->m_definition);

				++macros;
			}
		}
		hash = hashGenerator.Generate();
		
		WCHAR cachePathW[260]; cachePathW[0] = 0;
		char cachePath[260]; cachePath[0] = 0;
		{
			size_t len = 0;
			sprintf_s(cachePath, "%s.%s.%llx.blob", path, entryPoint, (uint64_t)hash);
			errno_t err = mbstowcs_s(&len, cachePathW, cachePath, _TRUNCATE);
			if(err) return -1;
		}

		bool hasCache = File::Exists(cachePath);
		if(hasCache)
		{
			hr = D3DReadFileToBlob(cachePathW, &shader);
			
			if(FAILED(hr))
			{
				hasCache = false;
			}
		}
		static_assert(sizeof(D3D_SHADER_MACRO) == sizeof(GfxShaderCompileMacro), "構造体そのままキャストできない");

		if(!hasCache)
		{
			hr = D3DCompile(
				code,
				fileSize,
				path,
				(const D3D_SHADER_MACRO*)desc.m_macros,
				nullptr,
				entryPoint,
				target,
				compileFlags,
				0,
				&shader,
				&errCode);

			if(FAILED(hr))
			{
				const char* errString = (const char*)errCode->GetBufferPointer();
				SI_ASSERT(0, "error D3DCompileFromFile\n%s", errString);
				return -1;
			}
			
			// キャッシュ保存.
			D3DWriteBlobToFile(shader.Get(), cachePathW, TRUE);
		}
		
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
