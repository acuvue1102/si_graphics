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
		struct ShaderConstant;

	protected:
		GfxRootSignature         m_rootSignature;
		GfxGraphicsState         m_graphicsState;
		
		GfxVertexShader          m_vertexShader;
		GfxPixelShader           m_pixelShader;

		GfxBuffer                m_vertexBuffer;
		GfxTexture               m_texture;
		GfxDescriptorHeap        m_cbvSrvUavHeap;
		GfxDescriptorHeap        m_samplerHeap;
		
		GfxBuffer                m_constantBuffer;
		ShaderConstant*          m_constantAddr;
	};
	
} // namespace APP002
} // namespace SI