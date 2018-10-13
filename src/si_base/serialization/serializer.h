#pragma once

#include <cstdint>
#include "si_base/serialization/reflection.h"

namespace SI
{
	class SerializerImpl;

	class Serializer
	{
	public:
		Serializer();
		~Serializer();
		
		void Initialize();
		void Terminate();

		template<typename T>
		void Serialize(const char* path, const T& obj)
		{
			SerializeRoot(path, &obj, GetReflectionType<T>(0));
		}

	private:
		void  SerializeRoot(const char* path, const void* buffer, const ReflectionType& reflection);

	private:
		SerializerImpl* m_impl;
	};

} // namespace SI
