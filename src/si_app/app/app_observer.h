#pragma once

#include <stdint.h>

namespace SI
{
	struct AppInitializeInfo
	{
		uint32_t m_width  = 1280;
		uint32_t m_height = 720;
		void*    m_hWnd   = nullptr;
	};

	struct AppUpdateInfo
	{
		float m_frameDeltaTime = 1.0f/60.0f;
	};

	struct AppRenderInfo
	{
		float m_frameDeltaTime = 1.0f/60.0f;
	};

	// Appの各イベントをを受け取るオブザーバー.
	// orderが低い順に各イベントが呼ばれる.
	class AppObserver
	{
	public:
		AppObserver(int sortKey) : m_sortKey(sortKey) {}
		virtual ~AppObserver(){}
		
		virtual int OnInitialize(const AppInitializeInfo&) { return 0; }
		virtual int OnTerminate()                          { return 0; }
		virtual int OnUpdate(const AppUpdateInfo&)         { return 0; }
		virtual int OnRender(const AppRenderInfo&)         { return 0; }

	public:
		int GetSortKey() const{ return m_sortKey; }

	private:
		int m_sortKey;
	};
} // namespace SI
