#pragma once

#include <stdint.h>
#include <si_app/pipeline/pipeline_base.h>

namespace SI
{
	class Pipeline  : public PipelineBase
	{
	public:
		explicit Pipeline(int observerSortKey);
		virtual ~Pipeline();

		int OnInitialize(const AppInitializeInfo&) override;
		int OnTerminate()                          override;
		int OnRender(const AppRenderInfo&)         override;

		int LoadAsset(const AppInitializeInfo& info);

	protected:
		GfxRootSignature         m_rootSignature;
		GfxGraphicsState         m_graphicsState;
		
		GfxVertexShader          m_vertexShader;
		GfxPixelShader           m_pixelShader;

		GfxBuffer                m_vertexBuffer;
	};

} // namespace SI