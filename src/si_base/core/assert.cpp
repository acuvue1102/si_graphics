
#include "si_base/core/assert.h"

#include <stdarg.h>
#include <Windows.h>
#include <stdio.h>

namespace SI
{
	void AssertInternal(const char* exprStr, const char* filename, int line)
	{
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "%s(%i): SI_ASSERT(%s)\n", filename, line, exprStr);
		OutputDebugStringA(buf);
		DebugBreak();
	}

	void AssertInternal(const char* exprStr, const char* filename, int line, const char* fmt, ...)
	{	
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "%s(%i): SI_ASSERT(%s,%s)\n", filename, line, exprStr, fmt);
		OutputDebugStringA(buf);

		va_list arg;
		va_start(arg, fmt);
		vsprintf_s(buf, sizeof(buf), fmt, arg);
		va_end(arg);

		strcat_s(buf, "\n");
		OutputDebugStringA(buf);
		
		DebugBreak();
	}
	
	void WarningInternal(const char* exprStr, const char* filename, int line)
	{
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "%s(%i): SI_WARNING(%s)\n", filename, line, exprStr);
		OutputDebugStringA(buf);
	}

	void WarningInternal(const char* exprStr, const char* filename, int line, const char* fmt, ...)
	{	
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "%s(%i): SI_WARNING(%s,%s)\n", filename, line, exprStr, fmt);
		OutputDebugStringA(buf);

		va_list arg;
		va_start(arg, fmt);
		vsprintf_s(buf, sizeof(buf), fmt, arg);
		va_end(arg);

		strcat_s(buf, "\n");
		OutputDebugStringA(buf);
	}
}
