#pragma once

#include <cstdint>
#include <vector>
#include "si_base/core/non_copyable.h"
#include "si_base/serialization/reflection.h"
#include "si_base/core/new_delete.h"

namespace SI
{
	class DeserializerImpl;

	class DeserializedObject : private NonCopyable
	{
	private:
		struct AllocatedBuffer
		{
			void*                     m_buffer;
			const SI::ReflectionType* m_type;
			uint32_t                  m_arrayCount;
		};

	public:
		DeserializedObject(void* object = nullptr, const ReflectionType* type = nullptr)
			: m_object(object)
			, m_type(type)
		{
		}

		DeserializedObject(DeserializedObject && src)
		{
			*this = std::move(src);
		}

		DeserializedObject& operator=(DeserializedObject && src)
		{
			Release();

			m_object = src.m_object;
			src.m_object = nullptr;
			m_type = src.m_type;
			src.m_type = nullptr;

			m_allocatedBuffers = std::move(src.m_allocatedBuffers);

			return *this;
		}

		~DeserializedObject()
		{
			Release();
		}

		void Release()
		{
			if(m_allocatedBuffers.empty()) return;
			
			auto end = m_allocatedBuffers.rend();
			for(auto itr = m_allocatedBuffers.rbegin(); itr!=end; ++itr)
			{
				AllocatedBuffer& b = *itr;
				uint32_t count = Max(b.m_arrayCount, 1u);
				
				if(b.m_type)
				{
					uint32_t size = b.m_type->GetSize();
					for(uint32_t i=0; i<count; ++i)
					{
						void* buffer = (void*)(((uint8_t*)b.m_buffer) + size * i);

						b.m_type->Destructor(buffer);
					}
				}

				SI_ALIGNED_FREE(b.m_buffer);
			}

			m_allocatedBuffers.clear();
		}
		
		template<typename T>
		const T* Get() const
		{
			SI_ASSERT(&GetReflectionType<T>(0) == m_type);
			return (const T*)m_object;
		}

		void AddAllocatedBuffer(void* buffer, const SI::ReflectionType* type, uint32_t arrayCount)
		{
			m_allocatedBuffers.emplace_back();
			AllocatedBuffer& item = m_allocatedBuffers.back();
			item.m_buffer = buffer;
			item.m_type = type;
			item.m_arrayCount = arrayCount;
		}

	private:
		void* m_object;
		const SI::ReflectionType* m_type;
		std::vector<AllocatedBuffer> m_allocatedBuffers;
	};

	class Deserializer
	{
	public:
		Deserializer();
		~Deserializer();
		
		void Initialize();
		void Terminate();

		template<typename T>
		bool Deserialize(DeserializedObject& outDeserializedObject, const char* path)
		{
			return DeserializeRoot(outDeserializedObject, path, GetReflectionType<T>(0));
		}

	private:
		bool DeserializeRoot(DeserializedObject& outDeserializedObject, const char* path, const ReflectionType& reflection);

	private:
		DeserializerImpl* m_impl;
	};

} // namespace SI
