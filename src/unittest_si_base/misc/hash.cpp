#include "pch.h"

#include <si_base/misc/hash.h>

using namespace SI;

TEST(Hash, Test64)
{
	constexpr Hash64 hashS = GetHash64S("12wedfghjuertyuikjhgfde45");
	Hash64 hash = GetHash64("12wedfghjuertyuikjhgfde45");
	EXPECT_EQ(hashS, hash);

	const char* hoge = "12wedfghjuertyuikjhgfde45";
	Hash64 hash2 = GetHash64(hoge);
	EXPECT_EQ(hashS, hash2);

	const char* hoge2 = " 12wedfghjuertyuikjhgfde45";
	++hoge2;
	Hash64 hash3 = GetHash64(hoge2);
	EXPECT_EQ(hashS, hash3);

	Hash64Generator generator;
	const char* hoge3 = " 12wedfghjuert";
	++hoge3;
	generator.Add(hoge3);
	generator.Add("yuikj");
	generator.Add("hgfde45");
	Hash64 hash4 = generator.Generate();
	EXPECT_EQ(hashS, hash4);

	struct HogeStruct
	{
		char hoge[25];
		HogeStruct()
		{
			memcpy(hoge, "12wedfghjuertyuikjhgfde45", sizeof(hoge));
		}
	}  hoge5;

	Hash64 hashT = GetHash64(hoge5);
	EXPECT_EQ(hashS, hashT);
}

TEST(Hash, Test32)
{
	constexpr Hash32 hashS = GetHash32S("12wedfghjuertyuikjhgfde45");
	Hash32 hash = GetHash32("12wedfghjuertyuikjhgfde45");
	EXPECT_EQ(hashS, hash);

	const char* hoge = "12wedfghjuertyuikjhgfde45";
	Hash32 hash2 = GetHash32(hoge);
	EXPECT_EQ(hashS, hash2);

	const char* hoge2 = " 12wedfghjuertyuikjhgfde45";
	++hoge2;
	Hash32 hash3 = GetHash32(hoge2);
	EXPECT_EQ(hashS, hash3);

	Hash32Generator generator;
	const char* hoge3 = " 12wedfghjuert";
	++hoge3;
	generator.Add(hoge3);
	generator.Add("yuikj");
	generator.Add("hgfde45");
	Hash32 hash4 = generator.Generate();
	EXPECT_EQ(hashS, hash4);
	
	struct HogeStruct
	{
		char hoge[25];
		HogeStruct()
		{
			memcpy(hoge, "12wedfghjuertyuikjhgfde45", sizeof(hoge));
		}
	}  hoge5;

	Hash32 hashT = GetHash32(hoge5);
	EXPECT_EQ(hashS, hashT);
}
