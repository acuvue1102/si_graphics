#pragma once

#include <cstdint>
#include <string>

#pragma warning(disable:4307) // '*': 整数定数がオーバーフローしました。

#define USE_FNV_HASH 0
#define USE_MURMUR2A_HASH 1

namespace SI
{
	using Hash32 = uint32_t;
	using Hash64 = uint64_t;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fnv-1 Hashの実装.
	static const uint32_t kFnvBasis32 = 2166136261u;
	static const uint64_t kFnvBasis64 = 14695981039346656037ull;

	static const uint32_t kFnvPrime32 = 16777619u;
	static const uint64_t kFnvPrime64 = 1099511628211ull;

	static const Hash32 kDefaultHashSeed32 = kFnvBasis32;
	static const Hash64 kDefaultHashSeed64 = kFnvBasis64;

	inline Hash32 FnvHash32(const void* buffer, size_t bufferSizeInByte, uint32_t seed = kDefaultHashSeed32)
	{
		uint32_t hash = seed;

		const uint8_t* buf = (const uint8_t*)buffer;
		for(size_t i=0 ; i<bufferSizeInByte; ++i)
		{
			hash = (kFnvPrime32 * hash) ^ (buf[i]);
		}
 
		return hash;
	}

	inline Hash64 FnvHash64(const void* buffer, size_t bufferSizeInByte, uint64_t seed = kDefaultHashSeed64)
	{
		uint64_t hash = seed;

		const uint8_t* buf = (const uint8_t*)buffer;
		for(size_t i=0 ; i<bufferSizeInByte; ++i)
		{
			hash = (kFnvPrime64 * hash) ^ (buf[i]);
		}
 
		return hash;
	}	
	
	inline constexpr Hash32 StaticFnvHash32(const void* buffer, size_t bufferSizeInByte, uint32_t seed = kDefaultHashSeed32)
	{
		// const uint8_t*を使うとコンパイルが通らないので、const char*経由で処理している.
		return (bufferSizeInByte==0)?
			seed :
			StaticFnvHash32(
				&((const char*)buffer)[1],
				bufferSizeInByte - 1,
				(kFnvPrime32 * seed) ^ (uint8_t)(((const char*)buffer)[0]) );
	}
	
	inline constexpr Hash64 StaticFnvHash64(const void* buffer, size_t bufferSizeInByte, uint64_t seed = kDefaultHashSeed64)
	{
		// const uint8_t*を使うとコンパイルが通らないので、const char*経由で処理している.
		return (bufferSizeInByte==0)?
			seed :
			StaticFnvHash64(
				&(((const char*)buffer)[1]),
				bufferSizeInByte - 1,
				(kFnvPrime64 * seed) ^ (uint8_t)(((const char*)buffer)[0]) );
	}

	class FnvHash32Generator
	{
	public:
		FnvHash32Generator(uint32_t seed = kDefaultHashSeed32)
			: m_hash(seed)
		{
		}

		void Reset(uint32_t seed = kDefaultHashSeed32)
		{
			m_hash = seed;
		}

		void Add(const void* buffer, size_t bufferSizeInByte)
		{
			const uint8_t* buf = (const uint8_t*)buffer;
			for(size_t i=0 ; i<bufferSizeInByte; ++i)
			{
				m_hash = (kFnvPrime32 * m_hash) ^ (buf[i]);
			}
		}

		Hash32 Generate() const
		{
			return m_hash;
		}

	private:
		Hash32 m_hash;
	};
	
	class FnvHash64Generator
	{
	public:
		FnvHash64Generator(uint64_t seed = kDefaultHashSeed64)
			: m_hash(seed)
		{
		}

		void Reset(uint64_t seed = kDefaultHashSeed64)
		{
			m_hash = seed;
		}

		void Add(const void* buffer, size_t bufferSizeInByte)
		{
			const uint8_t* buf = (const uint8_t*)buffer;
			for(size_t i=0 ; i<bufferSizeInByte; ++i)
			{
				m_hash = (kFnvPrime64 * m_hash) ^ (buf[i]);
			}
		}

		Hash64 Generate() const
		{
			return m_hash;
		}

	private:
		Hash64 m_hash;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
 
	static const uint32_t kMurmur2APrime32 = 0x5bd1e995;
	static const uint64_t kMurmur2APrime64 = 0xc6a4a7935bd1e995;
	
	static const uint32_t kMumur2Rotate32 = 24;
	static const uint64_t kMumur2Rotate64 = 47;
	
	#define MURMUR2A_MIX32(_hash, _key)\
	{\
		_key *= kMurmur2APrime32;\
		_key ^= _key >> kMumur2Rotate32;\
		_key *= kMurmur2APrime32;\
		_hash *= kMurmur2APrime32;\
		_hash ^= _key;\
	}

	#define MURMUR2A_MIX64(_hash, _key)\
	{\
		_key *= kMurmur2APrime64;\
		_key ^= _key >> kMumur2Rotate32;\
		_key *= kMurmur2APrime64;\
		_hash ^= _key; /*32bit版と↓の処理順が逆みたいなので注意.*/ \
		_hash *= kMurmur2APrime64;\
	}

	inline Hash32 Murmur2AHash32(const void* buffer, size_t bufferSizeInByte, uint32_t seed = kDefaultHashSeed32)
	{
		Hash32 hash = seed;

		const uint8_t* data = (const uint8_t *)buffer;
		const uint8_t* end  = data + (bufferSizeInByte/4) * 4;

		if(((uintptr_t)data & 3) == 0) // 入力バッファのalignment大丈夫.
		{
			while(data != end)
			{
				uint32_t key = *(const uint32_t*)data;

				MURMUR2A_MIX32(hash, key);
				
				data += 4;
			}
		}
		else
		{

			while(data != end)
			{
				// 入力バッファのalignmentが4byteアライメントじゃないので8byteづつ読む.
				// Little Endianを前提に書いてる.
				uint32_t key =
					(uint32_t(data[0])      ) |
					(uint32_t(data[1]) << 8 ) |
					(uint32_t(data[2]) << 16) |
					(uint32_t(data[3]) << 24);
				
				MURMUR2A_MIX32(hash, key);

				data += 4;
			}
		}

		switch(bufferSizeInByte & 3)
		{
		case 3: hash ^= data[2] << 16;
		case 2: hash ^= data[1] << 8;
		case 1: hash ^= data[0];
				hash *= kMurmur2APrime32;
		};
		
		{
			// Murmur2AなのでSizeを後から合成する.
			uint32_t key = (uint32_t)bufferSizeInByte;
			
			MURMUR2A_MIX32(hash, key);
		}

		hash ^= hash >> 13;
		hash *= kMurmur2APrime32;
		hash ^= hash >> 15;

		return hash;
	}

	inline Hash64 Murmur2AHash64(const void* buffer, size_t bufferSizeInByte, uint64_t seed = kDefaultHashSeed64)
	{
		uint64_t hash = seed;
		
		const uint8_t* data = (const uint8_t *)buffer;
		const uint8_t* end  = data + (bufferSizeInByte/8) * 8;
		
		if(((uintptr_t)data & 7) == 0) // 入力バッファのalignment大丈夫.
		{
			while(data != end)
			{
				uint64_t key = *(const uint64_t*)data;
				
				MURMUR2A_MIX64(hash, key);

				data += 8;
			}
		}
		else
		{
			while(data != end)
			{
				// 入力バッファのalignmentが4byteアライメントじゃないので8byteづつ読む.
				// Little Endianを前提に書いてる.
				uint64_t key = 
					(uint64_t(data[0])      ) |
					(uint64_t(data[1]) <<  8) |
					(uint64_t(data[2]) << 16) |
					(uint64_t(data[3]) << 24) |
					(uint64_t(data[4]) << 32) |
					(uint64_t(data[5]) << 40) |
					(uint64_t(data[6]) << 48) |
					(uint64_t(data[7]) << 56);
				
				MURMUR2A_MIX64(hash, key);

				data += 8;
			}
		}

		switch(bufferSizeInByte & 7)
		{
		case 7: hash ^= uint64_t(data[6]) << 48;
		case 6: hash ^= uint64_t(data[5]) << 40;
		case 5: hash ^= uint64_t(data[4]) << 32;
		case 4: hash ^= uint64_t(data[3]) << 24;
		case 3: hash ^= uint64_t(data[2]) << 16;
		case 2: hash ^= uint64_t(data[1]) << 8;
		case 1: hash ^= uint64_t(data[0]);
				hash *= kMurmur2APrime64;
		};
		
		{
			// Murmur2AなのでSizeを後から合成する.
			uint64_t key = (uint64_t)bufferSizeInByte;
			
			MURMUR2A_MIX64(hash, key);
		}

 
		hash ^= hash >> kMumur2Rotate64; // 32bit版と最後のRotateが違うみたい.
		hash *= kMurmur2APrime64;
		hash ^= hash >> kMumur2Rotate64; // 32bit版と最後のRotateが違うみたい.

		return hash;
	}

	// c++17じゃないとビルド通らないと思われる.
	inline constexpr Hash32 StaticMurmur2AHash32(const char* buffer, size_t bufferSizeInByte, uint32_t seed = kDefaultHashSeed32)
	{
		// const uint8_t*を使うとコンパイルが通らないので、const char*経由で処理している.
		Hash32 hash = seed;

		const char* data = (const char *)buffer;
		const char* end  = data + (bufferSizeInByte/4) * 4;

		while(data != end)
		{
			// 入力バッファのalignmentが4byteアライメントじゃないので8byteづつ読む.
			// Little Endianを前提に書いてる.
			uint32_t key =
				(uint32_t((uint8_t)data[0])      ) |
				(uint32_t((uint8_t)data[1]) << 8 ) |
				(uint32_t((uint8_t)data[2]) << 16) |
				(uint32_t((uint8_t)data[3]) << 24);
			
			MURMUR2A_MIX32(hash, key);

			data += 4;
		}

		switch(bufferSizeInByte & 3)
		{
		case 3: hash ^= uint32_t((uint8_t)data[2]) << 16;
		case 2: hash ^= uint32_t((uint8_t)data[1]) << 8;
		case 1: hash ^= uint32_t((uint8_t)data[0]);
				hash *= kMurmur2APrime32;
		};
		
		{
			// Murmur2AなのでSizeを後から合成する.
			uint32_t key = (uint32_t)bufferSizeInByte;
			
			MURMUR2A_MIX32(hash, key);
		}

		hash ^= hash >> 13;
		hash *= kMurmur2APrime32;
		hash ^= hash >> 15;

		return hash;
	}
	
	// c++17じゃないとビルド通らないと思われる.
	inline constexpr Hash64 StaticMurmur2AHash64(const char* buffer, size_t bufferSizeInByte, uint64_t seed = kDefaultHashSeed64)
	{
		// const uint8_t*を使うとコンパイルが通らないので、const char*経由で処理している.
		
		uint64_t hash = seed;
		const char* data = (const char *)buffer;
		const char* end  = data + (bufferSizeInByte/8) * 8;
		
		while(data != end)
		{
			// 入力バッファのalignmentが4byteアライメントじゃないので8byteづつ読む.
			// Little Endianを前提に書いてる.
			uint64_t key = 
				(uint64_t((uint8_t)data[0])      ) |
				(uint64_t((uint8_t)data[1]) <<  8) |
				(uint64_t((uint8_t)data[2]) << 16) |
				(uint64_t((uint8_t)data[3]) << 24) |
				(uint64_t((uint8_t)data[4]) << 32) |
				(uint64_t((uint8_t)data[5]) << 40) |
				(uint64_t((uint8_t)data[6]) << 48) |
				(uint64_t((uint8_t)data[7]) << 56);
			
			MURMUR2A_MIX64(hash, key);

			data += 8;
		}

		switch(bufferSizeInByte & 7)
		{
		case 7: hash ^= uint64_t((uint8_t)data[6]) << 48;
		case 6: hash ^= uint64_t((uint8_t)data[5]) << 40;
		case 5: hash ^= uint64_t((uint8_t)data[4]) << 32;
		case 4: hash ^= uint64_t((uint8_t)data[3]) << 24;
		case 3: hash ^= uint64_t((uint8_t)data[2]) << 16;
		case 2: hash ^= uint64_t((uint8_t)data[1]) << 8;
		case 1: hash ^= uint64_t((uint8_t)data[0]);
				hash *= kMurmur2APrime64;
		};

		{
			// Murmur2AなのでSizeを後から合成する.
			uint64_t key = (uint64_t)bufferSizeInByte;
			
			MURMUR2A_MIX64(hash, key);
		}
 
		hash ^= hash >> kMumur2Rotate64; // 32bit版と最後のRotateが違うみたい.
		hash *= kMurmur2APrime64;
		hash ^= hash >> kMumur2Rotate64; // 32bit版と最後のRotateが違うみたい.

		return hash;
	}

	class Murmur2AHash32Generator
	{
	public:
		Murmur2AHash32Generator(uint32_t seed = kDefaultHashSeed32)
			: m_size(0)
			, m_hash(seed)
			, m_tail(0)
			, m_tailCount(0)
		{
		}

		void Reset(uint32_t seed = kDefaultHashSeed32)
		{
			m_size = 0;
			m_hash = seed;
			m_tail = 0;
			m_tailCount = 0;
		}

		void Add(const void* buffer, size_t bufferSizeInByte)
		{
			m_size += bufferSizeInByte;
			const uint8_t* data = (const uint8_t*)buffer;

			if(((uintptr_t)data & 3) == 0 && m_tailCount == 0)
			{
				// アラインメント取れてて、tailも無かったら早く走査できる.
				const uint8_t* end  = data + (bufferSizeInByte/4) * 4;
				while(data != end)
				{
					uint32_t key = *(const uint32_t*)data;
					
					MURMUR2A_MIX32(m_hash, key);
				
					data += 4;
				}
			}
			
			{
				const uint8_t* end  = ((const uint8_t*)buffer) + bufferSizeInByte;

				while(data != end)
				{
					for(; m_tailCount<4 && data!=end; ++m_tailCount)
					{
						m_tail |= (uint32_t(*data) << 8*m_tailCount);
						++data;
					}

					if(m_tailCount==4)
					{
						uint32_t key = m_tail;
						
						MURMUR2A_MIX32(m_hash, key);

						m_tail = 0;
						m_tailCount = 0;
					}
				}
			}
		}

		Hash32 Generate() const
		{
			uint32_t hash = m_hash;
			
			if(0<m_tailCount)
			{
				hash ^= m_tail;
				hash *= kMurmur2APrime32;
			}

			{
				// Murmur2AなのでSizeを後から合成する.
				uint32_t key = (uint32_t)m_size;
				
				MURMUR2A_MIX32(hash, key);
			}

			hash ^= hash >> 13;
			hash *= kMurmur2APrime32;
			hash ^= hash >> 15;

			return hash;
		}

	private:
		size_t      m_size;
		uint32_t    m_hash;
		uint32_t    m_tail;
		uint32_t    m_tailCount;
	};
	
	class Murmur2AHash64Generator
	{
	public:
		Murmur2AHash64Generator(uint64_t seed = kDefaultHashSeed64)
			: m_size(0)
			, m_hash(seed)
			, m_tail(0)
			, m_tailCount(0)
		{
		}

		void Reset(uint64_t seed = kDefaultHashSeed64)
		{
			m_size = 0;
			m_hash = seed;
			m_tail = 0;
			m_tailCount = 0;
		}

		void Add(const void* buffer, size_t bufferSizeInByte)
		{
			m_size += bufferSizeInByte;
			const uint8_t* data = (const uint8_t*)buffer;

			if(((uintptr_t)data & 7) == 0 && m_tailCount == 0)
			{
				// アラインメント取れてて、tailも無かったら早く走査できる.
				const uint8_t* end  = data + (bufferSizeInByte/8) * 8;
				while(data != end)
				{
					uint64_t key = *(const uint64_t*)data;
					
					MURMUR2A_MIX64(m_hash, key);

					data += 8;
				}
			}
			
			// 
			{
				const uint8_t* end  = ((const uint8_t*)buffer) + bufferSizeInByte;

				while(data != end)
				{
					for(; m_tailCount<8 && data!=end; ++m_tailCount)
					{
						m_tail |= (uint64_t(*data) << 8*m_tailCount);
						++data;
					}

					if(m_tailCount==8)
					{
						uint64_t key = m_tail;
						
						MURMUR2A_MIX64(m_hash, key);

						m_tail = 0;
						m_tailCount = 0;
					}
				}
			}
		}

		Hash64 Generate() const
		{
			uint64_t hash = m_hash;
			
			if(0<m_tailCount)
			{
				hash ^= m_tail;
				hash *= kMurmur2APrime64;
			}

			{
				// Murmur2AなのでSizeを後から合成する.
				uint64_t key = (uint64_t)m_size;
				
				MURMUR2A_MIX64(hash, key);
			}

			hash ^= hash >> kMumur2Rotate64; // 32bit版と最後のRotateが違うみたい.
			hash *= kMurmur2APrime64;
			hash ^= hash >> kMumur2Rotate64; // 32bit版と最後のRotateが違うみたい.

			return hash;
		}

	private:
		size_t      m_size;
		uint64_t    m_hash;
		uint64_t    m_tail;
		uint32_t    m_tailCount;
	};
	
#if USE_FNV_HASH
	inline Hash32 InternalHash32(const void* buffer, size_t bufferSizeInByte, uint32_t seed = kDefaultHashSeed32)
	{ 
		return FnvHash32(buffer, bufferSizeInByte, seed);
	}

	inline Hash64 InternalHash64(const void* buffer, size_t bufferSizeInByte, uint64_t seed = kDefaultHashSeed64)
	{
		return FnvHash64(buffer, bufferSizeInByte, seed);
	}

	inline constexpr Hash32 StaticInternalHash32(const void* buffer, size_t bufferSizeInByte, uint32_t seed = kDefaultHashSeed32)
	{
		return StaticFnvHash32(buffer, bufferSizeInByte, seed);
	}
	
	inline constexpr Hash64 StaticInternalHash64(const void* buffer, size_t bufferSizeInByte, uint64_t seed = kDefaultHashSeed64)
	{
		return StaticFnvHash64(buffer, bufferSizeInByte, seed);
	}
	
	using InternalHash32Generator = FnvHash32Generator;
	using InternalHash64Generator = FnvHash64Generator;

#elif USE_MURMUR2A_HASH

	inline Hash32 InternalHash32(const void* buffer, size_t bufferSizeInByte, uint32_t seed = kDefaultHashSeed32)
	{ 
		return Murmur2AHash32(buffer, bufferSizeInByte, seed);
	}

	inline Hash64 InternalHash64(const void* buffer, size_t bufferSizeInByte, uint64_t seed = kDefaultHashSeed64)
	{
		return Murmur2AHash64(buffer, bufferSizeInByte, seed);
	}

	inline constexpr Hash32 StaticInternalHash32(const char* buffer, size_t bufferSizeInByte, uint32_t seed = kDefaultHashSeed32)
	{
		return StaticMurmur2AHash32(buffer, bufferSizeInByte, seed);
	}
	
	inline constexpr Hash64 StaticInternalHash64(const char* buffer, size_t bufferSizeInByte, uint64_t seed = kDefaultHashSeed64)
	{
		return StaticMurmur2AHash64(buffer, bufferSizeInByte, seed);
	}
	
	using InternalHash32Generator = Murmur2AHash32Generator;
	using InternalHash64Generator = Murmur2AHash64Generator;

#else
#error "Hash algorithm required."
#endif

} // namespace SI
