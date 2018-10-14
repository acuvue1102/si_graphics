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
#endif

namespace SerializationTest
{
	struct Test0
	{
		static int s_count;

		int    intHoge;
		double doubleHoge;

		Test0()
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

		Test4()
			: strHoge(nullptr)
		{
		}
		
		bool operator==(const Test4& b) const
		{
			if(strcmp(strHoge, b.strHoge) != 0) return false;
			for(int i=0; i<4; ++i)
			{
				if(intArrayHoge[i] != b.intArrayHoge[i]) return false;
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
			SI_REFLECTION_MEMBER_ARRAY(intArrayHoge))
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
	serializer.Serialize("test0.json", src);
	serializer.Terminate();

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test0>(obj, "test0.json");
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
	serializer.Serialize("test1.json", src);
	serializer.Terminate();

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test1>(obj, "test1.json");
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
	serializer.Serialize("test2.json", src);
	serializer.Terminate();

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test2>(obj, "test2.json");
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
	serializer.Serialize("test3.json", src);
	serializer.Terminate();
	
	// 現時点でのconstructorとdestructorの数を調べる.
	int countTmp = SerializationTest::Test0::s_count;

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test3>(obj, "test3.json");
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
		
	SI::Serializer serializer;
	serializer.Initialize();
	serializer.Serialize("test4.json", src);
	serializer.Terminate();

	SI::Deserializer deserializer;
	deserializer.Initialize();

	SI::DeserializedObject obj;
	bool ret = deserializer.Deserialize<SerializationTest::Test4>(obj, "test4.json");
	EXPECT_EQ(ret, true);

	const SerializationTest::Test4& dst = *obj.Get<SerializationTest::Test4>();
	deserializer.Terminate();

	EXPECT_EQ(src, dst);
}
#endif
