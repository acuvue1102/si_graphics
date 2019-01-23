#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <comdef.h>
#include "si_base/misc/hash_declare.h"
#include "si_base/gpu/gfx_shader.h"

namespace SI
{
	struct GfxShaderCompileDesc;
	
	class BaseShader
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		BaseShader()
			: m_hash(0)
		{}
		virtual ~BaseShader(){}
		
		virtual int LoadAndCompile(
			const char* file,
			const char* entryPoint,
			const GfxShaderCompileDesc& desc)  = 0;
		
		int LoadAndCompileCommon(
			const char* path,
			const char* entryPoint,
			const GfxShaderCompileDesc& desc,
			const char* target,
			Microsoft::WRL::ComPtr<ID3DBlob>& shader,
			Hash64& hash);
		
		virtual int Release()
		{
			m_shader.Reset();
			return 0;
		}

		virtual const void* GetBinary() const
		{
			return m_shader->GetBufferPointer();
		}

		virtual size_t GetBinarySize() const
		{
			return m_shader->GetBufferSize();
		}

		Hash64 GetHash() const
		{
			return m_hash;
		}

		const GfxShaderBindingResouceCount& GetBindingResourceCount() const
		{
			return m_resourceCount;
		}

	protected:
		ComPtr<ID3DBlob>              m_shader;
		Hash64                        m_hash;
		GfxShaderBindingResouceCount  m_resourceCount;
	};

	class BaseVertexShader : public BaseShader
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		BaseVertexShader(){}
		~BaseVertexShader(){}

		int LoadAndCompile(
			const char* file,
			const char* entryPoint,
			const GfxShaderCompileDesc& desc) override;
	};

	/////////////////////////////////////////////////////////////////////

	class BasePixelShader : public BaseShader
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BasePixelShader(){}
		~BasePixelShader(){}

		int LoadAndCompile(
			const char* file,
			const char* entryPoint,
			const GfxShaderCompileDesc& desc) override;
	};
	
	/////////////////////////////////////////////////////////////////////

	class BaseComputeShader : public BaseShader
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseComputeShader(){}
		~BaseComputeShader(){}

		int LoadAndCompile(
			const char* file,
			const char* entryPoint,
			const GfxShaderCompileDesc& desc) override;
	};


} // namespace SI

#endif // SI_USE_DX12
