#pragma once

#include <cstdint>
#include <si_app/pipeline/pipeline_base.h>
#include <si_base/math/math_declare.h>

namespace SI
{
namespace APP002
{
	class Pipeline  : public PipelineBase
	{
	public:
		explicit Pipeline(int observerSortKey);
		virtual ~Pipeline();

		int  OnInitialize(const AppInitializeInfo&) override;
		int  OnTerminate()                          override;

		void OnUpdate(const App& app, const AppUpdateInfo&)         override;
		void OnRender(const App& app, const AppUpdateInfo&)         override;

		int LoadAsset(const AppInitializeInfo& info);

		void SetView(Vfloat4x4_arg view);

	protected:
		struct TextureShaderConstant;
		struct LambertShaderConstant;

	protected:
		GfxRootSignatureEx       m_rootSignatures[2];
		GfxGraphicsState         m_graphicsStates[2];
		GfxBufferEx_Constant     m_constantBuffers[2];
		TextureShaderConstant*   m_textureConstant;
		LambertShaderConstant*   m_lambertConstant;

		GfxVertexShader          m_textureVS;
		GfxPixelShader           m_texturePS;
		
		GfxVertexShader          m_lambertVS;
		GfxPixelShader           m_lambertPS;

		GfxTextureEx_Static      m_texture;

		GfxTestureEx_Rt          m_rt;
		GfxTestureEx_DepthRt     m_depth;

		GfxDynamicSampler        m_sampler;
		GfxBuffer m_buffer;
	};
	
} // namespace APP002
} // namespace SI