#pragma once

#include "si_base/math/math_declare.h"

namespace SI
{
namespace Math
{
	void Print(Vfloat_arg    f, const char* tag="f");
	void Print(Vfloat3_arg   v, const char* tag="v");
	void Print(Vfloat4_arg   v, const char* tag="v");
	void Print(Vquat_arg     q, const char* tag="q");
	void Print(Vfloat3x3_arg m, const char* tag="m");
	void Print(Vfloat4x3_arg m, const char* tag="m");
	void Print(Vfloat4x4_arg m, const char* tag="m");

} // namespace Math
} // namespace SI
