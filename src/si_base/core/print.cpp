
#include "si_base/core/print.h"

#include <stdarg.h>
#include <Windows.h>
#include <stdio.h>

namespace SI
{
	void PrintInternal(const char* fmt, ...)
	{
		char buf[1024];
		
		va_list arg;
		va_start(arg, fmt);
		vsprintf_s(buf, sizeof(buf), fmt, arg);
		va_end(arg);

		OutputDebugStringA(buf);
	}
}
