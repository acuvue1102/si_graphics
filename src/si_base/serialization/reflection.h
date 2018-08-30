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
#include "si_base/core/basic_macro.h"
#include "si_base/misc/hash.h"

namespace SI
{
	class ReflectionMember;
	
	// 基本型の名前の解決を行う.
	template<typename T> struct NameIdentifer{};

	#define SI_NAME_IDENTIFER(type)\
		template<> struct SI::NameIdentifer<type>{ static const char* TYPE_NAME; };\
		const char* SI::NameIdentifer<type>::TYPE_NAME = #type;\
		template<> struct SI::NameIdentifer<type*>{ static const char* TYPE_NAME; };\
		const char* SI::NameIdentifer<type*>::TYPE_NAME = #type;
	
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

	template<typename T>
	const char* IdentifyTypeName(typename T::SfinaeType)
	{
		// Reflectionマクロが使われた型の場合.
		return T::GetTypeName();
	}

	template<typename T>
	const char* IdentifyTypeName(T)
	{
		// Reflectionマクロがない一般的な型の場合
		return SI::NameIdentifer<T>::TYPE_NAME;
	}

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
			, m_size(size)
		{
		}

		const char* GetName() const
		{
			return m_typeName;
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

	protected:
		const char*           m_typeName;
		uint32_t              m_size;
	};
	
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


	// struct/classのメンバー変数情報
	class ReflectionMember
	{
		ReflectionMember() = delete;

	public:
		ReflectionMember(const char* name, uint32_t offset, bool isPointer, const ReflectionType& type)
			: m_name(name)
			, m_offset(offset)
			, m_isPointer(isPointer)
			, m_type(type)
		{
		}

		const char* GetName() const
		{
			return m_name;
		}

		uint32_t GetOffset() const
		{
			return m_offset;
		}

		const ReflectionType& GetType() const
		{
			return m_type;
		}

		bool IsPointer() const{ return m_isPointer; }

	private:
		const char*            m_name;
		uint32_t               m_offset;
		bool                   m_isPointer;
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
			return &m_members[i];
		}

		std::array<const ReflectionMember, MEMBER_COUNT> m_members;
	};	
	
	// ユーザ定義の型だが、型内部にReflectionがない場合の対応.
	template<typename T>
	struct ReflectionExternal{};

	
	template<typename T>
	const SI::ReflectionType& GetReflectionType(typename T::SfinaeType)
	{
		// Reflectionマクロが使われた型の場合.
		return T::GetReflectionType();
	}
	
	template<typename T>
	const SI::ReflectionType& GetReflectionType(typename std::enable_if< std::is_pointer<T>::value, typename std::remove_pointer<T>::type::SfinaeType >::type)
	{
		// Reflectionマクロを持つ型のポインタが使われた型の場合.
		return std::remove_pointer<T>::type::GetReflectionType();
	}
		
	template<typename T>
	const SI::ReflectionType& GetReflectionType(typename ReflectionExternal<T>::SfinaeType)
	{
		// External Reflectionマクロが使われた型の場合.
		return ReflectionExternal<T>::GetReflectionType();
	}
		
	template<typename T>
	const SI::ReflectionType& GetReflectionType(typename std::enable_if< std::is_pointer<T>::value, typename ReflectionExternal<typename std::remove_pointer<T>::type>::SfinaeType >::type)
	{
		// External Reflectionマクロを持つ型のポインタが使われた型の場合.
		return ReflectionExternal<std::remove_pointer<T>::type>::GetReflectionType();
	}

	template<typename T>
	const SI::ReflectionType& GetReflectionType(T)
	{
		// Reflectionマクロがない一般的な型の場合
		return SI::ReflectionGenericType<T>::s_reflection;
	}

} // namespace SI

// class/structのメンバー変数を定義するためのマクロ.
#define SI_REFLECTION_MEMBER(name)\
	SI::ReflectionMember(\
		#name,\
		(uint32_t)(uintptr_t)(&(((const TargetType*)0)->name)),\
		 std::is_pointer<decltype(((const TargetType*)0)->name)>::value, SI::GetReflectionType<decltype(((const TargetType*)0)->name)>(0))

// class/structを定義するためのマクロ.
#define SI_REFLECTION(type, ...)\
	public:\
	using SfinaeType = int;\
	using TargetType = type;\
	static const char* GetTypeName()\
	{\
		return #type;\
	}\
	static const SI::ReflectionType& GetReflectionType()\
	{\
		using UserType = SI::ReflectionUserType< SI_ARGS_COUNT(__VA_ARGS__) >;\
		static const UserType s_reflection =\
			UserType(\
				#type,\
				sizeof(type),\
				{\
					__VA_ARGS__\
				} );\
		return s_reflection;\
	}


#define SI_REFLECTION_EXTERNAL_MEMBER(type, name)\
	SI::ReflectionMember(\
		#name,\
		(uint32_t)(uintptr_t)(&(((const type*)0)->name)),\
		 std::is_pointer<decltype(name)>::value, SI::GetReflectionType<decltype(name)>(0))


#define SI_REFLECTION_EXTERNAL(type, ...)\
	namespace SI\
	{\
		template<> struct SI::ReflectionExternal<type>\
		{\
			using SfinaeType = int;\
			using TargetType = type;\
			static const char* GetTypeName()\
			{\
				return #type;\
			}\
			static const SI::ReflectionType& GetReflectionType()\
			{\
				using UserType = SI::ReflectionUserType< SI_ARGS_COUNT(__VA_ARGS__) >;\
				static const UserType s_reflection =\
					UserType(\
						#type,\
						sizeof(type),\
						{\
							__VA_ARGS__\
						} );\
				return s_reflection;\
			}\
		};\
	}
