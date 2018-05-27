#pragma once

#include <stdint.h>
#include <si_app/pipeline/pipeline_base.h>

namespace SI
{
namespace APP002
{
	class Pipeline  : public PipelineBase
	{
	public:
		explicit Pipeline(int observerSortKey);
		virtual ~Pipeline();

		int OnInitialize(const AppInitializeInfo&) override;
		int OnTerminate()                          override;
		int OnUpdate(const AppUpdateInfo&)         override;
		int OnRender(const AppRenderInfo&)         override;

		int LoadAsset(const AppInitializeInfo& info);

	protected:
		struct TextureShaderConstant;
		struct LambertShaderConstant;

	protected:
		GfxRootSignature         m_rootSignatures[2];
		GfxGraphicsState         m_graphicsStates[2];
		GfxDescriptorHeap        m_cbvSrvUavHeaps[2];
		GfxDescriptorHeap        m_samplerHeaps[2];
		GfxBuffer                m_constantBuffers[2];
		TextureShaderConstant*   m_textureConstant;
		LambertShaderConstant*   m_lambertConstant;

		GfxVertexShader          m_textureVS;
		GfxPixelShader           m_texturePS;
		
		GfxVertexShader          m_lambertVS;
		GfxPixelShader           m_lambertPS;

		GfxBuffer                m_boxVertexBuffer;
		GfxBuffer                m_boxIndexBuffer;

		GfxBuffer                m_quadVertexBuffer;
		GfxTexture               m_texture;

		GfxTexture               m_rt;
		GfxDescriptorHeap        m_rtvHeap;

		GfxTexture               m_depth;
		GfxDescriptorHeap        m_dsvHeap;
	};
	
} // namespace APP002
} // namespace SI