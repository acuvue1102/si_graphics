#include <locale.h>
#include <Windows.h>
#include <Windowsx.h>
#include <stdio.h>
#include <tchar.h>

#include "si_base/platform/window_app.h"

namespace SI
{
	namespace
	{
		Key GetKeyCode(WPARAM wParam)
		{
			if('A' <= wParam && wParam <= 'Z') return (Key)wParam;
			if('0' <= wParam && wParam <= '9') return (Key)wParam;

			switch(wParam)
			{
			case VK_OEM_COMMA:
				return Key::Comma;
			case VK_OEM_MINUS:
				return Key::Hyphen;
			case VK_OEM_PERIOD:
				return Key::Dot;
			case VK_OEM_2:
				return Key::Slash;
			case VK_OEM_1:
				return Key::Colon;
			case VK_OEM_PLUS:
				return Key::Semicolon;
			case VK_OEM_3:
				return Key::At;
			case VK_OEM_4:
				return Key::LeftBracket;
			case VK_OEM_5:
				return Key::BackSlash;
			case VK_OEM_6:
				return Key::RightBracket;
			case VK_OEM_7:
				return Key::Carret;
			case VK_F1:
				return Key::F1;
			case VK_F3:
				return Key::F3;
			case VK_F4:
				return Key::F4;
			case VK_F5:
				return Key::F5;
			case VK_F6:
				return Key::F6;
			case VK_F7:
				return Key::F7;
			case VK_F8:
				return Key::F8;
			case VK_F9:
				return Key::F9;
			case VK_F10:
				return Key::F10;
			case VK_F11:
				return Key::F11;
			case VK_F12:
				return Key::F12;
			case VK_SHIFT:
				return Key::Shift;
			case VK_MENU:
				return Key::Alt;
			case VK_CONTROL:
				return Key::Ctrl;
			case VK_SPACE:
				return Key::Space;
			case VK_RETURN:
				return Key::Enter;
			case VK_TAB:
				return Key::Tab;
			case VK_CAPITAL:
				return Key::Caps;
			case VK_DELETE:
				return Key::Del;
			case VK_BACK:
				return Key::Backspace;
			case VK_UP:
				return Key::Up;
			case VK_DOWN:
				return Key::Down;
			case VK_LEFT:
				return Key::Left;
			case VK_RIGHT:
				return Key::Right;
			case VK_ESCAPE:
				return Key::Esc;
			case VK_INSERT:
				return Key::Insert;
			default:
				break;
			}

			return Key::None;
		}
	}

	LRESULT CALLBACK WndProcCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	WindowsApp::WindowsApp()
		: m_hWnd(nullptr)
	{
	}

	WindowsApp::~WindowsApp()
	{
	}

	void WindowsApp::InitializeBase(int width, int height)
	{
		setlocale( LC_CTYPE, "ja_JP.UTF-8");

		static LPCTSTR kClassName = _T("graphics_sandbox");

		HINSTANCE hInstance = GetModuleHandle(NULL);
		// initialize a window class.
		WNDCLASSEX wcex = {0};
		wcex.cbSize        = sizeof(WNDCLASSEX);
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = WndProcCallback;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = 0;
		wcex.hInstance     = hInstance;
		wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION); 
		wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName  = NULL;
		wcex.lpszClassName = kClassName;
		wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

		// register the window class
		if(!RegisterClassEx(&wcex))
		{
			// Failure
			MessageBox(NULL, _T("Error: Window Class Registeration"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		// create the window
		HWND hWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,    // dwExStyle
			kClassName,          // lpClass
			_T("graphics sandbox"), // lpWindowName
			WS_OVERLAPPEDWINDOW, // dwStyleame
			CW_USEDEFAULT,       // x
			CW_USEDEFAULT,		 // y
			width,               // nWidth
			height,              // nHeight
			NULL,                // hWndParent
			NULL,                // hMenu
			hInstance,           // hInstance
			this);               // lpParam

		if (hWnd == NULL)
		{
			MessageBox(NULL, _T("Error: Window Creation"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		m_hWnd = hWnd;
	}

	void WindowsApp::TerminateBase()
	{
	}

	int WindowsApp::UpdateBase()
	{
		m_keyboard.Flip();
		m_mouse.Flip();

		int ret = MessageProc();
		
		m_keyboard.UpdateModifier();
		
		return ret;
	}

	int WindowsApp::ShowWindow(int nCmdShow)
	{
		// show window
		::ShowWindow((HWND)m_hWnd, nCmdShow);

		return 0;
	}

	int WindowsApp::MessageProc()
	{
		// message loop
		MSG msg = {0};
		if(GetMessage(&msg, NULL, 0, 0)<= 0)
		{
			return -1;
		}
	
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		return 0;
	}
	
	intptr_t WindowsApp::WndProc(void* _hWnd, uint32_t msg, uintptr_t wParam, intptr_t lParam)
	{
		HWND hWnd = (HWND)_hWnd;

		switch (msg)
		{
		// マウス関連の処理.
		case WM_MOUSEMOVE:
		{
			m_mouse.SetX( GET_X_LPARAM(lParam) );
			m_mouse.SetY( GET_Y_LPARAM(lParam) );
			OnMouseMove(m_mouse.GetX(), m_mouse.GetY());
			break;
		}
		case WM_LBUTTONDOWN:
		{
			m_mouse.SetLButton( true );
			OnMouseButton(MouseButton::Left, true);
			break;
		}
		case WM_LBUTTONUP:
		{
			m_mouse.SetLButton( false );
			OnMouseButton(MouseButton::Left, false);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			m_mouse.SetRButton( true );
			OnMouseButton(MouseButton::Right, true);
			break;
		}
		case WM_RBUTTONUP:
		{
			m_mouse.SetRButton( false );
			OnMouseButton(MouseButton::Right, false);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			m_mouse.SetMButton( true );
			OnMouseButton(MouseButton::Middle, true);
			break;
		}
		case WM_MBUTTONUP:
		{
			m_mouse.SetMButton( false );
			OnMouseButton(MouseButton::Middle, false);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			m_mouse.SetWheel(GET_WHEEL_DELTA_WPARAM(wParam));
			OnMouseWheel(m_mouse.GetWheel());
			break;
		}

		// キーボード関連の処理
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			Key key = GetKeyCode(wParam);
			m_keyboard.SetKey(key, true);
			OnKeyboard(key, true);
			break;
		}
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			Key key = GetKeyCode(wParam);
			m_keyboard.SetKey(key, false);
			OnKeyboard(key, false);
			break;
		}

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}

	LRESULT CALLBACK WndProcCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CREATE:
		{
			LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)createStruct->lpCreateParams);
			break;
		}
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			break;
		default:
			return ((WindowsApp*)(GetWindowLongPtr(hWnd, GWLP_USERDATA)))->WndProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}
}