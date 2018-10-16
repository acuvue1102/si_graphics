#include "pch.h"

#include <si_base/math/math.h>

using namespace SI;

TEST(Math, Vfloat3EQ)
{
	SI::Vfloat3 aaa;
	SI::Vfloat3 bbb;
	memset(&aaa, 1, sizeof(aaa));
	memset(&bbb, 2, sizeof(bbb));
	aaa.SetX(5);aaa.SetY(6);aaa.SetZ(7);
	bbb.SetX(5);bbb.SetY(6);bbb.SetZ(7);
	EXPECT_EQ(aaa, bbb);
}
