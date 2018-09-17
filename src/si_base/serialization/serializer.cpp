
#include "si_base/serialization/serializer.h"

#include <map>

#define PICOJSON_USE_INT64
#include "external/picojson/picojson.h"

#include "si_base/core/new_delete.h"
#include "si_base/core/print.h"
#include "si_base/file/file.h"

namespace SI
{
	namespace
	{
		void PicojsonInsert(picojson::object& obj, const char* key, const picojson::value& a)
		{
			obj.insert(std::make_pair(key, a));
		}
		
		picojson::value ToPicojsonValue(int32_t i)
		{
			 return picojson::value(double(i));
		}
		
		picojson::value ToPicojsonValue(uint32_t i)
		{
			 return picojson::value(double(i));
		}
		
		picojson::value ToPicojsonValue(int64_t i)
		{
			 return picojson::value(i);
		}
		
		picojson::value ToPicojsonValue(uint64_t i)
		{
			 return picojson::value((int64_t)i);
		}
		
		picojson::value ToPicojsonValue(float i)
		{
			 return picojson::value(double(i));
		}
		
		picojson::value ToPicojsonValue(double i)
		{
			 return picojson::value(i);
		}

		std::string MakeJsonReadable(const std::string& json)
		{
			std::string o;

			int tab = 0;
			int arrayNest = 0;
			for(char c : json)
			{
				bool newLine = false;
				if(c=='{')
				{
					++tab;
					newLine = true;
				}
				else if(c=='}')
				{
					--tab;
					//newLine = true;
					o.push_back('\n');
					for(int i=0; i<tab; ++i)
					{
						o.push_back('\t');
					}
				}
				else if(c=='[')
				{
					//if(arrayNest==1)
					//{
					//	o.push_back('\n');
					//	for(int i=0; i<tab; ++i)
					//	{
					//		o.push_back('\t');
					//	}
					//}

					++tab;
					++arrayNest;
				}
				else if(c==']')
				{
					--tab;
					--arrayNest;
				}
				else if(c==',')
				{
					if(arrayNest==0)
					{
						newLine = true;
					}
				}

				o.push_back(c);
				
				if(newLine)
				{
					o.push_back('\n');
					for(int i=0; i<tab; ++i)
					{
						o.push_back('\t');
					}
				}
			}
			
			o.push_back('\n');
			return o;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	class SerializerImpl
	{
	public:
		SerializerImpl()
		{
			// 基本型をあらかじめ登録しておいて、出力されないようにする.
			m_typeTable.insert( std::make_pair("int8_t",   nullptr) );
			m_typeTable.insert( std::make_pair("int16_t",  nullptr) );
			m_typeTable.insert( std::make_pair("int32_t",  nullptr) );
			m_typeTable.insert( std::make_pair("int64_t",  nullptr) );
			m_typeTable.insert( std::make_pair("uint8_t",  nullptr) );
			m_typeTable.insert( std::make_pair("uint16_t", nullptr) );
			m_typeTable.insert( std::make_pair("uint32_t", nullptr) );
			m_typeTable.insert( std::make_pair("uint64_t", nullptr) );
			m_typeTable.insert( std::make_pair("float",    nullptr) );
			m_typeTable.insert( std::make_pair("double",   nullptr) );
		}

		~SerializerImpl()
		{
		}
		
		bool SerializeRoot(const char* path, const void* buffer, const ReflectionType& reflection)
		{
			// jsonにシリアライズ.
			std::string json;
			{
				picojson::object picoRoot;
				{
					picojson::object picoTypes;
					if(!SerializeType(picoTypes, reflection)) return false;
				
					picojson::object picoObject;
					{
						picojson::array picoData;
						if(!SerializeData(picoData, buffer, reflection)) return false;

						PicojsonInsert(picoObject, "dataType", picojson::value(reflection.GetName()));
						PicojsonInsert(picoObject, "data", picojson::value(picoData));
					}
				
					PicojsonInsert(picoRoot, "typeTable", picojson::value(picoTypes));
					PicojsonInsert(picoRoot, "object", picojson::value(picoObject));
				}

				json = picojson::value(picoRoot).serialize();

				// 見にくいので整形する.
				json = MakeJsonReadable(json);
			}
			
			// ファイルに出力.
			{
				File f;
				int ret = f.Open(path, SI::FileAccessType::Write);
				if(ret!=0)
				{
					SI_WARNING("file(%s) can't be loaded.", path);
					return false;
				}


				// utf-8 with BOM
				uint8_t bom[3] = {0xEF, 0xBB, 0xBF};
				f.Write(bom, sizeof(bom));

				f.Write(json.c_str(), json.size());

				f.Close();
			}

			return true;
		}
	
		bool SerializeType(picojson::object& picoTypes, const ReflectionType& reflection)
		{
			const char* name = reflection.GetName();
			auto itr = m_typeTable.find(std::string(name));
			if(itr != m_typeTable.end()) return true; // 登録済み

			picojson::array picoType;

			uint32_t memberCount = reflection.GetMemberCount();

			for(uint32_t m=0; m<memberCount; ++m)
			{
				const ReflectionMember* member = reflection.GetMember(m);
				if(!member) continue;
				const ReflectionType& memberReflection = member->GetType();
				
				picojson::object picoMember;
				PicojsonInsert(picoMember, "name",         picojson::value(member->GetName()));
				PicojsonInsert(picoMember, "type",         picojson::value(memberReflection.GetName()));
				PicojsonInsert(picoMember, "pointerCount", ToPicojsonValue(member->GetPointerCount()));

				picoType.push_back(picojson::value(picoMember));

				SerializeType(picoTypes, memberReflection);
			}
			
			picoTypes.insert( std::make_pair(name, picoType) );
			m_typeTable.insert( std::make_pair(name, &reflection) );

			return true;
		}

		bool SerializeDataByType(
			picojson::array& picoData,
			const void* offsetedBuffer,
			const ReflectionType& reflection,
			uint32_t pointerCount)
		{
			Hash64 typeNameHash = reflection.GetNameHash();

			if(0<pointerCount)
			{
				// ポインタ外し.
				const void* ptr = *(const void**)(((const uint8_t*)offsetedBuffer));
				return SerializeDataByType(
					picoData,
					ptr,
					reflection,
					pointerCount-1);
			}
			else if(typeNameHash == GetHash64S("int8_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int8_t") == 0);
				int8_t memberData = *(const int8_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("uint8_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "uint8_t") == 0);
				uint8_t memberData = *(const uint8_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("int16_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int16_t") == 0);
				int16_t memberData = *(const int16_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("uint16_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "uint16_t") == 0);
				uint16_t memberData = *(const uint16_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("int32_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int32_t") == 0);
				int32_t memberData = *(const int32_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("uint32_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "uint32_t") == 0);
				uint32_t memberData = *(const uint32_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("int64_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int64_t") == 0);
				int64_t memberData = *(const int64_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("uint64_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "uint64_t") == 0);
				uint64_t memberData = *(const uint64_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("float"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "float") == 0);
				float memberData = *(const float*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("double"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "double") == 0);
				double memberData = *(const double*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else
			{
				picojson::array picoMemberArray;
				SerializeData(picoMemberArray, offsetedBuffer, reflection);
				picoData.push_back(picojson::value(picoMemberArray));
			}

			return true;
		}
		
		bool SerializeData(picojson::array& picoData, const void* buffer, const ReflectionType& reflection)
		{
			const char* name = reflection.GetName();
			SI_ASSERT(m_typeTable.find(std::string(name)) != m_typeTable.end(), "type登録されていない.");

			uint32_t memberCount = reflection.GetMemberCount();

			// SI::Arrayの時は特別扱い.
			const char* templateName = reflection.GetTemplateName();
			if(templateName && reflection.GetTemplateNameHash() == GetHash64S("SI::Array"))
			{
				SI_ASSERT(strcmp(templateName, "SI::Array") == 0);
				SI_ASSERT(memberCount == 2);
				
				const ReflectionMember* member0 = reflection.GetMember(0);
				const ReflectionMember* member1 = reflection.GetMember(1);
				SI_ASSERT(member0 && member1);
								
				const ReflectionMember* arrayPointerMember;
				const ReflectionMember* arrayCountMember;
				if(member0->GetNameHash()==GetHash64S("m_items") && member1->GetNameHash()==GetHash64S("m_itemCount"))
				{
					arrayPointerMember = member0;
					arrayCountMember   = member1;
				}
				else
				if(member1->GetNameHash()==GetHash64S("m_items") && member0->GetNameHash()==GetHash64S("m_itemCount"))
				{
					arrayPointerMember = member1;
					arrayCountMember   = member0;
				}
				else
				{
					SI_ASSERT(0);
					return false;
				}
				
				SI_ASSERT(arrayCountMember->GetType().GetNameHash() == GetHash64S("uint32_t"));
				uint32_t arrayCount = *(uint32_t*)(((const uint8_t*)buffer) + arrayCountMember->GetOffset());

				SI_ASSERT(arrayPointerMember->GetPointerCount()==1);
				const void* arrayPtr = *(const void**)(((const uint8_t*)buffer) + arrayPointerMember->GetOffset());

				const ReflectionType& arrayItemReflection = arrayPointerMember->GetType();
				uint32_t arrayItemOffset = arrayItemReflection.GetSize();
				uint32_t arrayItemPointerCount = arrayPointerMember->GetPointerCount() - 1; // 元の型のポインタの数.

				picojson::array picoMemberArray;
				for(uint32_t a=0; a<arrayCount; ++a)
				{
					const void* arrayItemPtr = (const void*)(((const uint8_t*)arrayPtr) + a*arrayItemOffset);
					SerializeDataByType(picoMemberArray, arrayItemPtr, arrayItemReflection, arrayItemPointerCount);
				}
				picoData.push_back(picojson::value(picoMemberArray));

				return true;
			}

			for(uint32_t m=0; m<memberCount; ++m)
			{
				const ReflectionMember* member = reflection.GetMember(m);
				if(!member) continue;
				const ReflectionType& memberReflection = member->GetType();
								
				uint32_t offset = member->GetOffset();
				const void* memberBuffer = ((const uint8_t*)buffer) + offset;

				uint32_t pointerCount = member->GetPointerCount();
			
				SerializeDataByType(picoData, memberBuffer, memberReflection, pointerCount);
			}

			return true;
		}

	private:
		std::map<std::string, const ReflectionType*> m_typeTable;
	};

	////////////////////////////////////////////////////////////////////////////////

	Serializer::Serializer()
		: m_impl(nullptr)
	{
	}

	Serializer::~Serializer()
	{
		Terminate();
	}
		
	void Serializer::Initialize()
	{
		SI_ASSERT(!m_impl);
		m_impl = SI_NEW(SerializerImpl);
	}

	void Serializer::Terminate()
	{
		if(!m_impl) return;

		SI_DELETE(m_impl);
	}
	
	void Serializer::SerializeRoot(const char* path, const void* buffer, const ReflectionType& reflection)
	{
		m_impl->SerializeRoot(path, buffer, reflection);
	}
}
