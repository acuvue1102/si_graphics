/*

型リフレクションの仕組み.

----------------
: 使用例

namespace AAA
{
	struct Test
	{
		int    intHoge;
		int*   intPtrHoge;
		double doubleHoge;
		
		SI_REFLECTION(
			AAA::Test,
			SI_REFLECTION_MEMBER(intHoge),
			SI_REFLECTION_MEMBER(intPtrHoge),
			SI_REFLECTION_MEMBER(doubleHoge))
	};
	
	struct Test2
	{
		Test  testHoge;
		Test*  testPtrHoge;
		float floatHoge;
				
		SI_REFLECTION(
			AAA::Test2,
			SI_REFLECTION_MEMBER(testHoge),
			SI_REFLECTION_MEMBER(testPtrHoge),
			SI_REFLECTION_MEMBER(floatHoge))
	};
}

// 内部で定義せず、外部で定義もできる.グローバル名前空間で宣言すること.
//SI_REFLECTION_EXTERNAL(
//	AAA::Test,
//	SI_REFLECTION_MEMBER(intHoge),
//	SI_REFLECTION_MEMBER(intPtrHoge),
//	SI_REFLECTION_MEMBER(doubleHoge))
//	
//SI_REFLECTION_EXTERNAL(
//	AAA::Test2,
//	SI_REFLECTION_MEMBER(testHoge),
//	SI_REFLECTION_MEMBER(testPtrHoge),
//	SI_REFLECTION_MEMBER(floatHoge))

// GetReflectionType関数で型の情報を取得する.
const SI::ReflectionType& type = AAA::Test2::GetReflectionType();
//const SI::ReflectionType& type = SI::ReflectionExternal<AAA::Test2>::GetReflectionType(); // external用
const char* typeName = type.GetName();
SI_PRINT("%s\n", typeName);
		
uint32_t memberCount = type.GetMemberCount();
for(uint32_t i=0; i<memberCount; ++i)
{
	const SI::ReflectionMember* member = type.GetMember(i);

	const char* memberName = member->GetName();
	uint32_t offset = member->GetOffset();
	const SI::ReflectionType& memberType = member->GetType();
	const char* memberTypeName = memberType.GetName();
	uint32_t typeSize = memberType.GetSize();
	SI_PRINT("\t%s%s, %s, offset=%d, size=%d\n", memberTypeName, member->IsPointer()? "*" : "", memberName, offset, typeSize);
			
	// メンバーの型の中のメンバーの型も調べられる.
	uint32_t memberCount2 = memberType.GetMemberCount();
	for(uint32_t j=0; j<memberCount2; ++j)
	{
		const SI::ReflectionMember* member2 = memberType.GetMember(j);

		const char* memberName2 = member2->GetName();
		uint32_t offset2 = member2->GetOffset();
		const SI::ReflectionType& memberType2 = member2->GetType();
		const char* memberTypeName2 = memberType2.GetName();
	uint32_t typeSize2 = memberType2.GetSize();
		SI_PRINT("\t\t%s%s, %s, offset=%d, size=%d\n", memberTypeName2, member2->IsPointer()? "*" : "", memberName2, offset2, typeSize2);
	}
}

---------
: 結果
AAA::Test2
	AAA::Test, testHoge, offset=0, size=24
		int32_t, intHoge, offset=0, size=4
		int32_t*, intPtrHoge, offset=8, size=8
		double, doubleHoge, offset=16, size=8
	AAA::Test*, testPtrHoge, offset=24, size=24
		int32_t, intHoge, offset=0, size=4
		int32_t*, intPtrHoge, offset=8, size=8
		double, doubleHoge, offset=16, size=8
	float, floatHoge, offset=32, size=4
*/

#pragma once

#include <cstdint>
#include <array>
#include <type_traits>

#include "si_base/core/basic_function.h"
#include "si_base/core/basic_macro.h"
#include "si_base/misc/hash.h"
#include "si_base/math/math.h"

namespace SI
{
	class ReflectionMember;

	////////////////////////////////////////////////////////////////

	// リフレクションの基本クラス.
	// このクラスを元に型情報にアクセスする.
	class ReflectionType
	{
		ReflectionType() = delete;

	public:
		ReflectionType(
			const char* typeName,
			uint32_t size )
			: m_typeName(typeName)
			, m_typeNameHash(GetHash64(typeName))
			, m_size(size)
		{
		}

		const char* GetName() const
		{
			return m_typeName;
		}

		Hash64 GetNameHash() const
		{
			return m_typeNameHash;
		}

		virtual uint32_t GetMemberCount() const
		{
			return 0;
		}

		virtual const ReflectionMember* GetMember(uint32_t i) const
		{
			return nullptr;
		}

		uint32_t GetSize() const
		{
			return m_size;
		}

		virtual const char* GetTemplateName() const
		{
			return nullptr;
		}

		virtual Hash64 GetTemplateNameHash() const
		{
			return 0;
		}

		virtual const ReflectionType* GetTemplateArgType() const
		{
			return nullptr;
		}

	protected:
		const char*           m_typeName;
		Hash64                m_typeNameHash;
		uint32_t              m_size;
	};
	
	// ユーザ定義の型だが、型内部にReflectionがない場合の対応.
	template<typename T>
	struct ReflectionExternal{};

	////////////////////////////////////////////////////////////////
	
	// 基本型の名前の解決を行う.
	template<typename T> struct NameIdentifer{ /*static const char* GetTypeName(){ return "unknown"; }*/ };

	#define SI_NAME_IDENTIFER(type)\
		template<> struct SI::NameIdentifer<type>{ static const char* GetTypeName(){ return #type; } };\
		template<> struct SI::NameIdentifer<type*>{ static const char* GetTypeName(){ return #type; } };
	
	SI_NAME_IDENTIFER(char)
	SI_NAME_IDENTIFER(int8_t)
	SI_NAME_IDENTIFER(int16_t)
	SI_NAME_IDENTIFER(int32_t)
	SI_NAME_IDENTIFER(int64_t)
	SI_NAME_IDENTIFER(uint8_t)
	SI_NAME_IDENTIFER(uint16_t)
	SI_NAME_IDENTIFER(uint32_t)
	SI_NAME_IDENTIFER(uint64_t)
	SI_NAME_IDENTIFER(float)
	SI_NAME_IDENTIFER(double)
	SI_NAME_IDENTIFER(bool)
	SI_NAME_IDENTIFER(SI::Vfloat)
	SI_NAME_IDENTIFER(SI::Vquat)
	SI_NAME_IDENTIFER(SI::Vfloat3)
	SI_NAME_IDENTIFER(SI::Vfloat4)
	SI_NAME_IDENTIFER(SI::Vfloat4x3)
	SI_NAME_IDENTIFER(SI::Vfloat4x4)

	template<typename T>
	const char* IdentifyTypeName(typename T::SfinaeType)
	{
		// Reflectionマクロが使われた型の場合.
		return T::GetTypeName();
	}

	template<typename T>
	const char* IdentifyTypeName(typename SI::ReflectionExternal<T>::SfinaeType)
	{
		// External Reflectionマクロが使われた型の場合.
		return SI::ReflectionExternal<T>::GetTypeName();
	}
	
	template<typename T>
	const char* IdentifyTypeName(typename std::enable_if< std::is_pointer<T>::value, typename int>::type input)
	{
		// ポインタの場合、ポインタを外した型で再評価.
		return IdentifyTypeName<std::remove_pointer<T>::type>(input);
	}

	template<typename T>
	const char* IdentifyTypeName(...)
	{
		// Reflectionマクロがない一般的な型の場合
		return SI::NameIdentifer<T>::GetTypeName();
	}
	
	// intなどの基本型用のリフレクション
	template<typename T>
	class ReflectionGenericType : public ReflectionType
	{
		ReflectionGenericType() = delete;
	public:
		ReflectionGenericType(const char* typeName)
			: ReflectionType(typeName, (uint32_t)sizeof(T))
		{
		}

		static const ReflectionGenericType<T> s_reflection;
	};

	template<typename T>
	const ReflectionGenericType<T> ReflectionGenericType<T>::s_reflection( IdentifyTypeName<T>(0) );

	//////////////////////////////////////////////////////////////////////////
	
	template<typename T>
	uint32_t GetPointerCountProxy(uint32_t input)
	{
		// GetPointerCountを再帰で呼び出したいが、何故かコンパイルエラーになるのでこのプロキシクラス経由で呼び出す.
		return GetPointerCount<T>(input);
	}

	template<typename T>
	uint32_t GetPointerCount(typename std::enable_if<std::is_pointer<T>::value, uint32_t>::type input)
	{
		// ポインタの場合、ポインタを外した型で再評価.
		return GetPointerCountProxy<typename std::remove_pointer<T>::type>(input+1u);
	}

	template<typename T>
	uint32_t GetPointerCount(typename std::enable_if<!std::is_pointer<T>::value, uint32_t>::type input)
	{
		// ポインタではない場合
		return input;
	}

	//////////////////////////////////////////////////////////////////////////


	// struct/classのメンバー変数情報
	class ReflectionMember
	{
		ReflectionMember() = delete;

	public:
		ReflectionMember(const char* name, uint32_t offset, uint32_t pointerCount, uint32_t arrayCount, const ReflectionType& type)
			: m_name(name)
			, m_nameHash(GetHash64(name))
			, m_offset(offset)
			, m_pointerCount(pointerCount)
			, m_arrayCount(arrayCount)
			, m_type(type)
		{
		}

		const char* GetName() const
		{
			return m_name;
		}

		Hash64 GetNameHash() const
		{
			return m_nameHash;
		}

		uint32_t GetOffset() const
		{
			return m_offset;
		}

		const ReflectionType& GetType() const
		{
			return m_type;
		}

		bool IsPointer() const{ return m_pointerCount!=0; }
		uint32_t GetPointerCount() const{ return m_pointerCount; }

		bool IsArray() const{ return m_arrayCount!=0; }
		uint32_t GetArrayCount() const{ return m_arrayCount; }

	protected:
		const char*            m_name;
		Hash64                 m_nameHash;
		uint32_t               m_offset;
		uint32_t               m_pointerCount;
		uint32_t               m_arrayCount;
		const ReflectionType&  m_type;
	};
	
	
	// ユーザー定義の型リフレクションのためのクラス.
	template<size_t MEMBER_COUNT>
	class ReflectionUserType : public ReflectionType
	{
		ReflectionUserType() = delete;

	public:
		ReflectionUserType(
			const char* typeName,
			uint32_t size,
			std::array<const ReflectionMember, MEMBER_COUNT> members)
			: ReflectionType(typeName, size)
			, m_members(members)
		{
		}

		virtual uint32_t GetMemberCount() const override
		{
			return MEMBER_COUNT;
		}

		virtual const ReflectionMember* GetMember(uint32_t i) const override
		{
			SI_ASSERT(i<MEMBER_COUNT);
			return &m_members[i];
		}

	protected:
		std::array<const ReflectionMember, MEMBER_COUNT> m_members;
	};
	
	// ユーザー定義のtemplate型リフレクションのためのクラス.
	template<size_t MEMBER_COUNT>
	class ReflectionTemplate1UserType : public ReflectionUserType<MEMBER_COUNT>
	{
		ReflectionTemplate1UserType() = delete;

	public:
		ReflectionTemplate1UserType(
			const char* typeName,
			uint32_t size,
			const char* templateTypeName,
			const ReflectionType&  templateArgType,
			std::array<const ReflectionMember, MEMBER_COUNT> members)
			: ReflectionUserType<MEMBER_COUNT>(typeName, size, members)
			, m_templateTypeName(templateTypeName)
			, m_templateTypeNameHash(GetHash64(templateTypeName))
			, m_templateArgType(templateArgType)
		{
		}

		virtual const char* GetTemplateName() const override
		{
			return m_templateTypeName;
		}
		
		virtual Hash64 GetTemplateNameHash() const
		{
			return m_templateTypeNameHash;
		}

		virtual const ReflectionType* GetTemplateArgType() const override
		{
			return &m_templateArgType;
		}

	protected:
		const char*            m_templateTypeName;
		Hash64                 m_templateTypeNameHash;
		const ReflectionType&  m_templateArgType;
	};
	
	template<typename T>
	const SI::ReflectionType& GetReflectionTypeProxy(int input)
	{
		// 再帰で呼び出したいが、何故かエラーになるのでプロキシクラス経由で呼び出す.
		return GetReflectionType<T>(input);
	}
	
	template<typename T>
	const SI::ReflectionType& GetReflectionType(typename T::SfinaeType)
	{
		// Reflectionマクロが使われた型の場合.
		return T::GetReflectionType();
	}

	template<typename T>
	const SI::ReflectionType& GetReflectionType(typename ReflectionExternal<T>::SfinaeType)
	{
		// External Reflectionマクロが使われた型の場合.
		return ReflectionExternal<T>::GetReflectionType();
	}

	template<typename T>
	const SI::ReflectionType& GetReflectionType(typename std::enable_if< std::is_pointer<T>::value, typename int>::type input)
	{
		// ポインタの場合、ポインタを外した型で再評価.
		return GetReflectionTypeProxy<std::remove_pointer<T>::type>(input);
	}

	template<typename T>
	const SI::ReflectionType& GetReflectionType(...)
	{
		// Reflectionマクロがない一般的な型の場合
		return SI::ReflectionGenericType<T>::s_reflection;
	}

	struct TemplateName
	{
		TemplateName(const char* templateName, const char* argName, uint32_t argPointerCount)
		{
			sprintf_s(m_name, "%s<%s%s%s%s>",
				templateName,
				argName,
				(0<argPointerCount)? "*" : "",
				(1<argPointerCount)? "*" : "",
				(2<argPointerCount)? "*" : "");
			SI_ASSERT(argPointerCount<=3);
		}
		char m_name[256];
	};
	
	inline int CheckSameSizeTypeAndReturn0(bool b)
	{
		SI_ASSERT(b, "SI_REFLECTION_MEMBER_AS_TYPEで宣言したメンバの型のサイズが違う");
		return 0;
	}

} // namespace SI

// class/structのメンバー変数を定義するためのマクロ.
#define SI_REFLECTION_MEMBER(name)\
	SI::ReflectionMember(\
		#name,\
		(uint32_t)(uintptr_t)(&(((const TargetType*)0)->name)),\
		SI::GetPointerCount<decltype(((const TargetType*)0)->name)>(0u),\
		0u,\
		SI::GetReflectionType<decltype(((const TargetType*)0)->name)>(0))

// class/structの配列のメンバー変数を定義するためのマクロ.
#define SI_REFLECTION_MEMBER_ARRAY(name)\
	SI::ReflectionMember(\
		#name,\
		(uint32_t)(uintptr_t)(&(((const TargetType*)0)->name[0])),\
		SI::GetPointerCount< std::decay<decltype(((const TargetType*)0)->name[0])>::type >(0u),\
		(uint32_t)SI::ArraySize(((const TargetType*)0)->name),\
		SI::GetReflectionType< std::decay<decltype(((const TargetType*)0)->name[0])>::type >(0))

// class/structのメンバー変数を定義するためのマクロ. 明示的にタイプを指定する版(念のため型のサイズは同じかチェックしておく).
#define SI_REFLECTION_MEMBER_AS_TYPE(name, type)\
	SI::ReflectionMember(\
		#name,\
		(uint32_t)(uintptr_t)(&(((const TargetType*)0)->name)),\
		SI::GetPointerCount<type>(0u),\
		0u,\
		SI::GetReflectionType<type>(SI::CheckSameSizeTypeAndReturn0(sizeof(decltype(((const TargetType*)0)->name)) == sizeof(type))))

// class/structを定義するためのマクロ.
#define SI_REFLECTION(type, ...)\
	public:\
	using SfinaeType = typename int;\
	using TargetType = typename type;\
	static const char* GetTypeName()\
	{\
		return #type;\
	}\
	static const SI::ReflectionType& GetReflectionType()\
	{\
		using UserType = typename SI::ReflectionUserType< SI_ARGS_COUNT(__VA_ARGS__) >;\
		static const UserType s_reflection =\
			UserType(\
				GetTypeName(),\
				sizeof(type),\
				{\
					__VA_ARGS__\
				} );\
		return s_reflection;\
	}

// template class/structを定義するためのマクロ. template引数1つ版.
#define SI_TEMPLATE1_REFLECTION(templateType, templateArgType, ...)\
	public:\
	using SfinaeType = typename int;\
	using TargetType = typename templateType<templateArgType>;\
	static const char* GetTypeName()\
	{\
		/*templateなので何の型かマクロでは取れない. 型情報から型名を取る必要がある*/\
		static const SI::TemplateName s_templateName(\
			#templateType,\
			SI::IdentifyTypeName<templateArgType>(0),\
			SI::GetPointerCount<templateArgType>(0u));\
		return s_templateName.m_name;\
	}\
	static const SI::ReflectionType& GetReflectionType()\
	{\
		using UserType = typename SI::ReflectionTemplate1UserType< SI_ARGS_COUNT(__VA_ARGS__) >;\
		static const UserType s_reflection =\
			UserType(\
				GetTypeName(),\
				sizeof(templateType<templateArgType>),\
				#templateType,\
				SI::GetReflectionType<templateArgType>(0),\
				{\
					__VA_ARGS__\
				} );\
		return s_reflection;\
	}

// クラスの外部でReflection定義をするためのマクロ
#define SI_REFLECTION_EXTERNAL(type, ...)\
	namespace SI\
	{\
		template<> struct SI::ReflectionExternal<type>\
		{\
			using SfinaeType = typename int;\
			using TargetType = typename type;\
			static const char* GetTypeName()\
			{\
				return #type;\
			}\
			static const SI::ReflectionType& GetReflectionType()\
			{\
				using UserType = typename SI::ReflectionUserType< SI_ARGS_COUNT(__VA_ARGS__) >;\
				static const UserType s_reflection =\
					UserType(\
						GetTypeName(),\
						sizeof(type),\
						{\
							__VA_ARGS__\
						} );\
				return s_reflection;\
			}\
		};\
	}

// Externalでもメンバがprivateだとアクセスできないので、
// 必要なstructをfriendにしてアクセスできるようにするためのマクロ
#define SI_REFLECTION_EXTERNAL_FRIEND(type)\
	friend struct SI::ReflectionExternal<type>
