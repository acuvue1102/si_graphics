#pragma once

#include "si_base/misc/hash_declare.h"
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseShader;
	class BaseVertexShader;
	class BasePixelShader;

	struct GfxShaderCompileMacro
	{
		const char* m_name       = nullptr;
		const char* m_definition = nullptr;
	};

	template<size_t SIZE>
	struct GfxShaderCompileMacros
	{
		void Add(const char* name, const char* definition)
		{
			SI_ASSERT(m_macroCount<SIZE);
			GfxShaderCompileMacro& m = m_macros[m_macroCount++];
			m.m_name = name;
			m.m_definition = definition;
		}

		GfxShaderCompileMacro m_macros[SIZE+1]; // +1は番兵用.
		uint32_t m_macroCount = 0;
	};

	struct GfxShaderCompileDesc
	{
		const GfxShaderCompileMacro* m_macros = nullptr; // "マクロ名" "定義"の順に並べる. 最後はnullptr. { {"PI", "3.14"}, {nullptr, nullptr} }
	};

	struct GfxShaderBindingResouceCount
	{
		uint32_t m_constantCount  = 0;
		uint32_t m_samplerCount   = 0;
		uint32_t m_srvBufferCount = 0;
		uint32_t m_uavBufferCount = 0;
	};
	
	///////////////////////////////////////////////////////

	class GfxShader
	{
	public:
		GfxShader() : m_base(nullptr) {}
		virtual ~GfxShader(){}

		virtual int LoadAndCompile(
			const char* file,
			const char* entryPoint,
			const GfxShaderCompileDesc& desc) = 0;
		virtual int Release() = 0;

		Hash64 GetHash() const;

		const GfxShaderBindingResouceCount& GetBindingResourceCount() const;
		virtual GfxShaderType GetType() const = 0;

		bool IsValid() const{ return m_base!=nullptr; }

	public:
		BaseShader*       GetBaseShader()      { return m_base; }
		const BaseShader* GetBaseShader() const{ return m_base; }

	protected:
		BaseShader* m_base;
	};

	///////////////////////////////////////////////////////

	class GfxVertexShader : public GfxShader
	{
	public:
		GfxVertexShader();
		~GfxVertexShader() override;
		
		int LoadAndCompile(
			const char* file,
			const char* entryPoint = "VSMain",
			const GfxShaderCompileDesc& desc = GfxShaderCompileDesc()) override;
		int Release() override;
		
		GfxShaderType GetType() const override{ return GfxShaderType::Vertex; }
	};

	///////////////////////////////////////////////////////

	class GfxPixelShader : public GfxShader
	{
	public:
		GfxPixelShader();
		~GfxPixelShader() override;

		int LoadAndCompile(
			const char* file,
			const char* entryPoint = "PSMain",
			const GfxShaderCompileDesc& desc = GfxShaderCompileDesc()) override;
		int Release() override;
		GfxShaderType GetType() const override{ return GfxShaderType::Pixel; }
	};

	///////////////////////////////////////////////////////

	class GfxComputeShader : public GfxShader
	{
	public:
		GfxComputeShader();
		~GfxComputeShader() override;

		int LoadAndCompile(
			const char* file,
			const char* entryPoint = "CSMain",
			const GfxShaderCompileDesc& desc = GfxShaderCompileDesc()) override;
		int Release() override;
		GfxShaderType GetType() const override{ return GfxShaderType::Compute; }
	};

} // namespace SI
