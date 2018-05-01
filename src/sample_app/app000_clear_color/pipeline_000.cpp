
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
	
	int Pipeline::OnRender(const AppRenderInfo&)
	{
		BeginRender();

		GfxTexture tex = m_swapChain.GetSwapChainTexture();

		m_graphicsCommandList.ClearRenderTarget(tex, 0.0f, 0.2f, 0.4f, 1.0f);

		EndRender();

		return 0;
	}
	
} // namespace APP000
} // namespace SI
