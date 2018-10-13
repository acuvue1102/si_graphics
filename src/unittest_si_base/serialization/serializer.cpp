#include "pch.h"

#include <si_base/serialization/serializer.h>
#include <si_base/serialization/deserializer.h>

namespace SerializationTest
{
	struct Test0
	{
		int    intHoge;
		double doubleHoge;
		
		SI_REFLECTION(
			SerializationTest::Test0,
			SI_REFLECTION_MEMBER(intHoge),
			SI_REFLECTION_MEMBER(doubleHoge))
	};
}

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
	SerializationTest::Test0* dst = deserializer.Deserialize<SerializationTest::Test0>("test0.json");
	deserializer.Terminate();

	EXPECT_EQ(src.intHoge,    dst->intHoge);
	EXPECT_EQ(src.doubleHoge, dst->doubleHoge);
}