#pragma once

namespace SI
{
	class BaseShader;
	class BaseVertexShader;
	class BasePixelShader;

	struct GfxShaderCompileDesc
	{
		const char** m_macros = nullptr; // "マクロ名" "定義"の順に並べる. { "PI", "3.14", nullptr, nullptr }
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
	};

} // namespace SI
