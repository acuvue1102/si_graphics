#pragma once

#include <cstdint>
#include <si_app/pipeline/pipeline_base.h>
#include <si_base/math/math_declare.h>
#include <si_base/renderer/model_instance.h>

namespace SI
{
namespace APP004

{
	class Pipeline : public PipelineBase
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
		ModelInstancePtr         m_modelInstance;

		GfxRootSignatureEx       m_rootSignatures[2];
		GfxGraphicsState         m_graphicsStates[2];
		GfxBufferEx_Constant     m_constantBuffers[2];
		TextureShaderConstant*   m_textureConstant;
		LambertShaderConstant*   m_lambertConstant;

		GfxVertexShader          m_textureVS;
		GfxPixelShader           m_texturePS;
		
		GfxVertexShader          m_lambertVS;
		GfxPixelShader           m_lambertPS;

		GfxTextureEx             m_texture;

		GfxTestureEx_Rt          m_rt;
		GfxTestureEx_DepthRt     m_depth;

		GfxSamplerEx             m_sampler;
	};
	
} // namespace APP004
} // namespace SI