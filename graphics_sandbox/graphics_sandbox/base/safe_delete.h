#pragma once

namespace SI
{
	template<typename T>
	inline void SafeDelete(T*& pointer)
	{
		delete pointer;
		pointer = nullptr;
	}

	template<typename T>
	inline void SafeDeleteArray(T*& pointer)
	{
		delete[] pointer;
		pointer = nullptr;
	}
}