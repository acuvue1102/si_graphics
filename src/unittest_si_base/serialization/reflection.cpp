#include "pch.h"

#include <si_base/serialization/reflection.h>

namespace ReflectionTest
{
	struct Test
	{
		char     charHoge;
		int*     intPtrHoge;
		double   doubleHoge;
		int8_t   int8Hoge;
		uint8_t  uint8Hoge;
		short    shortHoge;
		int16_t  int16Hoge;
		uint16_t uint16Hoge;
		uint32_t uint32Hoge;
		int32_t  int32Hoge;
		
		SI_REFLECTION(
			ReflectionTest::Test,
			SI_REFLECTION_MEMBER(charHoge),
			SI_REFLECTION_MEMBER(intPtrHoge),
			SI_REFLECTION_MEMBER(doubleHoge),
			SI_REFLECTION_MEMBER(int8Hoge),
			SI_REFLECTION_MEMBER(uint8Hoge),
			SI_REFLECTION_MEMBER(shortHoge),
			SI_REFLECTION_MEMBER(int16Hoge),
			SI_REFLECTION_MEMBER(uint16Hoge),
			SI_REFLECTION_MEMBER(uint32Hoge),
			SI_REFLECTION_MEMBER(int32Hoge))
	};
	
	struct Test2
	{
		Test   testHoge;
		Test*  testPtrHoge;
		float  floatHoge;
				
		SI_REFLECTION(
			ReflectionTest::Test2,
			SI_REFLECTION_MEMBER(testHoge),
			SI_REFLECTION_MEMBER(testPtrHoge),
			SI_REFLECTION_MEMBER(floatHoge))
	};
	
	struct Test3
	{
		float  floatHoge;
		int    intHoge;
		Test2  test2Hoge;
		Test*  testPtrHoge;
	};
}
		
SI_REFLECTION_EXTERNAL(
	ReflectionTest::Test3,
	SI_REFLECTION_MEMBER(floatHoge),
	SI_REFLECTION_MEMBER(intHoge),
	SI_REFLECTION_MEMBER(test2Hoge),
	SI_REFLECTION_MEMBER(testPtrHoge))
				
TEST(Serialization, Reflection)
{
	const SI::ReflectionType& test3Type = SI::ReflectionExternal<ReflectionTest::Test3>::GetReflectionType();

	EXPECT_STREQ(test3Type.GetName(), "ReflectionTest::Test3");
	EXPECT_EQ(test3Type.GetMemberCount(), 4u);
	
	EXPECT_STREQ(test3Type.GetMember(0)->GetName(), "floatHoge");
	EXPECT_STREQ(test3Type.GetMember(1)->GetName(), "intHoge");
	EXPECT_STREQ(test3Type.GetMember(2)->GetName(), "test2Hoge");
	EXPECT_STREQ(test3Type.GetMember(3)->GetName(), "testPtrHoge");
	EXPECT_STREQ(test3Type.GetMember(0)->GetType().GetName(), "float");
	EXPECT_STREQ(test3Type.GetMember(1)->GetType().GetName(), "int32_t");
	EXPECT_STREQ(test3Type.GetMember(2)->GetType().GetName(), "ReflectionTest::Test2");
	EXPECT_STREQ(test3Type.GetMember(3)->GetType().GetName(), "ReflectionTest::Test");
	EXPECT_EQ(test3Type.GetMember(3)->GetPointerCount(), 1);
	EXPECT_EQ(test3Type.GetSize(), sizeof(ReflectionTest::Test3));
	EXPECT_EQ(test3Type.GetMember(0)->GetType().GetSize(), sizeof(float));
	EXPECT_EQ(test3Type.GetMember(2)->GetType().GetSize(), sizeof(ReflectionTest::Test2));
	
	const SI::ReflectionType& test2Type = test3Type.GetMember(2)->GetType();
	EXPECT_EQ(test2Type.GetSize(), sizeof(ReflectionTest::Test2));
	EXPECT_EQ(test2Type.GetMemberCount(), 3u);
	EXPECT_STREQ(test2Type.GetMember(0)->GetName(), "testHoge");
	EXPECT_STREQ(test2Type.GetMember(1)->GetName(), "testPtrHoge");
	EXPECT_STREQ(test2Type.GetMember(2)->GetName(), "floatHoge");
	EXPECT_EQ(test2Type.GetMember(0)->GetPointerCount(), 0);
	EXPECT_EQ(test2Type.GetMember(1)->GetPointerCount(), 1);
	EXPECT_EQ(test2Type.GetMember(2)->GetPointerCount(), 0);
	
	
	const SI::ReflectionType& testType = test2Type.GetMember(0)->GetType();
	EXPECT_STREQ(testType.GetName(), "ReflectionTest::Test");
	EXPECT_EQ(testType.GetSize(), sizeof(ReflectionTest::Test));
	EXPECT_EQ(testType.GetMemberCount(), 10u);
	EXPECT_STREQ(testType.GetMember(0)->GetName(), "charHoge");
	EXPECT_STREQ(testType.GetMember(1)->GetName(), "intPtrHoge");
	EXPECT_STREQ(testType.GetMember(2)->GetName(), "doubleHoge");	
	EXPECT_STREQ(testType.GetMember(3)->GetName(), "int8Hoge");
	EXPECT_STREQ(testType.GetMember(4)->GetName(), "uint8Hoge");
	EXPECT_STREQ(testType.GetMember(5)->GetName(), "shortHoge");
	EXPECT_STREQ(testType.GetMember(6)->GetName(), "int16Hoge");
	EXPECT_STREQ(testType.GetMember(7)->GetName(), "uint16Hoge");
	EXPECT_STREQ(testType.GetMember(8)->GetName(), "uint32Hoge");
	EXPECT_STREQ(testType.GetMember(9)->GetName(), "int32Hoge");
	
	EXPECT_STREQ(testType.GetMember(0)->GetType().GetName(), "char");
	EXPECT_STREQ(testType.GetMember(1)->GetType().GetName(), "int32_t");
	EXPECT_STREQ(testType.GetMember(2)->GetType().GetName(), "double");	
	EXPECT_STREQ(testType.GetMember(3)->GetType().GetName(), "int8_t");
	EXPECT_STREQ(testType.GetMember(4)->GetType().GetName(), "uint8_t");
	EXPECT_STREQ(testType.GetMember(5)->GetType().GetName(), "int16_t");
	EXPECT_STREQ(testType.GetMember(6)->GetType().GetName(), "int16_t");
	EXPECT_STREQ(testType.GetMember(7)->GetType().GetName(), "uint16_t");
	EXPECT_STREQ(testType.GetMember(8)->GetType().GetName(), "uint32_t");
	EXPECT_STREQ(testType.GetMember(9)->GetType().GetName(), "int32_t");

	EXPECT_EQ(testType.GetMember(0)->GetPointerCount(), 0);
	EXPECT_EQ(testType.GetMember(1)->GetPointerCount(), 1);
	EXPECT_EQ(testType.GetMember(2)->GetPointerCount(), 0);

	EXPECT_EQ(testType.GetMember(0)->GetType().GetSize(), sizeof(char));
	EXPECT_EQ(testType.GetMember(1)->GetType().GetSize(), sizeof(int32_t));
	EXPECT_EQ(testType.GetMember(2)->GetType().GetSize(), sizeof(double));	
	EXPECT_EQ(testType.GetMember(3)->GetType().GetSize(), sizeof(int8_t));
	EXPECT_EQ(testType.GetMember(4)->GetType().GetSize(), sizeof(uint8_t));
	EXPECT_EQ(testType.GetMember(5)->GetType().GetSize(), sizeof(int16_t));
	EXPECT_EQ(testType.GetMember(6)->GetType().GetSize(), sizeof(int16_t));
	EXPECT_EQ(testType.GetMember(7)->GetType().GetSize(), sizeof(uint16_t));
	EXPECT_EQ(testType.GetMember(8)->GetType().GetSize(), sizeof(uint32_t));
	EXPECT_EQ(testType.GetMember(9)->GetType().GetSize(), sizeof(int32_t));
}
