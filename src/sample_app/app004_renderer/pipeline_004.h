#pragma once

#include <cstdint>
#include <si_app/pipeline/pipeline_base.h>
#include <si_base/math/math_declare.h>
#include <si_base/math/math.h>
#include <si_base/renderer/scenes_instance.h>

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
		//ModelInstancePtr         m_modelInstance;
		ScenesInstancePtr        m_scenesInstance;

		GfxRootSignatureEx       m_rootSignature;
		GfxGraphicsState         m_graphicsState;
		GfxBufferEx_Constant     m_constantBuffer;
		TextureShaderConstant*   m_textureConstant;

		GfxVertexShader          m_textureVS;
		GfxPixelShader           m_texturePS;

		GfxTestureEx_Rt          m_rt;
		GfxTestureEx_DepthRt     m_depth;

		GfxDynamicSampler        m_sampler;
		
		Vfloat4x4                m_view;
		Vfloat4x4                m_proj;
	};
	
} // namespace APP004
} // namespace SI