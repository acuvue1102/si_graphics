#pragma once

#include <cstdint>
#include "si_base/serialization/reflection.h"

namespace SI
{
	class DeserializerImpl;

	class Deserializer
	{
	public:
		Deserializer();
		~Deserializer();
		
		void Initialize();
		void Terminate();

		template<typename T>
		T* Deserialize(const char* path)
		{
			return (T*)DeserializeRoot(path, GetReflectionType<T>(0));
		}

	private:
		void* DeserializeRoot(const char* path, const ReflectionType& reflection);

	private:
		DeserializerImpl* m_impl;
	};

} // namespace SI
