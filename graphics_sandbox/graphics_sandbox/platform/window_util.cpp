#include <locale.h>
#include <Windows.h>
#include <tchar.h>

#include "platform/window_util.h"

namespace SI
{
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void* WindowUtil::Initialize(int width, int height)
	{
		setlocale( LC_CTYPE, "ja_JP.UTF-8");

		static LPCTSTR kClassName = _T("graphics_sandbox");

		HINSTANCE hInstance = GetModuleHandle(NULL);
		// initialize a window class.
		WNDCLASSEX wcex = {0};
		wcex.cbSize        = sizeof(WNDCLASSEX);
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = WndProc;
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
			return nullptr;
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
			NULL);               // lpParam

		if (hWnd == NULL)
		{
			MessageBox(NULL, _T("Error: Window Creation"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
			return nullptr;
		}

		return hWnd;
	}

	int WindowUtil::ShowWindow(void* hWnd, int nCmdShow)
	{
		// show window
		::ShowWindow((HWND)hWnd, nCmdShow);

		return 0;
	}

	int WindowUtil::Terminate()
	{
		return 0;
	}

	int WindowUtil::MessageProc()
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

	
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}
}