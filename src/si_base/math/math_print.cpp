#include "si_base/math/math_print.h"
#include "si_base/math/vfloat.h"
#include "si_base/math/vfloat3.h"
#include "si_base/math/vfloat4.h"
#include "si_base/math/vquat.h"
#include "si_base/math/vfloat3x3.h"
#include "si_base/math/vfloat4x3.h"
#include "si_base/math/vfloat4x4.h"

#include <stdio.h>
#include <Windows.h>

namespace SI
{
namespace Math
{
	namespace
	{
		void PrintOutputDebug( const char* format, ...)
		{
			va_list	list;
			char str[256];
			va_start(list, format);
			vsprintf_s( str, format, list);
			va_end(list);

			OutputDebugString(str);
		}
	}

	void Print(Vfloat_arg f, const char* tag)
	{
		PrintOutputDebug("%s = (%+10.3f)\n", tag, f.AsFloat());
	}

	void Print(Vfloat3_arg   v, const char* tag)
	{
		PrintOutputDebug("%s = (%+10.3f, %+10.3f, %+10.3f)\n", tag,
			v.X().AsFloat(),
			v.Y().AsFloat(),
			v.Z().AsFloat());
	}

	void Print(Vfloat4_arg   v, const char* tag)
	{
		PrintOutputDebug("%s = (%+10.3f, %+10.3f, %+10.3f, %+10.3f)\n", tag,
			v.X().AsFloat(),
			v.Y().AsFloat(),
			v.Z().AsFloat(),
			v.W().AsFloat());
	}

	void Print(Vquat_arg     q, const char* tag)
	{
		PrintOutputDebug("%s = (%+10.3f, %+10.3f, %+10.3f, %+10.3f)\n", tag,
			q.X().AsFloat(),
			q.Y().AsFloat(),
			q.Z().AsFloat(),
			q.W().AsFloat());
	}

	void Print(Vfloat3x3_arg m, const char* tag)
	{
		Vfloat3 v0 = m.GetRow(0);
		Vfloat3 v1 = m.GetRow(1);
		Vfloat3 v2 = m.GetRow(2);
		
		PrintOutputDebug("-----------------\n");
		PrintOutputDebug("%s = (%+10.3f, %+10.3f, %+10.3f)\n", tag,
			v0.X().AsFloat(), v0.Y().AsFloat(), v0.Z().AsFloat());
		
		PrintOutputDebug("%s   (%+10.3f, %+10.3f, %+10.3f)\n", tag,
			v1.X().AsFloat(), v1.Y().AsFloat(), v1.Z().AsFloat());
		
		PrintOutputDebug("%s   (%+10.3f, %+10.3f, %+10.3f)\n", tag,
			v2.X().AsFloat(), v2.Y().AsFloat(), v2.Z().AsFloat());
		PrintOutputDebug("-----------------\n");
	}

	void Print(Vfloat4x3_arg m, const char* tag)
	{
		Vfloat3 v0 = m.GetRow(0);
		Vfloat3 v1 = m.GetRow(1);
		Vfloat3 v2 = m.GetRow(2);
		Vfloat3 v3 = m.GetRow(3);
		
		PrintOutputDebug("-----------------\n");
		PrintOutputDebug("%s = (%+10.3f, %+10.3f, %+10.3f)\n", tag,
			v0.X().AsFloat(), v0.Y().AsFloat(), v0.Z().AsFloat());
		
		PrintOutputDebug("%s   (%+10.3f, %+10.3f, %+10.3f)\n", tag,
			v1.X().AsFloat(), v1.Y().AsFloat(), v1.Z().AsFloat());
		
		PrintOutputDebug("%s   (%+10.3f, %+10.3f, %+10.3f)\n", tag,
			v2.X().AsFloat(), v2.Y().AsFloat(), v2.Z().AsFloat());
		
		PrintOutputDebug("%s   (%+10.3f, %+10.3f, %+10.3f)\n", tag,
			v3.X().AsFloat(), v3.Y().AsFloat(), v3.Z().AsFloat());
		PrintOutputDebug("-----------------\n");
	}

	void Print(Vfloat4x4_arg m, const char* tag)
	{
		Vfloat4 v0 = m.GetRow(0);
		Vfloat4 v1 = m.GetRow(1);
		Vfloat4 v2 = m.GetRow(2);
		Vfloat4 v3 = m.GetRow(3);
		
		PrintOutputDebug("-----------------\n");
		PrintOutputDebug("%s = (%+10.3f, %+10.3f, %+10.3f, %+10.3f)\n", tag,
			v0.X().AsFloat(), v0.Y().AsFloat(), v0.Z().AsFloat(), v0.W().AsFloat());
		
		PrintOutputDebug("%s   (%+10.3f, %+10.3f, %+10.3f, %+10.3f)\n", tag,
			v1.X().AsFloat(), v1.Y().AsFloat(), v1.Z().AsFloat(), v1.W().AsFloat());
		
		PrintOutputDebug("%s   (%+10.3f, %+10.3f, %+10.3f, %+10.3f)\n", tag,
			v2.X().AsFloat(), v2.Y().AsFloat(), v2.Z().AsFloat(), v2.W().AsFloat());
		
		PrintOutputDebug("%s   (%+10.3f, %+10.3f, %+10.3f, %+10.3f)\n", tag,
			v3.X().AsFloat(), v3.Y().AsFloat(), v3.Z().AsFloat(), v3.W().AsFloat());
		PrintOutputDebug("-----------------\n");
	}

} // namespace Math
} // namespace SI
