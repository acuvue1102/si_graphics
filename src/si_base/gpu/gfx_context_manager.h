#pragma once

#include <vector>

#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_graphics_context.h"
#include "si_base/gpu/gfx_graphics_command_list.h"

namespace SI
{
	class GfxCommandQueue;

	struct GfxContextManagerDesc
	{
		uint32_t m_contextCount = 2;
	};

	class GfxContextManager
	{
	public:
		GfxContextManager();
		~GfxContextManager();
		
		void Initialize(const GfxContextManagerDesc& desc);
		void Terminate();

		uint32_t GetGraphicsContextCount(){ return (uint32_t)m_graphicsContexts.size(); }
		
		GfxGraphicsContext& GetGraphicsContext(uint32_t id)
		{
			return *m_graphicsContexts[id];
		}
		GfxGraphicsContext& GetLastGraphicsContext()
		{
			return GetGraphicsContext(GetGraphicsContextCount()-1);
		}
		
		void ResetContexts();
		void CloseContexts();
		
		void Execute(GfxCommandQueue& queue);

	private:
		std::vector<GfxGraphicsContext*>     m_graphicsContexts;
		std::vector<GfxGraphicsCommandList*> m_graphicsCoordinators; // コンテキスト間のステートの調整用コマンドリスト
		
		std::vector<GfxCommandList*>         m_tempCommandLists;
	};

} // namespace SI
