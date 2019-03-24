#pragma once

#include <cstdint>
#include "si_app/app/app_module.h"
#include "si_base/gpu/gfx.h"
#include "si_base/renderer/renderer.h"

namespace SI
{
	struct PipelineDesc
	{
		uint32_t m_width  = 1280;
		uint32_t m_height = 720;
		void*   m_hWnd    = nullptr;
	};

	class PipelineBase : public AppModule
	{
	public:
		explicit PipelineBase(int observerSortKey);
		virtual ~PipelineBase();

		virtual int  OnInitialize(const AppInitializeInfo&) override;
		virtual int  OnTerminate()                          override;

		virtual void OnUpdate(const App& app, const AppUpdateInfo&) override;
		virtual void OnRender(const App& app, const AppUpdateInfo&) override;

	protected:
		void BeginRender();
		void EndRender();

	protected:
		GfxDevice                m_device;
		GfxCore                  m_core;
		GfxCommandQueue          m_commandQueue;
		GfxSwapChain             m_swapChain;
		GfxContextManager        m_contextManager;
		//GfxGraphicsCommandList   m_graphicsCommandList;
		Renderer                 m_renderer;
		
	};

} // namespace SI
