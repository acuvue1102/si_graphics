#pragma once

namespace SI
{
	void PrintInternal(const char* fmt, ...);
}

#define SI_PRINT(...) SI::PrintInternal(__VA_ARGS__)
