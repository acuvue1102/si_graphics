#pragma once

#include <cstdint>
#include <string>
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
		inline bool Serialize(std::string& outString, const T& obj)
		{
			return SerializeRoot(outString, &obj, GetReflectionType<T>(0));
		}

		template<typename T>
		inline bool Serialize(const char* path, const T& obj)
		{
			std::string str;
			if(!Serialize(str, obj)) return false;

			return Save(path, str.c_str(), str.size());
		}

	private:
		bool  SerializeRoot(std::string& outString, const void* buffer, const ReflectionType& reflection);

	private:
		static bool Save(const char* path, const char* str, size_t strSize);

	private:
		SerializerImpl* m_impl;
	};

} // namespace SI
