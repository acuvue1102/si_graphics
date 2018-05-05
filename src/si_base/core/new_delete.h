/////////////////////////////////////////
// new_delete.h
//
// メモリ確保は直接newを使わず、
// 後にフックできるようにするため、ここのマクロ経由で作る.
/////////////////////////////////////////
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

#define SI_NEW(type, ...)              (new type(__VA_ARGS__))
#define SI_NEW_ARRAY(type, size)       (new type[(size)])
#define SI_DELETE(p)                   (SI::SafeDelete(p))
#define SI_DELETE_ARRAY(p)             (SI::SafeDeleteArray(p))
