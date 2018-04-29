#pragma once

namespace SI
{
	void AssertInternal(const char* exprStr, const char* filename, int line);
	void AssertInternal(const char* exprStr, const char* filename, int line, const char* fmt, ...);
	void WarningInternal(const char* exprStr, const char* filename, int line);
	void WarningInternal(const char* exprStr, const char* filename, int line, const char* fmt, ...);
}

#define _SI_ASSERT(expr, exprStr, filename, line, ...) if(!expr){ AssertInternal(exprStr, filename, line, __VA_ARGS__); }
#define SI_ASSERT(expr, ...) _SI_ASSERT((expr), (#expr), __FILE__, __LINE__, __VA_ARGS__)

#define _SI_WARNING(expr, exprStr, filename, line, ...) if(!expr){ WarningInternal(exprStr, filename, line, __VA_ARGS__); }
#define SI_WARNING(expr, ...) _SI_WARNING((expr), (#expr), __FILE__, __LINE__, __VA_ARGS__)
