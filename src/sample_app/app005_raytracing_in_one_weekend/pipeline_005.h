#pragma once

#include <cstdint>
#include <si_app/pipeline/pipeline_base.h>
#include <si_base/math/math_declare.h>

namespace SI
{
namespace APP005
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

	protected:
		struct RaytracingShaderConstant;
		struct TextureShaderConstant;

	protected:
		GfxRootSignatureEx       m_computeRootSignatures;
		GfxRootSignatureEx       m_rootSignatures;
		GfxComputeState          m_computeStates;
		GfxGraphicsState         m_graphicsStates;
		GfxBufferEx_Constant     m_raytracingConstantBuffers;
		GfxBufferEx_Constant     m_constantBuffers;
		RaytracingShaderConstant*    m_raytracingConstant;
		TextureShaderConstant*   m_textureConstant;
		
		GfxComputeShader         m_raytracingCS;

		GfxVertexShader          m_textureVS;
		GfxPixelShader           m_texturePS;

		GfxBufferEx_Vertex       m_quadVertexBuffer;		
		GfxTestureEx_Uav         m_resultTexture;

		GfxSamplerEx             m_sampler;
	};
	
} // namespace APP003
} // namespace SI