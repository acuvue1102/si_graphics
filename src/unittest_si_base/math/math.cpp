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


TEST(Math, Vfloat3_HorizontalMinMax)
{
	SI::Vfloat3 aaa3(0.3f, 0.5f, 0.4f);
	SI::Vfloat3 bbb3(1.0f, -3.0f, 0.7f);
	
	EXPECT_EQ(SI::Math::HorizontalMin(aaa3).AsFloat(), 0.3f);
	EXPECT_EQ(SI::Math::HorizontalMin(bbb3).AsFloat(), -3.0f);
	EXPECT_EQ(SI::Math::HorizontalMax(aaa3).AsFloat(), 0.5f);
	EXPECT_EQ(SI::Math::HorizontalMax(bbb3).AsFloat(), 1.0f);
	
	SI::Vfloat4 aaa4(1.3f, 0.54f, 12.14f, 0.33f);
	SI::Vfloat4 bbb4(0.01f, -1.0f, 0.73f, -2.03f);
	
	EXPECT_EQ(SI::Math::HorizontalMin(aaa4).AsFloat(), 0.33f);
	EXPECT_EQ(SI::Math::HorizontalMin(bbb4).AsFloat(), -2.03f);
	EXPECT_EQ(SI::Math::HorizontalMax(aaa4).AsFloat(), 12.14f);
	EXPECT_EQ(SI::Math::HorizontalMax(bbb4).AsFloat(), 0.73f);
}
