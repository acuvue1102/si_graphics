#pragma once

#include "si_base/input/mouse.h"
#include "si_base/input/keyboard.h"

namespace SI
{
	class WindowsApp
	{
	public:
		WindowsApp();
		virtual ~WindowsApp();

		void InitializeBase(int width, int height);
		void TerminateBase();
		int  UpdateBase();

	public:
		void* GetWindowHandle() const{ return m_hWnd; }
		int ShowWindow(int nCmdShow);

	public:
		const Mouse&    GetMouse() const{ return m_mouse; }
		const Keyboard& GetKeyboard() const{ return m_keyboard; }

	protected:
		// イベントハンドラ.
		
		// マウスの状態に変化があったときに呼ばれる関数.
		virtual void OnMouseMove     (int x, int y){}
		virtual void OnMouseButton   (MouseButton b, bool isDown){}
		virtual void OnMouseWheel    (int wheel){}
		
		// キーボードの状態に変化があったときに呼ばれる関数.
		virtual void OnKeyboard      (Key k, bool isDown){}

	public:
		intptr_t WndProc(void* hWnd, uint32_t msg, uintptr_t wParam, intptr_t lParam);

	private:
		void UpdateMouseMove();
		int MessageProc();

	private:
		void*      m_hWnd;
		Mouse      m_mouse;
		Keyboard   m_keyboard;
	};
	
	using AppPlatformBase = WindowsApp;

} // namespace SI
