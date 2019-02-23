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


TEST(Math, TestHorizontalMinMaxAdd)
{
	{
		SI::Vfloat3 aaa(0.3f, 0.5f, 0.4f);
		SI::Vfloat3 bbb(1.0f, -3.0f, 0.7f);
	
		EXPECT_EQ(SI::Math::HorizontalMin(aaa).AsFloat(), 0.3f);
		EXPECT_EQ(SI::Math::HorizontalMin(bbb).AsFloat(), -3.0f);
		EXPECT_EQ(SI::Math::HorizontalMax(aaa).AsFloat(), 0.5f);
		EXPECT_EQ(SI::Math::HorizontalMax(bbb).AsFloat(), 1.0f);
		EXPECT_EQ(SI::Math::HorizontalAdd(aaa).AsFloat(), (aaa.X()+aaa.Y()+aaa.Z()).AsFloat());
		EXPECT_EQ(SI::Math::HorizontalAdd(bbb).AsFloat(), (bbb.X()+bbb.Y()+bbb.Z()).AsFloat());
		EXPECT_EQ(SI::Math::HorizontalMul(aaa).AsFloat(), (aaa.X()*aaa.Y()*aaa.Z()).AsFloat());
		EXPECT_EQ(SI::Math::HorizontalMul(bbb).AsFloat(), (bbb.X()*bbb.Y()*bbb.Z()).AsFloat());
	}

	{
		SI::Vfloat4 aaa(1.3f, 0.54f, 12.14f, 0.33f);
		SI::Vfloat4 bbb(0.01f, -1.0f, 0.73f, -2.03f);
	
		EXPECT_EQ(SI::Math::HorizontalMin(aaa).AsFloat(), 0.33f);
		EXPECT_EQ(SI::Math::HorizontalMin(bbb).AsFloat(), -2.03f);
		EXPECT_EQ(SI::Math::HorizontalMax(aaa).AsFloat(), 12.14f);
		EXPECT_EQ(SI::Math::HorizontalMax(bbb).AsFloat(), 0.73f);
		EXPECT_EQ(SI::Math::HorizontalAdd(aaa).AsFloat(), (aaa.X()+aaa.Y()+aaa.Z()+aaa.W()).AsFloat());
		EXPECT_EQ(SI::Math::HorizontalAdd(bbb).AsFloat(), (bbb.X()+bbb.Y()+bbb.Z()+bbb.W()).AsFloat());
		EXPECT_EQ(SI::Math::HorizontalMul(aaa).AsFloat(), (aaa.X()*aaa.Y()*aaa.Z()*aaa.W()).AsFloat());
		EXPECT_EQ(SI::Math::HorizontalMul(bbb).AsFloat(), (bbb.X()*bbb.Y()*bbb.Z()*bbb.W()).AsFloat());
	}
}


TEST(Math, TestFloor)
{
	{
		SI::Vfloat value(3.13f);
		SI::Vfloat floorValue = SI::Math::Floor(value);
		float floorValueX = floor(value.AsFloat());
	
		EXPECT_EQ(floorValue.AsFloat(), floorValueX);
	}

	{
		SI::Vfloat3 value(10.3f, -20.5f, 0.4f);
		SI::Vfloat3 floorValue = SI::Math::Floor(value);
		float floorValueX = floor(value.X().AsFloat());
		float floorValueY = floor(value.Y().AsFloat());
		float floorValueZ = floor(value.Z().AsFloat());
	
		EXPECT_EQ(floorValue.X().AsFloat(), floorValueX);
		EXPECT_EQ(floorValue.Y().AsFloat(), floorValueY);
		EXPECT_EQ(floorValue.Z().AsFloat(), floorValueZ);
	}
	
	{
		SI::Vfloat4 value(-30.3f, 2.1f, 0, 0.3f);
		SI::Vfloat4 floorValue = SI::Math::Floor(value);
		float floorValueX = floor(value.X().AsFloat());
		float floorValueY = floor(value.Y().AsFloat());
		float floorValueZ = floor(value.Z().AsFloat());
		float floorValueW = floor(value.Z().AsFloat());
	
		EXPECT_EQ(floorValue.X().AsFloat(), floorValueX);
		EXPECT_EQ(floorValue.Y().AsFloat(), floorValueY);
		EXPECT_EQ(floorValue.Z().AsFloat(), floorValueZ);
		EXPECT_EQ(floorValue.W().AsFloat(), floorValueW);
	}
}
