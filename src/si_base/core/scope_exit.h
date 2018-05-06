#pragma once

// SCOPE_EXIT�}�N���Ŏg����I�u�W�F�N�g

#define __STR(s) #s
#define _STR(s) __STR(s)

#define __JOIN(a, b) a ## b
#define _JOIN(a, b) __JOIN(a, b)
//
//#define SCOPE_EXIT(...) \
//struct _JOIN(_ScopeExit, __LINE__)\
//{\
//	_JOIN(_JOIN(_ScopeExit, __LINE__), ())\
//	{\
//	}\
//	_JOIN(_JOIN(~_ScopeExit, __LINE__), ())\
//	{\
//	__VA_ARGS__\
//	}\
//} _JOIN(se, __LINE__)


#include <functional>

#define SI_SCOPE_EXIT(...) \
struct _JOIN(_ScopeExit, __LINE__)\
{\
	std::function<void(void)> m_func;\
	_JOIN(_JOIN(_ScopeExit, __LINE__), (std::function<void(void)> f))\
		: m_func(f)\
	{\
	}\
	_JOIN(_JOIN(~_ScopeExit, __LINE__), ())\
	{\
		m_func();\
	}\
} _JOIN(se, __LINE__)([&]{__VA_ARGS__})