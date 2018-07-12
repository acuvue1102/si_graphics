#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <comdef.h>

namespace SI
{
	struct GfxShaderCompileDesc;

	class BaseShader
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		BaseShader(){}
		virtual ~BaseShader(){}
		
		virtual int LoadAndCompile(
			const char* file,
			const char* entryPoint,
			const GfxShaderCompileDesc& desc)  = 0;
		
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

	protected:
		ComPtr<ID3DBlob> m_shader;
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
