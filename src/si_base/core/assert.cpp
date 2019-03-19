
#include "si_base/core/assert.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "si_base/platform/windows_proxy.h"

namespace SI
{
	namespace
	{
		void PrintErrorInternal(const char* buf)
		{
			OutputDebugStringA(buf);
			puts(buf);
		}
	}

	void AssertInternal(const char* exprStr, const char* filename, int line)
	{
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "%s(%i): SI_ASSERT(%s)\n", filename, line, exprStr);
		PrintErrorInternal(buf);

		DebugBreak();
	}

	void AssertInternal(const char* exprStr, const char* filename, int line, const char* fmt, ...)
	{	
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "%s(%i): SI_ASSERT(%s,%s)\n", filename, line, exprStr, fmt);
		PrintErrorInternal(buf);

		va_list arg;
		va_start(arg, fmt);
		vsprintf_s(buf, sizeof(buf), fmt, arg);
		va_end(arg);

		strcat_s(buf, "\n");
		PrintErrorInternal(buf);
		
		DebugBreak();
	}
	
	void WarningInternal(const char* exprStr, const char* filename, int line)
	{
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "%s(%i): SI_WARNING(%s)\n", filename, line, exprStr);
		PrintErrorInternal(buf);
	}

	void WarningInternal(const char* exprStr, const char* filename, int line, const char* fmt, ...)
	{	
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "%s(%i): SI_WARNING(%s,%s)\n", filename, line, exprStr, fmt);
		PrintErrorInternal(buf);

		va_list arg;
		va_start(arg, fmt);
		vsprintf_s(buf, sizeof(buf), fmt, arg);
		va_end(arg);

		strcat_s(buf, "\n");
		PrintErrorInternal(buf);
	}
}
