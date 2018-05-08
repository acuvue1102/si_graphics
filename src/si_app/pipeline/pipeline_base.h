#pragma once

#include <stdint.h>
#include "si_app/app/app_observer.h"
#include "si_base/gpu/gfx.h"

namespace SI
{
	struct PipelineDesc
	{
		uint32_t m_width  = 1280;
		uint32_t m_height = 720;
		void*   m_hWnd    = nullptr;
	};

	class PipelineBase : public AppObserver
	{
	public:
		explicit PipelineBase(int observerSortKey);
		virtual ~PipelineBase();

		virtual int OnInitialize(const AppInitializeInfo&) override;
		virtual int OnTerminate()                          override;
		virtual int OnUpdate(const AppUpdateInfo&)         override;
		virtual int OnRender(const AppRenderInfo&)         override;

	protected:
		int BeginRender();
		int EndRender();

	protected:
		GfxDevice                m_device;
		GfxCommandQueue          m_commandQueue;
		GfxSwapChain             m_swapChain;
		GfxGraphicsCommandList   m_graphicsCommandList;
	};

} // namespace SI
