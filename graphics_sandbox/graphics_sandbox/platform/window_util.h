#pragma once

namespace SI
{
	class WindowUtil
	{
	public:
		static void* Initialize(int width, int height);
		static int ShowWindow(void* hWnd, int nCmdShow);
		static int Terminate();
		static int MessageProc();
	};

} // namespace SI