#pragma once

#include <cstdint>
#include <string>
#include "si_base/core/assert.h"
#include "si_base/misc/hash_internal.h"

namespace SI
{
	////////////////////////////////////////////////////////////////////////////////////////////////////

	// 普通の型のハッシュ.
	template<typename T>
	inline Hash32 GetHash32(const T& value, uint32_t seed = kDefaultHashSeed32)
	{
		return InternalHash32(&value, sizeof(T), seed);
	}
	
	// 文字列のハッシュ.
	inline Hash32 GetHash32(const char* const& str, uint32_t seed = kDefaultHashSeed32)
	{
		SI_ASSERT(str);
		return InternalHash32(str, strlen(str), seed);
	}
	
	// 定数文字列のハッシュ.
	template<size_t SIZE>
	inline Hash32 GetHash32(const char (&str)[SIZE], uint32_t seed = kDefaultHashSeed32)
	{
		return InternalHash32(str, SIZE-1, seed);
	}
	
	// 普通の型のハッシュ.
	template<typename T>
	inline Hash64 GetHash64(const T& value, uint64_t seed = kDefaultHashSeed64)
	{
		return InternalHash64(&value, sizeof(T), seed);
	}
	
	// 文字列のハッシュ.
	inline Hash64 GetHash64(const char* const& str, uint64_t seed = kDefaultHashSeed64)
	{
		SI_ASSERT(str);
		return InternalHash64(str, strlen(str), seed);
	}
	
	// 定数文字列のハッシュ. コンパイル時解決.
	template<size_t SIZE>
	inline Hash64 GetHash64(const char (&str)[SIZE], uint64_t seed = kDefaultHashSeed64)
	{
		return InternalHash64(str, SIZE-1, seed);
	}

	////////////////////////////////////////////////////////////////////////////////////
	
	// 定数文字列のハッシュ. コンパイル時解決.
	template<size_t SIZE>
	inline constexpr Hash32 GetHash32S(const char (&str)[SIZE], uint32_t seed = kDefaultHashSeed32)
	{
		return StaticInternalHash32(str, SIZE-1, seed);
	}
	
	// 定数文字列のハッシュ. コンパイル時解決.
	template<size_t SIZE>
	inline constexpr Hash64 GetHash64S(const char (&str)[SIZE], uint64_t seed = kDefaultHashSeed64)
	{
		return StaticInternalHash64(str, SIZE-1, seed);
	}

#if 0 // ポインタのキャストが出来ないのでconstexprで実装できない.

	// 普通の型のハッシュ. コンパイル時解決.
	template<typename T>
	inline constexpr Hash32 GetHash32S(const T& value, uint32_t seed = kDefaultHashSeed32)
	{
		return StaticInternalHash32(&value, sizeof(T), seed);
	}

	// 普通の型のハッシュ. コンパイル時解決.
	template<typename T>
	inline constexpr Hash64 GetHash64S(const T& value, uint64_t seed = kDefaultHashSeed64)
	{
		return StaticInternalHash64(&value, sizeof(T), seed);
	}
#endif

	////////////////////////////////////////////////////////////////////////////////////

	class Hash32Generator
	{
	public:
		Hash32Generator(uint32_t seed = kDefaultHashSeed32)
			: m_internal(seed)
		{
		}
				
		inline void Reset(uint32_t seed = kDefaultHashSeed32)
		{
			m_internal.Reset(seed);
		}

		inline void Add(const void* buffer, size_t bufferSizeInByte)
		{
			m_internal.Add(buffer, bufferSizeInByte);
		}
		
		// 普通の型のハッシュ.
		template<typename T>
		inline void Add(const T& value)
		{
			m_internal.Add(&value, sizeof(T));
		}
	
		// 文字列のハッシュ.
		inline void Add(const char*& str)
		{
			SI_ASSERT(str);
			m_internal.Add(str, strlen(str));
		}
	
		// 定数文字列のハッシュ.
		template<size_t SIZE>
		inline void Add(const char (&str)[SIZE])
		{
			m_internal.Add(str, SIZE-1);
		}

		inline Hash32 Generate() const
		{
			return m_internal.Generate();
		}

	private:
		InternalHash32Generator m_internal;
	};

	class Hash64Generator
	{
	public:
		Hash64Generator(uint64_t seed = kDefaultHashSeed64)
			: m_internal(seed)
		{
		}
		
		inline void Reset(uint64_t seed = kDefaultHashSeed64)
		{
			m_internal.Reset(seed);
		}

		inline void Add(const void* buffer, size_t bufferSizeInByte)
		{
			m_internal.Add(buffer, bufferSizeInByte);
		}
		
		// 普通の型のハッシュ.
		template<typename T>
		inline void Add(const T& value)
		{
			m_internal.Add(&value, sizeof(T));
		}
	
		// 文字列のハッシュ.
		inline void Add(const char*& str)
		{
			SI_ASSERT(str);
			m_internal.Add(str, strlen(str));
		}
	
		// 定数文字列のハッシュ.
		template<size_t SIZE>
		inline void Add(const char (&str)[SIZE])
		{
			m_internal.Add(str, SIZE-1);
		}

		inline Hash64 Generate() const
		{
			return m_internal.Generate();
		}

	private:
		InternalHash64Generator m_internal;
	};

} // namespace SI
