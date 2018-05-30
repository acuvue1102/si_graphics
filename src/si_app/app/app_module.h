#pragma once

#include <stdint.h>

namespace SI
{
	enum class MouseButton;
	enum class Key;
	class App;

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

	// Appの各イベントをを受け取るオブザーバー.
	// orderが低い順に各イベントが呼ばれる.
	class AppModule
	{
	public:
		AppModule(int sortKey) : m_sortKey(sortKey) {}
		virtual ~AppModule(){}
		
		virtual int  OnInitialize     (const AppInitializeInfo&) { return 0; }
		virtual int  OnTerminate      ()                         { return 0; }

		virtual void OnPreUpdate      (const App& app, const AppUpdateInfo&)     {}
		virtual void OnUpdate         (const App& app, const AppUpdateInfo&)     {}
		virtual void OnRender         (const App& app, const AppUpdateInfo&)     {}
		
		virtual void OnMouseMove     (const App& app, int x, int y)              {}
		virtual void OnMouseButton   (const App& app, MouseButton b, bool isDown){}
		virtual void OnMouseWheel    (const App& app, int wheel)                 {}
		virtual void OnKeyboard      (const App& app, Key k, bool isDown)        {}

	public:
		int GetSortKey() const{ return m_sortKey; }

	public:
		enum{ classValue = 'ApMd' };
	private:
		int m_sortKey;
	};
} // namespace SI
