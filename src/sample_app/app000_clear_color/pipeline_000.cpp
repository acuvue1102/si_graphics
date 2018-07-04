
#include "pipeline_000.h"

#include <string>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>

namespace SI
{	
namespace APP000
{
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
	{
	}

	Pipeline::~Pipeline()
	{
	}
	
	int Pipeline::OnInitialize(const AppInitializeInfo& info)
	{
		if( PipelineBase::OnInitialize(info) != 0) return -1;

		return 0;
	}

	int Pipeline::OnTerminate()
	{
		m_swapChain.Flip(); // wait previous frame.

		PipelineBase::OnTerminate();

		return 0;
	}
	
	void Pipeline::OnRender(const App& app, const AppUpdateInfo&)
	{
		BeginRender();

		GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);
		GfxTestureEx_SwapChain& swapChainTexture = m_swapChain.GetTexture();
		swapChainTexture.SetClearColor(GfxColorRGBA(0.0f, 0.2f, 0.4f, 1.0f));

		context.ClearRenderTarget(m_swapChain.GetTexture());

		EndRender();
	}
	
} // namespace APP000
} // namespace SI
