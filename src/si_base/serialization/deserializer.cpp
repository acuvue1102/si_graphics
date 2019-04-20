
#include "si_base/serialization/Deserializer.h"

#include <map>

#define PICOJSON_USE_INT64
#include "external/picojson/picojson.h"

#include "si_base/core/print.h"
#include "si_base/file/file.h"
#include "si_base/container/array.h"

namespace SI
{
	namespace
	{
		class DynamicReflectionMember
		{
			DynamicReflectionMember() = delete;
		public:
			DynamicReflectionMember(
				std::string name,
				std::string typeName,
				uint32_t arrayCount,
				uint32_t pointerCount)
				: m_name(std::move(name))
				, m_nameHash(SI::GetHash64(m_name.c_str()))
				, m_typeName(std::move(typeName))
				, m_arrayCount(arrayCount)
				, m_pointerCount(pointerCount)
			{
			}
			
			std::string m_name;
			SI::Hash64  m_nameHash;
			std::string m_typeName;
			uint32_t m_arrayCount;
			uint32_t m_pointerCount;
		};

		class DynamicReflectionType
		{
		public:
			DynamicReflectionType()
				: m_nameHash(0)
			{
			}

			void SetName(std::string name)
			{
				m_name = std::move(name);
				m_nameHash = SI::GetHash64(m_name.c_str());
			}

			const std::string& GetName() const
			{
				return m_name;
			}

			SI::Hash64 GetNameHash() const
			{
				return m_nameHash;
			}

			void ReserveMember(size_t size)
			{
				m_members.reserve(size);
			}

			void AddMember(
				std::string name,
				std::string typeName,
				uint32_t arrayCount,
				uint32_t pointerCount)
			{
				m_members.emplace_back(std::move(name), std::move(typeName), arrayCount, pointerCount);
			}

			size_t GetMemberCount() const
			{
				return m_members.size();
			}

			const DynamicReflectionMember& GetMember(size_t index) const
			{
				return m_members[index];
			}			

		private:
			std::string m_name;
			SI::Hash64  m_nameHash;
			std::vector<DynamicReflectionMember> m_members;
		};
	}

	////////////////////////////////////////////////////////////////////////////////

	class DeserializerImpl
	{
	public:
		DeserializerImpl()
		{
			// 基本型をあらかじめ登録しておいて、出力されないようにする.
			m_typeTable.insert( std::make_pair("char",          nullptr) );
			m_typeTable.insert( std::make_pair("int8_t",        nullptr) );
			m_typeTable.insert( std::make_pair("int16_t",       nullptr) );
			m_typeTable.insert( std::make_pair("int32_t",       nullptr) );
			m_typeTable.insert( std::make_pair("int64_t",       nullptr) );
			m_typeTable.insert( std::make_pair("uint8_t",       nullptr) );
			m_typeTable.insert( std::make_pair("uint16_t",      nullptr) );
			m_typeTable.insert( std::make_pair("uint32_t",      nullptr) );
			m_typeTable.insert( std::make_pair("uint64_t",      nullptr) );
			m_typeTable.insert( std::make_pair("float",         nullptr) );
			m_typeTable.insert( std::make_pair("double",        nullptr) );
			m_typeTable.insert( std::make_pair("bool",          nullptr) );
			m_typeTable.insert( std::make_pair("SI::Vfloat",    nullptr) );
			m_typeTable.insert( std::make_pair("SI::Vquat",     nullptr) );
			m_typeTable.insert( std::make_pair("SI::Vfloat3",   nullptr) );
			m_typeTable.insert( std::make_pair("SI::Vfloat4",   nullptr) );
			m_typeTable.insert( std::make_pair("SI::Vfloat4x3", nullptr) );
			m_typeTable.insert( std::make_pair("SI::Vfloat4x4", nullptr) );
		}

		~DeserializerImpl()
		{
		}
		
		bool DeserializeRoot(
			DeserializedObject& outDeserializedObject,
			const char* path,
			const ReflectionType& reflection)
		{
			// ファイルに出力.
			std::string inputStr;
			{
				std::vector<uint8_t> buffer;
				
				File f;
				int ret = f.Open(path, SI::FileAccessType::Read);
				if(ret!=0)
				{
					SI_WARNING("file(%s) can't be loaded.", path);
					return false;
				}

				int64_t size = f.GetFileSize();
				buffer.resize(size);

				f.Read(&buffer[0], size, nullptr);

				f.Close();

				// utf-8 with BOM
				const uint8_t bom[3] = {0xEF, 0xBB, 0xBF};
				if( 3<buffer.size() &&
					buffer[0] == bom[0] &&
					buffer[1] == bom[1] &&
					buffer[2] == bom[2])
				{
					inputStr = (const char*)&buffer[3];
				}
				else
				{
					inputStr = (const char*)&buffer[0];
				}
			}

			picojson::value picoValue;
			std::string err = picojson::parse(picoValue, inputStr);
			if(!err.empty())
			{
				SI_WARNING(0, "%s\n", err.c_str());
				return false;
			}
			
			picojson::object& topObj = picoValue.get<picojson::object>();
			
			if(!RegisterRefelenceType(reflection)) return false;

			auto typeTableItr = topObj.find("typeTable");
			if(typeTableItr == topObj.end()){ return false; }
			if(!DeserializeTypeTable(typeTableItr->second.get<picojson::object>())){ return false; }
			//
			auto objectItr = topObj.find("object");
			if(objectItr == topObj.end()){ return false; }
			
			void* buffer = SI_ALIGNED_MALLOC( reflection.GetSize(), reflection.GetAlignment() );
			reflection.Constructor(buffer);

			DeserializedObject outObject(buffer, &reflection);
			outObject.AddAllocatedBuffer(buffer, &reflection, 0);

			const picojson::object& picoRootObject = objectItr->second.get<picojson::object>();
			SI_ASSERT(!picoRootObject.empty());
			const picojson::array& picoRootMemberArray = picoRootObject.begin()->second.get<picojson::array>();
			bool ret = DeserializeObject(outObject, buffer, picoRootMemberArray, reflection);

			if(!ret)
			{
				// outObjectはreleaseされる.
				return false;
			}

			outDeserializedObject = std::move(outObject);		
			return true;
		}

		bool DeserializeTypeTable(picojson::object& typeTable)
		{
			m_dynamicTypes.reserve(m_dynamicTypes.size()+typeTable.size());

			for(auto itr = typeTable.begin(); itr != typeTable.end(); ++itr)
			{
				m_dynamicTypes.emplace_back();
				DynamicReflectionType& reflection = m_dynamicTypes.back();
				std::string name = itr->first;
				m_dynamicTypeTable.insert( std::make_pair(itr->first, &reflection) );
			
				reflection.SetName(itr->first);
				picojson::array& picoMembers = itr->second.get<picojson::array>();

				reflection.ReserveMember(picoMembers .size());
				for(auto itr2 = picoMembers.begin(); itr2 != picoMembers.end(); ++itr2)
				{
					picojson::object& picoMember = itr2->get<picojson::object>();
					
					uint32_t arrayCount = 0;
					auto arrayCountItr = picoMember.find("arrayCount");
					if(arrayCountItr != picoMember.end())
					{
						arrayCount = (uint32_t)arrayCountItr->second.get<double>();
					}
				
					uint32_t pointerCount = 0;
					auto pointerCountItr = picoMember.find("pointerCount");
					if(pointerCountItr != picoMember.end())
					{
						pointerCount = (uint32_t)pointerCountItr->second.get<double>();
					}
								
					std::string name;
					auto nameItr = picoMember.find("name");
					if(nameItr != picoMember.end())
					{
						name = nameItr->second.get<std::string>();
					}
								
					std::string typeName;
					auto typeNameItr = picoMember.find("type");
					if(typeNameItr != picoMember.end())
					{
						typeName = typeNameItr->second.get<std::string>();
					}

					reflection.AddMember(std::move(name), std::move(typeName), arrayCount, pointerCount);
				}

				SI_PRINT("");
			}

			return true;
		}
	
		bool RegisterRefelenceType(const ReflectionType& reflection)
		{
			const char* name = reflection.GetName();
			auto itr = m_typeTable.find(std::string(name));
			if(itr != m_typeTable.end()) return true; // 登録済み

			uint32_t memberCount = reflection.GetMemberCount();

			for(uint32_t m=0; m<memberCount; ++m)
			{
				const ReflectionMember* member = reflection.GetMember(m);
				if(!member) continue;
				const ReflectionType& memberReflection = member->GetType();
				
				RegisterRefelenceType(memberReflection);
			}
			
			m_typeTable.insert( std::make_pair(name, &reflection) );

			return true;
		}
		
		void DeserializeType(
			DeserializedObject& outDeserializedObject,
			void* buffer,
			const picojson::value& picoValue,
			const ReflectionType& reflection,
			uint32_t pointerCount,
			bool inArray)
		{
			Hash64 typeNameHash = reflection.GetNameHash();
			
			if(0<pointerCount)
			{
				// 文字列の時だけは特別扱い.
				if(pointerCount==1 && typeNameHash == GetHash64S("char"))
				{
					SI_ASSERT(strcmp(reflection.GetName(), "char") == 0);
					
					std::string str = picoValue.get<std::string>();
					size_t strLength = str.size();
					
					char*& pointerBuffer = *((char**)buffer);
					pointerBuffer = (char*)SI_ALIGNED_MALLOC(sizeof(char)*(strLength+1), alignof(char*));
					outDeserializedObject.AddAllocatedBuffer(pointerBuffer, nullptr, 0); // arrayだけどいいだろう.

					memcpy(pointerBuffer, str.c_str(), strLength);
					pointerBuffer[strLength] = 0; // 終端.
					return;
				}
				else if(pointerCount==1)
				{
					// 指定の型で領域確保.
					void*& pointerBuffer = *((void**)buffer);
					pointerBuffer = SI_ALIGNED_MALLOC(reflection.GetSize(), reflection.GetAlignment());
					outDeserializedObject.AddAllocatedBuffer(pointerBuffer, &reflection, 0);

					reflection.Constructor(pointerBuffer);

					return DeserializeType(
						outDeserializedObject,
						pointerBuffer,
						picoValue,
						reflection,
						pointerCount-1,
						inArray);
				}
				else
				{
					// ポインタ外し.
					void*& pointerBuffer = *((void**)buffer);
					pointerBuffer = SI_ALIGNED_MALLOC(sizeof(void*), alignof(void*));
					outDeserializedObject.AddAllocatedBuffer(pointerBuffer, nullptr, 0);

					return DeserializeType(
						outDeserializedObject,
						pointerBuffer,
						picoValue,
						reflection,
						pointerCount-1,
						inArray);
				}
			}

			if(typeNameHash == GetHash64S("int8_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int8_t") == 0);
				int8_t memberData = (int8_t)picoValue.get<double>();
				*(int8_t*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("char"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "char") == 0);
				char memberData = (int8_t)picoValue.get<double>();
				*(char*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("uint8_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "uint8_t") == 0);
				uint8_t memberData = (int8_t)picoValue.get<double>();
				*(uint8_t*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("int16_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int16_t") == 0);
				int16_t memberData = (int16_t)picoValue.get<double>();
				*(int16_t*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("uint16_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "uint16_t") == 0);
				uint16_t memberData = (uint16_t)picoValue.get<double>();
				*(uint16_t*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("int32_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int32_t") == 0);
				int32_t memberData = (int32_t)picoValue.get<double>();
				*(int32_t*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("uint32_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "uint32_t") == 0);
				uint32_t memberData = (uint32_t)picoValue.get<double>();
				*(uint32_t*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("int64_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int64_t") == 0);
				int64_t memberData = (int64_t)picoValue.get<double>();
				*(int64_t*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("uint64_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "uint64_t") == 0);
				uint64_t memberData = (uint64_t)picoValue.get<double>();
				*(uint64_t*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("float"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "float") == 0);
				float memberData = (float)picoValue.get<double>();
				*(float*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("double"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "double") == 0);
				double memberData = picoValue.get<double>();
				*(double*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("bool"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "bool") == 0);
				bool memberData = picoValue.get<bool>();
				*(bool*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat") == 0);
				SI::Vfloat memberData = SI::Vfloat((float)picoValue.get<double>());
				*(SI::Vfloat*)buffer = memberData;
			}
			else if(typeNameHash == GetHash64S("SI::Vquat"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vquat") == 0);
				picojson::array picoArray = picoValue.get<picojson::array>();
				if(picoArray.size()==4)
				{
					SI::Vquat& q = *(SI::Vquat*)buffer;
					q.Set((float)picoArray[0].get<double>(),
						  (float)picoArray[1].get<double>(),
						  (float)picoArray[2].get<double>(),
						  (float)picoArray[3].get<double>());
				}
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat3"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat3") == 0);
				picojson::array picoArray = picoValue.get<picojson::array>();
				if(picoArray.size()==3)
				{
					SI::Vfloat3& v = *(SI::Vfloat3*)buffer;
					v.Set((float)picoArray[0].get<double>(),
						  (float)picoArray[1].get<double>(),
						  (float)picoArray[2].get<double>());
				}
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat4"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat4") == 0);
				picojson::array picoArray = picoValue.get<picojson::array>();
				if(picoArray.size()==4)
				{
					SI::Vfloat4& v = *(SI::Vfloat4*)buffer;
					v.Set((float)picoArray[0].get<double>(),
						  (float)picoArray[1].get<double>(),
						  (float)picoArray[2].get<double>(),
						  (float)picoArray[3].get<double>());
				}
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat3x3"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat3x3") == 0);
				picojson::array picoArray = picoValue.get<picojson::array>();
				if(picoArray.size()==9)
				{
					SI::Vfloat3x3& m = *(SI::Vfloat3x3*)buffer;
					for(uint32_t i=0; i<3; ++i)
					{
						SI::Vfloat3 v(
							(float)picoArray[i*3 + 0].get<double>(),
							(float)picoArray[i*3 + 1].get<double>(),
							(float)picoArray[i*3 + 2].get<double>());

						m.SetRow(i, v);
					}
				}
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat4x3"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat4x3") == 0);
				picojson::array picoArray = picoValue.get<picojson::array>();
				if(picoArray.size()==12)
				{
					SI::Vfloat4x3& m = *(SI::Vfloat4x3*)buffer;
					for(uint32_t i=0; i<4; ++i)
					{
						SI::Vfloat3 v(
							(float)picoArray[i*3 + 0].get<double>(),
							(float)picoArray[i*3 + 1].get<double>(),
							(float)picoArray[i*3 + 2].get<double>());

						m.SetRow(i, v);
					}
				}
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat4x4"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat4x4") == 0);
				picojson::array picoArray = picoValue.get<picojson::array>();
				if(picoArray.size()==16)
				{
					SI::Vfloat4x4& m = *(SI::Vfloat4x4*)buffer;
					for(uint32_t i=0; i<4; ++i)
					{
						SI::Vfloat4 v(
							(float)picoArray[i*4 + 0].get<double>(),
							(float)picoArray[i*4 + 1].get<double>(),
							(float)picoArray[i*4 + 2].get<double>(),
							(float)picoArray[i*4 + 3].get<double>());

						m.SetRow(i, v);
					}
				}
			}
			else if(inArray)
			{
				const picojson::array& picoMemberArray = picoValue.get<picojson::array>();
				DeserializeObject(outDeserializedObject, buffer, picoMemberArray, reflection);
			}
			else
			{
				const picojson::object picoObject = picoValue.get<picojson::object>();
				SI_ASSERT(!picoObject.empty());
				const picojson::array& picoMemberArray = picoObject.begin()->second.get<picojson::array>();
				DeserializeObject(outDeserializedObject, buffer, picoMemberArray, reflection);
			}
		}

		bool DeserializeObject(
			DeserializedObject& outDeserializedObject,
			void* buffer,
			const picojson::array& picoMemberArray,
			const ReflectionType& reflection)
		{
			if( reflection.GetTemplateNameHash() == SI::GetHash64S("SI::Array") )
			{
				 SI_ASSERT(strcmp(reflection.GetTemplateName(), "SI::Array") == 0 );
				 SI_ASSERT(reflection.GetMemberCount() == 2);
				 
				 const ReflectionType* argType = reflection.GetTemplateArgType();
				 uint32_t argPointerCount      = reflection.GetTemplateArgPointerCount();

				 size_t arraySize = picoMemberArray.size();
				 
				 SI::Array<uint8_t>& arrayProxy = *((SI::Array<uint8_t>*)buffer);

				// 文字列Arrayだけは特別.
				if(argPointerCount==0 && arraySize==1 && argType->GetNameHash() == GetHash64S("char"))
				{
					SI_ASSERT(strcmp(argType->GetName(), "char") == 0);
									
					std::string str = picoMemberArray[0].get<std::string>();
					size_t strLength = str.size();
					
					void* charBuffer = SI_ALIGNED_MALLOC(strLength, 1);
					outDeserializedObject.AddAllocatedBuffer(charBuffer, nullptr, 0); // char arrayだけどいいだろう.

					memcpy(charBuffer, str.c_str(), strLength);
					arrayProxy.Setup((uint8_t*)charBuffer, (uint32_t)strLength);

					return true;
				}
				 
				 void* pointerBuffer = SI_ALIGNED_MALLOC(argType->GetSize()*arraySize, argType->GetAlignment());
				 outDeserializedObject.AddAllocatedBuffer(pointerBuffer, argType, (uint32_t)arraySize);

				for(size_t i=0; i<arraySize; ++i)
				{
					void* arrayItemBuffer = ((uint8_t*)pointerBuffer) + i * argType->GetSize();
					argType->Constructor(arrayItemBuffer);
					 
					DeserializeType(
						outDeserializedObject,
						arrayItemBuffer,
						picoMemberArray[i],
						*argType,
						argPointerCount,
						true);
				}

				 arrayProxy.Setup((uint8_t*)pointerBuffer, (uint32_t)arraySize);

				 return true;
			}
			
			auto dynamicTypeItr = m_dynamicTypeTable.find(reflection.GetName());
			if(dynamicTypeItr == m_dynamicTypeTable.end()){ SI_ASSERT(0); return false; }
			const DynamicReflectionType& dynamicType = *(dynamicTypeItr->second);
			
			size_t memberCount = SI::Min(dynamicType.GetMemberCount(), picoMemberArray.size());
			for(size_t i=0; i<memberCount; ++i)
			{
				const DynamicReflectionMember& memberInFile = dynamicType.GetMember(i);
								
				const ReflectionMember* member = reflection.FindMember(memberInFile.m_name.c_str(), memberInFile.m_nameHash);
				if(!member) continue;

				if((0<memberInFile.m_arrayCount) != (0<member->GetArrayCount())) continue; // 一方がarrayでもう一方がarrayじゃない.

				const picojson::value& picoValue = picoMemberArray[i];
				void* memberBuffer = (void*)((uint8_t*)buffer + member->GetOffset());

				uint32_t arrayCount = SI::Min(memberInFile.m_arrayCount, member->GetArrayCount());
				if(0<arrayCount)
				{
					uint32_t arrayItemSize = member->GetType().GetSize();
					const picojson::object& picoArrayObject = picoValue.get<picojson::object>();
					SI_ASSERT(picoArrayObject.begin()->first[0] == '@', "Arrayの時の先頭文字は@のはず.");
					SI_ASSERT(strcmp(&(picoArrayObject.begin()->first[1]), member->GetType().GetName()) == 0, "型名一緒のはず.");
					const picojson::array& picoArray = picoArrayObject.begin()->second.get<picojson::array>();
					for(uint32_t a=0; a<arrayCount; ++a)
					{
						void* arrayItemBuffer = (void*)((uint8_t*)memberBuffer + a*arrayItemSize);
						DeserializeType(
							outDeserializedObject,
							arrayItemBuffer,
							picoArray[a],
							member->GetType(),
							member->GetPointerCount(),
							true);
					}
				}
				else
				{
					DeserializeType(
						outDeserializedObject,
						memberBuffer,
						picoValue,
						member->GetType(),
						member->GetPointerCount(),
						false);
				}
			}

			return true;
		}

	private:
		std::map<std::string, const ReflectionType*>  m_typeTable;
		std::map<std::string, DynamicReflectionType*> m_dynamicTypeTable;
		std::vector<DynamicReflectionType>            m_dynamicTypes; 
	};

	////////////////////////////////////////////////////////////////////////////////

	Deserializer::Deserializer()
		: m_impl(nullptr)
	{
	}

	Deserializer::~Deserializer()
	{
		Terminate();
	}
		
	void Deserializer::Initialize()
	{
		SI_ASSERT(!m_impl);
		m_impl = SI_NEW(DeserializerImpl);
	}

	void Deserializer::Terminate()
	{
		if(!m_impl) return;

		SI_DELETE(m_impl);
		m_impl = nullptr;
	}
	
	bool Deserializer::DeserializeRoot(
		DeserializedObject& outDeserializedObject,
		const char* path,
		const ReflectionType& reflection)
	{
		return m_impl->DeserializeRoot(outDeserializedObject, path, reflection);
	}
}
