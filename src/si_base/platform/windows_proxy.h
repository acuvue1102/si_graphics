#pragma once
// windowsのヘッダは色々マクロを定義しているので打ち消したかったが….

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>

//#ifdef near
//#undef near
//#endif
//
//#ifdef far
//#undef far
//#endif

namespace SI
{
	inline void OpenDefaultConsole()
	{
		AllocConsole();
		FILE* fp = nullptr;
		freopen_s(&fp, "conin$","r",stdin);
		freopen_s(&fp, "conout$","w",stdout);
		freopen_s(&fp, "conout$","w",stderr);
	}
}