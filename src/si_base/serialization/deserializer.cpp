
#include "si_base/serialization/Deserializer.h"

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

		picojson::value ToPicojsonValue(bool i)
		{
			 return picojson::value(i);
		}
		
		picojson::value ToPicojsonValue(SI::Vfloat i)
		{
			 return picojson::value(double(i.Get()));
		}
		
		picojson::value ToPicojsonValue(SI::Vfloat3 i)
		{
			picojson::array a;
			a.push_back(picojson::value(double(i.Xf())));
			a.push_back(picojson::value(double(i.Yf())));
			a.push_back(picojson::value(double(i.Zf())));
			return picojson::value(a);
		}

		picojson::value ToPicojsonValue(SI::Vquat i)
		{
			picojson::array a;
			a.push_back(picojson::value(double(i.Xf())));
			a.push_back(picojson::value(double(i.Yf())));
			a.push_back(picojson::value(double(i.Zf())));
			a.push_back(picojson::value(double(i.Wf())));
			return picojson::value(a);
		}

		picojson::value ToPicojsonValue(SI::Vfloat4 i)
		{
			picojson::array a;
			a.push_back(picojson::value(double(i.Xf())));
			a.push_back(picojson::value(double(i.Yf())));
			a.push_back(picojson::value(double(i.Zf())));
			a.push_back(picojson::value(double(i.Wf())));
			return picojson::value(a);
		}

		picojson::value ToPicojsonValue(SI::Vfloat3x3 i)
		{
			picojson::array a;
			for(uint32_t id=0; id<3; ++id)
			{
				Vfloat3 v = i[id];
				a.push_back(picojson::value(double(v.Xf())));
				a.push_back(picojson::value(double(v.Yf())));
				a.push_back(picojson::value(double(v.Zf())));
			}
			return picojson::value(a);
		}

		picojson::value ToPicojsonValue(SI::Vfloat4x3 i)
		{
			picojson::array a;
			for(uint32_t id=0; id<4; ++id)
			{
				Vfloat3 v = i[id];
				a.push_back(picojson::value(double(v.Xf())));
				a.push_back(picojson::value(double(v.Yf())));
				a.push_back(picojson::value(double(v.Zf())));
			}
			return picojson::value(a);
		}

		picojson::value ToPicojsonValue(SI::Vfloat4x4 i)
		{
			picojson::array a;
			for(uint32_t id=0; id<4; ++id)
			{
				Vfloat4 v = i[id];
				a.push_back(picojson::value(double(v.Xf())));
				a.push_back(picojson::value(double(v.Yf())));
				a.push_back(picojson::value(double(v.Zf())));
				a.push_back(picojson::value(double(v.Wf())));
			}
			return picojson::value(a);
		}

		std::string MakeJsonReadable(const std::string& json)
		{
			std::string o;

			std::string tmpArray;
			int tab = 0;
			size_t jsonLength = json.size();
			bool isPureArray = false;
			for(size_t stringId=0; stringId<jsonLength; ++stringId)
			{
				char c = json[stringId];

				bool newLine = false;
				if(c=='{')
				{
					++tab;
					newLine = true;
				}
				else if(c=='}')
				{
					--tab;
					o.push_back('\n');
					for(int i=0; i<tab; ++i)
					{
						o.push_back('\t');
					}
				}
				else if(c=='[')
				{					
					// 基本要素だけの単純な配列か調べる.
					// (単純な配列なら改行しない)
					isPureArray = true;
					for(size_t stringId2=stringId+1; stringId2<jsonLength; ++stringId2)
					{
						char c2 = json[stringId2];
						if(c2==']')
						{
							break;
						}
						else if(c2=='{')
						{
							isPureArray = false;
							break;
						}
						else if(c2=='[')
						{
							isPureArray = false;
							break;
						}
					}
					
					if(!isPureArray)
					{
						++tab;
						newLine = true;
					}
				}
				else if(c==']')
				{
					if(isPureArray)
					{
						isPureArray = false;
					}
					else
					{
						--tab;
						o.push_back('\n');
						for(int i=0; i<tab; ++i)
						{
							o.push_back('\t');
						}
					}
				}
				else if(c==',')
				{
					if(!isPureArray)
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
		
		void* DeserializeRoot(const char* path, const ReflectionType& reflection)
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
					return nullptr;
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
				return nullptr;
			}
			
			picojson::object& topObj = picoValue.get<picojson::object>();
			
			if(!RegisterRefelenceType(reflection)) return nullptr;

			auto typeTableItr = topObj.find("typeTable");
			if(typeTableItr == topObj.end()){ return nullptr; }
			if(!DeserializeTypeTable(typeTableItr->second.get<picojson::object>())){ return nullptr; }
			//
			auto objectItr = topObj.find("object");
			if(objectItr == topObj.end()){ return nullptr; }
			void * buffer = DeserializeObject(objectItr->second.get<picojson::object>(), reflection);

			//reflection.Destructor(buffer);
			//SI_ALIGNED_FREE(buffer);
			
			return buffer;
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
			void* buffer,
			picojson::value& picoValue,
			const ReflectionType& reflection,
			uint32_t pointerCount)
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

					memcpy(pointerBuffer, str.c_str(), strLength);
					pointerBuffer[strLength] = 0; // 終端.
				}
				else
				{
					// ポインタ外し.
					void*& pointerBuffer = *((void**)buffer);
					pointerBuffer = SI_ALIGNED_MALLOC(sizeof(void*), alignof(void*));
					return DeserializeType(
						pointerBuffer,
						picoValue,
						reflection,
						pointerCount-1);
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
							(float)picoArray[i*4 + 2].get<double>());

						m.SetRow(i, v);
					}
				}
			}
			else
			{
				picojson::object picoObject = picoValue.get<picojson::object>();

				DeserializeObject(picoObject, reflection);
			}
		}

		void* DeserializeObject(picojson::object& picoObject, const ReflectionType& reflection)
		{
			auto picoMemberArrayItr = picoObject.find(reflection.GetName());
			if(picoMemberArrayItr == picoObject.end()) return nullptr;
			picojson::array& picoMemberArray = picoMemberArrayItr->second.get<picojson::array>();

			void* buffer = SI_ALIGNED_MALLOC( reflection.GetSize(), reflection.GetAlignment() );
			reflection.Constructor(buffer);

			auto dynamicTypeItr = m_dynamicTypeTable.find(reflection.GetName());
			if(dynamicTypeItr == m_dynamicTypeTable.end()){ SI_ASSERT(0); return nullptr; }
			const DynamicReflectionType& dynamicType = *(dynamicTypeItr->second);

			size_t memberCount = SI::Min(dynamicType.GetMemberCount(), picoMemberArray.size());
			for(size_t i=0; i<memberCount; ++i)
			{
				const DynamicReflectionMember& memberInFile = dynamicType.GetMember(i);
								
				const ReflectionMember* member = reflection.FindMember(memberInFile.m_name.c_str(), memberInFile.m_nameHash);
				if(!member) continue;

				picojson::value& picoValue = picoMemberArray[i];
				

				void* memberBuffer = (void*)((uint8_t*)buffer + member->GetOffset());
				DeserializeType(memberBuffer, picoValue, member->GetType(), member->GetPointerCount());

			}

			return buffer;
		}

#if 0
		bool SerializeDataByType(
			picojson::array& picoData,
			const void* offsetedBuffer,
			const ReflectionType& reflection,
			uint32_t pointerCount)
		{
			Hash64 typeNameHash = reflection.GetNameHash();

			if(0<pointerCount)
			{
				// 文字列の時だけは特別扱い.
				if(pointerCount==1 && typeNameHash == GetHash64S("char"))
				{
					SI_ASSERT(strcmp(reflection.GetName(), "char") == 0);
					const char* str = (const char*)offsetedBuffer;
					picoData.push_back(picojson::value(str));
				}
				else
				{
					// ポインタ外し.
					const void* ptr = *(const void**)(((const uint8_t*)offsetedBuffer));
					return SerializeDataByType(
						picoData,
						ptr,
						reflection,
						pointerCount-1);
				}
			}
			else if(typeNameHash == GetHash64S("int8_t"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "int8_t") == 0);
				int8_t memberData = *(const int8_t*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("char"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "char") == 0);
				char memberData = *(const char*)offsetedBuffer;
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
			else if(typeNameHash == GetHash64S("bool"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "bool") == 0);
				bool memberData = *(const bool*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat") == 0);
				SI::Vfloat memberData = *(const SI::Vfloat*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("SI::Vquat"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vquat") == 0);
				SI::Vquat memberData = *(const SI::Vquat*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat3"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat3") == 0);
				SI::Vfloat3 memberData = *(const SI::Vfloat3*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat4"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat4") == 0);
				SI::Vfloat4 memberData = *(const SI::Vfloat4*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat4x3"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat4x3") == 0);
				SI::Vfloat4x3 memberData = *(const SI::Vfloat4x3*)offsetedBuffer;
				picoData.push_back(ToPicojsonValue(memberData));
			}
			else if(typeNameHash == GetHash64S("SI::Vfloat4x4"))
			{
				SI_ASSERT(strcmp(reflection.GetName(), "SI::Vfloat4x4") == 0);
				SI::Vfloat4x4 memberData = *(const SI::Vfloat4x4*)offsetedBuffer;
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

				// 文字列の時は特別扱い.
				if(arrayItemPointerCount==0 && arrayPointerMember->GetType().GetNameHash() == GetHash64S("char"))
				{
					SI_ASSERT(strcmp(arrayPointerMember->GetType().GetName(), "char") == 0);
					const char* str = (const char*)arrayPtr;
					picoData.push_back(picojson::value(str, (size_t)arrayCount));
					return true;
				}

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
				uint32_t arrayCount = member->GetArrayCount();
				
				if(0<arrayCount)
				{
					picojson::array picoMemberArray;
					size_t typeSize = memberReflection.GetSize();

					for(uint32_t a=0; a<arrayCount; ++a)
					{
						const void* arrayOffsetedBuffer = (const int8_t*)memberBuffer + a * typeSize;
						SerializeDataByType(picoMemberArray, arrayOffsetedBuffer, memberReflection, pointerCount);
					}
					picoData.push_back(picojson::value(picoMemberArray));
				}
				else
				{
					SerializeDataByType(picoData, memberBuffer, memberReflection, pointerCount);
				}
			}

			return true;
		}
#endif


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
	}
	
	void* Deserializer::DeserializeRoot(const char* path, const ReflectionType& reflection)
	{
		return m_impl->DeserializeRoot(path, reflection);
	}
}
