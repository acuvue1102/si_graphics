#include "pch.h"

#include <si_base/serialization/serializer.h>
#include <si_base/serialization/deserializer.h>
#include <si_base/core/scope_exit.h>
#include <si_base/container/array.h>
#include <si_base/math/math.h>

#define ENABLE_SERIALIZATION_TEST 1

#if ENABLE_SERIALIZATION_TEST
#define ENABLE_SERIALIZATION_TEST0 1
#define ENABLE_SERIALIZATION_TEST1 1
#define ENABLE_SERIALIZATION_TEST2 1
#define ENABLE_SERIALIZATION_TEST3 1
#define ENABLE_SERIALIZATION_TEST4 1
#define ENABLE_SERIALIZATION_TEST5 1
	// 変更前の状態.
	#define BEFORE_SERIALIZATION_TEST5  0

#endif

namespace SerializationTest
{
	struct Test0
	{
		static int s_count;

		int    intHoge;
		double doubleHoge;

		Test0()
			: intHoge(44)
			, doubleHoge(45.0)
		{
			++s_count;
		}

		~Test0()
		{
			--s_count;
		}

		bool operator==(const Test0& b) const
		{
			if(intHoge != b.intHoge) return false;
			if(doubleHoge != b.doubleHoge) return false;

			return true;
		}

		bool operator!=(const Test0& b) const
		{
			return !((*this) == b);
		}

		SI_REFLECTION(
			SerializationTest::Test0,
			SI_REFLECTION_MEMBER(intHoge),
			SI_REFLECTION_MEMBER(doubleHoge))
	};
	int Test0::s_count = 0;
	
	struct Test1
	{
		Test0 test0Hoge;
		short shortHoge;
		char  charHoge;
		
		bool operator==(const Test1& b) const
		{
			if(test0Hoge != b.test0Hoge) return false;
			if(shortHoge != b.shortHoge) return false;
			if(charHoge  != b.charHoge)  return false;

			return true;
		}

		bool operator!=(const Test1& b) const
		{
			return !((*this) == b);
		}
		
		SI_REFLECTION(
			SerializationTest::Test1,
			SI_REFLECTION_MEMBER(test0Hoge),
			SI_REFLECTION_MEMBER(shortHoge),
			SI_REFLECTION_MEMBER(charHoge))
	};
	
	struct Test2
	{
		short     shortHoge;
		Test0*    test0PtrHoge;
		char      charHoge;
		uint64_t* uint64PtrHoge;

		Test2()
			: shortHoge(1)
			, test0PtrHoge(nullptr)
			, charHoge(1)
			, uint64PtrHoge(nullptr)
		{
		}
		
		bool operator==(const Test2& b) const
		{
			if(shortHoge != b.shortHoge) return false;
			if(*test0PtrHoge != *(b.test0PtrHoge)) return false;
			if(charHoge  != b.charHoge)  return false;
			if(*uint64PtrHoge  != *(b.uint64PtrHoge))  return false;

			return true;
		}

		bool operator!=(const Test2& b) const
		{
			return !((*this) == b);
		}
		
		SI_REFLECTION(
			SerializationTest::Test2,
			SI_REFLECTION_MEMBER(shortHoge),
			SI_REFLECTION_MEMBER(test0PtrHoge),
			SI_REFLECTION_MEMBER(charHoge),
			SI_REFLECTION_MEMBER(uint64PtrHoge))
	};
	
	
	struct Test3
	{
		uint32_t         uint32Hoge;
		SI::Array<char>  charArrayHoge;
		SI::Array<Test0> test0ArrayHoge;

		Test3()
			: uint32Hoge(1)
		{
		}
		
		bool operator==(const Test3& b) const
		{
			if(uint32Hoge != b.uint32Hoge) return false;
			if(charArrayHoge.GetItemCount() != b.charArrayHoge.GetItemCount()) return false;
			for(uint32_t i=0; i<charArrayHoge.GetItemCount(); ++i)
			{
				if(charArrayHoge[i] != b.charArrayHoge[i]) return false;
			}

			if(test0ArrayHoge.GetItemCount() != b.test0ArrayHoge.GetItemCount()) return false;
			for(uint32_t i=0; i<test0ArrayHoge.GetItemCount(); ++i)
			{
				if(test0ArrayHoge[i] != b.test0ArrayHoge[i]) return false;
			}

			return true;
		}

		bool operator!=(const Test3& b) const
		{
			return !((*this) == b);
		}
		
		SI_REFLECTION(
			SerializationTest::Test3,
			SI_REFLECTION_MEMBER(uint32Hoge),
			SI_REFLECTION_MEMBER(charArrayHoge),
			SI_REFLECTION_MEMBER(test0ArrayHoge))
	};
	
	struct Test4
	{
		const char*      strHoge;
		int              intArrayHoge[4];
		Test0            test0Array[4];
		char             charArray[8];

		Test4()
			: strHoge(nullptr)
		{
		}
		
		bool operator==(const Test4& b) const
		{
			if(strcmp(strHoge, b.strHoge) != 0) return false;
			for(size_t i=0; i<SI::ArraySize(intArrayHoge); ++i)
			{
				if(intArrayHoge[i] != b.intArrayHoge[i]) return false;
			}
			for(size_t i=0; i<SI::ArraySize(test0Array); ++i)
			{
				if(test0Array[i] != b.test0Array[i]) return false;
			}
			for(size_t i=0; i<SI::ArraySize(charArray); ++i)
			{
				if(charArray[i] != b.charArray[i]) return false;
			}

			return true;
		}

		bool operator!=(const Test4& b) const
		{
			return !((*this) == b);
		}

		SI_REFLECTION(
			SerializationTest::Test4,
			SI_REFLECTION_MEMBER(strHoge),
			SI_REFLECTION_MEMBER_ARRAY(intArrayHoge),
			SI_REFLECTION_MEMBER_ARRAY(test0Array),
			SI_REFLECTION_MEMBER_ARRAY(charArray))
	};
	
	
	struct Test5Child
	{
		int    intHoge;
#if BEFORE_SERIALIZATION_TEST5
		Test0  test0Hoge;
#endif
		double doubleHoge;
#if !BEFORE_SERIALIZATION_TEST5
		uint64_t uint64Hoge;
#endif

		Test5Child()
		{
			intHoge = 1;
			doubleHoge = 1.1;
#if !BEFORE_SERIALIZATION_TEST5
			uint64Hoge = 2;
#endif
		}

		~Test5Child()
		{
		}

		bool operator==(const Test5Child& b) const
		{
			if(intHoge != b.intHoge) return false;
#if BEFORE_SERIALIZATION_TEST5
			if(test0Hoge != b.test0Hoge) return false;
#endif
			if(doubleHoge != b.doubleHoge) return false;
#if !BEFORE_SERIALIZATION_TEST5
			if(uint64Hoge != b.uint64Hoge) return false;
#endif

			return true;
		}

		bool operator!=(const Test5Child& b) const
		{
			return !((*this) == b);
		}

#if BEFORE_SERIALIZATION_TEST5
		SI_REFLECTION(
			SerializationTest::Test5Child,
			SI_REFLECTION_MEMBER(intHoge),
			SI_REFLECTION_MEMBER(test0Hoge),
			SI_REFLECTION_MEMBER(doubleHoge))
#else
		SI_REFLECTION(
			SerializationTest::Test5Child,
			SI_REFLECTION_MEMBER(intHoge),
			SI_REFLECTION_MEMBER(doubleHoge),
			SI_REFLECTION_MEMBER(uint64Hoge))
#endif
	};

	struct Test5
	{
#if BEFORE_SERIALIZATION_TEST5
		const char*      strHoge;
		int              intArrayHoge[4];
		Test5Child       test5ChildArray[4];
		uint64_t         uint64Hoge;
		char             charArray[8];
#else
		const char*      strHoge;
		uint16_t         uint16Hoge;
		int              intArrayHoge[8];
		Test5Child       test5ChildArray[5];
		char             charArray[2];
#endif

		Test5()
			: strHoge(nullptr)
		{
			for(size_t i=0; i<SI::ArraySize(intArrayHoge); ++i)
			{
				intArrayHoge[i] = 0;
			}
			for(size_t i=0; i<SI::ArraySize(charArray); ++i)
			{
				charArray[i] = 0;
			}
			
#if BEFORE_SERIALIZATION_TEST5
			uint64Hoge = 8;
#else
			uint16Hoge = 33;
#endif
		}
		
		bool operator==(const Test5& b) const
		{
			if(strcmp(strHoge, b.strHoge) != 0) return false;
			for(size_t i=0; i<SI::ArraySize(intArrayHoge); ++i)
			{
				if(intArrayHoge[i] != b.intArrayHoge[i]) return false;
			}
			for(size_t i=0; i<SI::ArraySize(test5ChildArray); ++i)
			{
				if(test5ChildArray[i] != b.test5ChildArray[i]) return false;
			}
			for(size_t i=0; i<SI::ArraySize(charArray); ++i)
			{
				if(charArray[i] != b.charArray[i]) return false;
			}
			
#if BEFORE_SERIALIZATION_TEST5
			if(uint64Hoge != b.uint64Hoge) return false;
#else
			if(uint16Hoge != b.uint16Hoge) return false;
#endif
			return true;
		}

		bool operator!=(const Test5& b) const
		{
			return !((*this) == b);
		}
		
#if BEFORE_SERIALIZATION_TEST5
		SI_REFLECTION(
			SerializationTest::Test5,
			SI_REFLECTION_MEMBER(strHoge),
			SI_REFLECTION_MEMBER_ARRAY(intArrayHoge),
			SI_REFLECTION_MEMBER_ARRAY(test5ChildArray),
			SI_REFLECTION_MEMBER(uint64Hoge),
			SI_REFLECTION_MEMBER_ARRAY(charArray))
#else
		SI_REFLECTION(
			SerializationTest::Test5,
			SI_REFLECTION_MEMBER(strHoge),
			SI_REFLECTION_MEMBER(uint16Hoge),
			SI_REFLECTION_MEMBER_ARRAY(intArrayHoge),
			SI_REFLECTION_MEMBER_ARRAY(test5ChildArray),
			SI_REFLECTION_MEMBER_ARRAY(charArray))
#endif
	};
}

#if ENABLE_SERIALIZATION_TEST0
TEST(Serialization, Serialization0)
{	
	SerializationTest::Test0 src;
	src.intHoge    = 3;
	src.doubleHoge = 4.0;
	
	SI::Serializer serializer;
	serializer.Initialize();
	serializer.Serialize("asset\\test0.json", src);
	serializer.Terminate();

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test0>(obj, "asset\\test0.json");
	EXPECT_EQ(ret, true);

	const SerializationTest::Test0& dst = *obj.Get<SerializationTest::Test0>();
	deserializer.Terminate();

	EXPECT_EQ(src, dst);
}
#endif

#if ENABLE_SERIALIZATION_TEST1
TEST(Serialization, Serialization1)
{	
	SerializationTest::Test1 src;
	src.test0Hoge.intHoge = 4;
	src.test0Hoge.doubleHoge = 8.0;
	src.shortHoge = 9;
	src.charHoge = 12;
	
	SI::Serializer serializer;
	serializer.Initialize();
	serializer.Serialize("asset\\test1.json", src);
	serializer.Terminate();

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test1>(obj, "asset\\test1.json");
	EXPECT_EQ(ret, true);

	const SerializationTest::Test1& dst = *obj.Get<SerializationTest::Test1>();
	deserializer.Terminate();

	EXPECT_EQ(src, dst);
}
#endif


#if ENABLE_SERIALIZATION_TEST2
TEST(Serialization, Serialization2)
{	
	SerializationTest::Test2 src;
	src.test0PtrHoge = new SerializationTest::Test0; SI_SCOPE_EXIT(delete src.test0PtrHoge;);
	src.test0PtrHoge->intHoge = 3;
	src.test0PtrHoge->doubleHoge = 11.0;
	src.shortHoge = 3;
	src.charHoge = 14;
	src.uint64PtrHoge = new uint64_t; SI_SCOPE_EXIT(delete src.uint64PtrHoge;);
	*src.uint64PtrHoge = 0xc0ffeec0ffee;
	
	SI::Serializer serializer;
	serializer.Initialize();
	serializer.Serialize("asset\\test2.json", src);
	serializer.Terminate();

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test2>(obj, "asset\\test2.json");
	EXPECT_EQ(ret, true);

	const SerializationTest::Test2& dst = *obj.Get<SerializationTest::Test2>();
	deserializer.Terminate();

	EXPECT_EQ(src, dst);
}

#endif

#if ENABLE_SERIALIZATION_TEST3
TEST(Serialization, Serialization3)
{
	SerializationTest::Test3 src;
	src.uint32Hoge = 5;
	
	src.charArrayHoge.Setup(new char[8], 8);
	SI_SCOPE_EXIT( delete[] src.charArrayHoge.GetItemsAddr(); );
	for(uint32_t i=0; i<8; ++i){ src.charArrayHoge[i] = (char)i; }
	
	src.test0ArrayHoge.Setup(new SerializationTest::Test0[4], 4);
	SI_SCOPE_EXIT( delete[] src.test0ArrayHoge.GetItemsAddr(); );
	for(uint32_t i=0; i<4; ++i)
	{
		src.test0ArrayHoge[i].intHoge = i;
		src.test0ArrayHoge[i].doubleHoge = (double)i+0.5;
	}
		
	SI::Serializer serializer;
	serializer.Initialize();
	serializer.Serialize("asset\\test3.json", src);
	serializer.Terminate();
	
	// 現時点でのconstructorとdestructorの数を調べる.
	int countTmp = SerializationTest::Test0::s_count;

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test3>(obj, "asset\\test3.json");
	EXPECT_EQ(ret, true);

	const SerializationTest::Test3& dst = *obj.Get<SerializationTest::Test3>();
	deserializer.Terminate();

	EXPECT_EQ(src, dst);
		
	EXPECT_GT(SerializationTest::Test0::s_count, countTmp);

	obj.Release();
	// deserializedObjectが無くなって、deserialize前に戻ったはず.
	EXPECT_EQ(SerializationTest::Test0::s_count, countTmp);
}
#endif

#if ENABLE_SERIALIZATION_TEST4
TEST(Serialization, Serialization4)
{
	SerializationTest::Test4 src;
	src.strHoge = "hogehoge";
	for(size_t i=0; i<SI::ArraySize(src.intArrayHoge); ++i)
	{
		src.intArrayHoge[i] = (int)(i*i);
	}
	for(size_t i=0; i<SI::ArraySize(src.test0Array); ++i)
	{
		src.test0Array[i].intHoge    = (int)(i*i*i);
		src.test0Array[i].doubleHoge = - (double)(i*i*i) * 0.1;
	}
	for(size_t i=0; i<SI::ArraySize(src.charArray); ++i)
	{
		src.charArray[i] = '0' + (char)i;
	}
		
	SI::Serializer serializer;
	serializer.Initialize();
	serializer.Serialize("asset\\test4.json", src);
	serializer.Terminate();

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test4>(obj, "asset\\test4.json");
	EXPECT_EQ(ret, true);

	const SerializationTest::Test4& dst = *obj.Get<SerializationTest::Test4>();
	deserializer.Terminate();

	EXPECT_EQ(src, dst);
}
#endif



#if ENABLE_SERIALIZATION_TEST5
// パラメータが途中で変わっても、互換性を保てるかテスト.
TEST(Serialization, Serialization5)
{
#if BEFORE_SERIALIZATION_TEST5
	SerializationTest::Test5 src;
	src.strHoge = "fugafuga";
	for(size_t i=0; i<SI::ArraySize(src.intArrayHoge); ++i)
	{
		src.intArrayHoge[i] = (int)(i*i);
	}
	for(size_t i=0; i<SI::ArraySize(src.test5ChildArray); ++i)
	{
		src.test5ChildArray[i].intHoge              = (int)(i*i*i);
		src.test5ChildArray[i].test0Hoge.intHoge    = 2*(int)(i*i*i);
		src.test5ChildArray[i].test0Hoge.doubleHoge = - (double)(i*i*i) * 0.01;
		src.test5ChildArray[i].doubleHoge           = - (double)(i*i*i) * 0.1;
	}
	for(size_t i=0; i<SI::ArraySize(src.charArray); ++i)
	{
		src.charArray[i] = '0' + (char)i;
	}
		
	SI::Serializer serializer;
	serializer.Initialize();
	serializer.Serialize("asset\\test5.json", src);
	serializer.Terminate();

#else

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test5>(obj, "asset\\test5.json");
	EXPECT_EQ(ret, true);

	const SerializationTest::Test5& dst = *obj.Get<SerializationTest::Test5>();
	deserializer.Terminate();
	
	EXPECT_EQ(dst.uint16Hoge, 33);
	for(size_t i=0; i<4; ++i)
	{
		EXPECT_EQ( dst.test5ChildArray[i].intHoge              , (int)(i*i*i));
		EXPECT_EQ( dst.test5ChildArray[i].doubleHoge           , - (double)(i*i*i) * 0.1);
		EXPECT_EQ( dst.test5ChildArray[i].uint64Hoge, 2);
	}
	EXPECT_EQ( dst.test5ChildArray[4].intHoge, 1);
	EXPECT_EQ( dst.test5ChildArray[4].doubleHoge, 1.1);
	EXPECT_EQ( dst.test5ChildArray[4].uint64Hoge, 2);
	
#endif
}
#endif

