#pragma once

#include <cstdint>
#include <si_app/pipeline/pipeline_base.h>

namespace SI
{
namespace APP001
{
	class Pipeline  : public PipelineBase
	{
	public:
		explicit Pipeline(int observerSortKey);
		virtual ~Pipeline();

		int  OnInitialize(const AppInitializeInfo&) override;
		int  OnTerminate()                          override;
		void OnRender(const App& app, const AppUpdateInfo&)         override;

		int LoadAsset(const AppInitializeInfo& info);

	protected:
		GfxRootSignatureEx       m_rootSignature;
		GfxGraphicsState         m_graphicsState;
		
		GfxVertexShader          m_vertexShader;
		GfxPixelShader           m_pixelShader;

		GfxBuffer                m_vertexBuffer;
	};
	
} // namespace APP001
} // namespace SI