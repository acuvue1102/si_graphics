#pragma once

#include <cstdint>

namespace SI
{
	struct StringUtil
	{
		// 小文字化.
		static char GetLower(char c)
		{
			if('A' <= c && c <= 'Z')
			{
				return c-'A'+'a';
			}

			return c;
		}

		// 大文字化.
		static char GetUpper(char c)
		{
			if('a' <= c && c <= 'z')
			{
				return c-'a'+'A';
			}

			return c;
		}

		// aとbが同じか判定する.
		static bool IsSame(const char* a, const char* b)
		{
			while((*a) == (*b))
			{
				if((*a) == 0) return true;

				++a;
				++b;
			}

			return false;
		}
		
		// aとbが同じか判定する. 大文字と小文字を比較しない.
		static bool IsSameNoCase(const char* a, const char* b)
		{
			while(GetLower(*a) == GetLower(*b))
			{
				if((*a) == 0) return true;

				++a;
				++b;
			}

			return false;
		}
	};

} // namespace SI
