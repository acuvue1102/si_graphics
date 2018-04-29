#pragma once

// SCOPE_EXITマクロで使われるオブジェクト

#define __STR(s) #s
#define _STR(s) __STR(s)

#define __JOIN(a, b) a ## b
#define _JOIN(a, b) __JOIN(a, b)

#define SCOPE_EXIT(...) \
struct _JOIN(_ScopeExit, __LINE__)\
{\
	_JOIN(_JOIN(_ScopeExit, __LINE__), ())\
	{\
	}\
	_JOIN(_JOIN(~_ScopeExit, __LINE__), ())\
	{\
	__VA_ARGS__\
	}\
} _JOIN(se, __LINE__)
