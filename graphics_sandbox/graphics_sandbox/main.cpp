// main.cpp       Written by Keiji Matsuda in 2017

#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/* main function */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	static LPCWSTR kClassName = L"graphics_sandbox";

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
		MessageBox(NULL, L"Error: Window Class Registeration", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}


	// create the window
	HWND hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,    // dwExStyle
		kClassName,          // lpClass
		L"graphics sandbox", // lpWindowName
		WS_OVERLAPPEDWINDOW, // dwStyleame
		CW_USEDEFAULT,       // x
		CW_USEDEFAULT,		 // y
		1280,                // nWidth
		720,                 // nHeight
		NULL,                // hWndParent
		NULL,                // hMenu
		hInstance,           // hInstance
		NULL);               // lpParam

	if (hWnd == NULL)
	{
		MessageBox(NULL, L"Error: Window Creation", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}


	// show window
	ShowWindow(hWnd, nCmdShow);


	// message loop
	MSG msg = {0};
	while (0 < GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
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
